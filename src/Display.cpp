
#include <vector>

#include "Display.h"
#include "BBWiFi.h"
#include "MQTT.h"
#include "Debug.h"

#ifndef ESPBB_DISPLAY_TASK_PRIORITY
#define ESPBB_DISPLAY_TASK_PRIORITY 5
#endif


namespace espbb
{

// internal namespace for global variables
namespace internal
{

std::vector<espbb::DisplayFunc_t> idleFuncs;
size_t currentPage = 0;
espbb::DisplayFunc_t alertFunc;

SemaphoreHandle_t display_mutex = NULL;

void lock()
{
    xSemaphoreTake(display_mutex, portMAX_DELAY);
}

void unlock()
{
    xSemaphoreGive(display_mutex);
}

} // namespace

void resetDisplay()
{
#ifdef ESPBB_OLED_RESET
    pinMode(ESPBB_OLED_RESET,OUTPUT);
    digitalWrite(ESPBB_OLED_RESET,LOW); // reset OLED
    delay(50);
    digitalWrite(ESPBB_OLED_RESET,HIGH); // Enable OLED
#endif // LORA
}

void setDisplay()
{
    if (NULL == internal::display_mutex)
    {
        internal::display_mutex = xSemaphoreCreateMutex();
    }

    internal::lock();

#ifdef ESPBB_OLED_DISPLAY
    static auto display = ESPBB_OLED_DISPLAY(ESPBB_OLED_ADDRESS,
                                             ESPBB_OLED_SDA,
                                             ESPBB_OLED_SCL);

    // Initialising the UI will init the display too.
    resetDisplay();
    display.init();
    display.flipScreenVertically();

    static TaskHandle_t display_task = NULL;

    if (NULL != display_task)
    {
        vTaskDelete(display_task);
        display_task = NULL;
    }

    xTaskCreate(
        [](void*)
        {
            debugPrintln("[DISP]:\tStarting Display thread");
            while(1) // forever
            {
                internal::lock();
                // clear the display
                display.clear();

                if (internal::alertFunc)
                {
                    internal::alertFunc(display);
                }
                else
                {
                    if (not internal::idleFuncs.empty())
                    {
                        size_t page = internal::currentPage % internal::idleFuncs.size();
                        internal::idleFuncs[page](display);
                    }
                }

                // Draw the ip
                display.setTextAlignment(TEXT_ALIGN_LEFT);
                display.setFont(ArialMT_Plain_10);
                if (not espbb::connected())
                {
                    display.drawString(0, 54, "~m " + espbb::getWiFiIp());
                }
                else
                {
                    display.drawString(0, 54, espbb::getWiFiIp());
                }
                if (internal::idleFuncs.size() > 1)
                {
                    String dots;
                    for (size_t id=0; id<internal::idleFuncs.size(); ++id)
                    {
                        if (id == internal::currentPage)
                        {
                            dots += ":";
                        }
                        else
                        {
                            dots += ".";
                        }
                    }
                    display.drawString(64, 54, dots);
                }

                // Make a stinky timer.
                display.setFont(ArialMT_Plain_10);
                display.setTextAlignment(TEXT_ALIGN_RIGHT);
                display.drawString(128, 54, String(static_cast<int>(millis() / 10)/100.0));

                // write the buffer to the display
                display.display();

                internal::unlock();
                vTaskDelay(40/portTICK_PERIOD_MS);
            }

            debugPrintln("[DISP]:\tAAH! I should never leave this display thread!");

        }, "Display", 4096, NULL, ESPBB_DISPLAY_TASK_PRIORITY, &display_task);

#endif // display
    internal::unlock();
}

void setIdleFunction(const DisplayFunc_t& idleFunc)
{
    internal::lock();
    internal::idleFuncs.clear();
    addPage(idleFunc);
    internal::unlock();
}

void addPage(const DisplayFunc_t& pageFunc)
{
    internal::lock();
    internal::idleFuncs.push_back(pageFunc);
    internal::unlock();
}

void nextPage()
{
    internal::lock();
    internal::currentPage = (internal::currentPage+1) % internal::idleFuncs.size();
    internal::unlock();
}

void autoRotatePages(double a_period_seconds)
{
    internal::lock();
    static TaskHandle_t rotate_task = NULL;
    static double period_seconds = a_period_seconds;

    if (NULL != rotate_task)
    {
        vTaskDelete(rotate_task);
        rotate_task = NULL;
    }

    xTaskCreate(
        [](void*)
        {
            debugPrintln("[DISP]:\tStarting Rotate thread");
            while(1) // forever
            {
                vTaskDelay(static_cast<int>(period_seconds * 1000/portTICK_PERIOD_MS));
                nextPage();
            }

            debugPrintln("[DISP]:\tAAH! I should never leave this Rotate thread!");

        }, "DisplayRotate", 4096, NULL, 20, &rotate_task);

    internal::unlock();
}

void setAlert(const DisplayFunc_t& a_alertFunc)
{
    internal::lock();
    internal::alertFunc = a_alertFunc;
    internal::unlock();
}

void clearAlert()
{
    internal::lock();
    internal::alertFunc = DisplayFunc_t();
    internal::unlock();
}

} // namespace
