
#include <BBNTP.h>
#include "Debug.h"

#include <NTPClient.h>
#include <Preferences.h>
#include <WiFiUdp.h>

#include <memory>

#ifndef ESPBB_NTP_TASK_PRIORITY
#define ESPBB_NTP_TASK_PRIORITY 15
#endif

namespace espbb
{

// The scope of this is only in this file.

std::unique_ptr<NTPClient> timeClient;
int offsetHour = 0;

namespace ntp
{

// Lock up the timeClient and offset.
SemaphoreHandle_t mutex = NULL;

void lock()
{
    if (NULL != mutex)
    {
        xSemaphoreTake(mutex, portMAX_DELAY);
    }
}

void unlock()
{
    if (NULL != mutex)
    {
        xSemaphoreGive(mutex);
    }
}

} // namespace

void setupNtp()
{
    ntp::mutex = xSemaphoreCreateMutex();
    ntp::lock();

    Preferences preferences;
    // setup
    preferences.begin("NTP", true);
    offsetHour = preferences.getInt("offsetHour", 0);
    preferences.end();

    static WiFiUDP ntpUDP;

    timeClient.reset(new NTPClient(ntpUDP, "pool.ntp.org", offsetHour, 60000));

    timeClient->begin();

    static TaskHandle_t task = NULL;

    // If this isn't the first time this has been called.
    if (NULL != task)
    {
        vTaskDelete(task);
        task = NULL;
    }

    xTaskCreate(
        [](void*)
        {
            debugPrintln("[NTP]:\tStarting NTP thread");
            while(1) // forever
            {
                ntp::lock();
                timeClient->setTimeOffset(offsetHour * 3600);
                timeClient->update();
                ntp::unlock();

                vTaskDelay(100/portTICK_PERIOD_MS);
            }
            debugPrintln("[NTP]:\tAAH! I should never leave this NTP thread!");
        }, "NTP", 4096, NULL, ESPBB_NTP_TASK_PRIORITY, &task);

    ntp::unlock();
}

void nextOffsetHour()
{
    ntp::lock();
    if (not timeClient)
    {
        ntp::unlock();
        return;
    }

    offsetHour += 1;
    if (offsetHour > 14)
    {
        offsetHour = -11;
    }

    debugPrintln(String("New NTP offset hour: ") + String(offsetHour));
    Preferences preferences;
    preferences.begin("NTP", false);
    preferences.putInt("offsetHour", offsetHour);
    preferences.end();

    ntp::unlock();
}

String hour24()
{
    ntp::lock();
    if (not timeClient)
    {
        ntp::unlock();
        return "--";
    }
    auto hours = timeClient->getHours();
    ntp::unlock();
    return String(hours);
}

String hour12()
{
    ntp::lock();
    if (not timeClient)
    {
        ntp::unlock();
        return "--";
    }
    int hours = timeClient->getHours() % 12;
    if (0 == hours)
    {
        hours = 12;
    }
    ntp::unlock();
    return String(hours);
}

String ampm()
{
    ntp::lock();
    if (not timeClient)
    {
        ntp::unlock();
        return "am";
    }
    auto hours = timeClient->getHours();
    ntp::unlock();

    if (hours > 12)
    {
        return "pm";
    }
    else
    {
        return "am";
    }
}

String minutes()
{
    ntp::lock();
    if (not timeClient)
    {
        ntp::unlock();
        return "--";
    }
    auto minutes = timeClient->getMinutes();
    ntp::unlock();

    return (minutes < 10 ? "0" + String(minutes) : String(minutes));
}

String dayOfWeek()
{
    ntp::lock();
    if (not timeClient)
    {
        ntp::unlock();
        return "Funday";
    }
    auto day = timeClient->getDay();
    ntp::unlock();

    switch(day)
    {
        case 0:  return "Sunday";
        case 1:  return "Monday";
        case 2:  return "Tuesday";
        case 3:  return "Wednesday";
        case 4:  return "Thursday";
        case 5:  return "Friday";
        case 6:  return "Saturday";
        default: return "Noneday";
    }
}

int seconds()
{
    ntp::lock();
    if (not timeClient)
    {
        ntp::unlock();
        return 0;
    }
    auto seconds = timeClient->getSeconds();
    ntp::unlock();
    return seconds;
}

} // namespace
