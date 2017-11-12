
#include "BBWiFi.h"
#include "Debug.h"

#include <WiFi.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifndef ESPBB_WIFI_TASK_PRIORITY
#define ESPBB_WIFI_TASK_PRIORITY 30
#endif

namespace espbb
{

bool WiFiUp()
{
    return WiFi.status() == WL_CONNECTED;
}

String getWiFiIp()
{
    const auto ip = WiFi.localIP();
    return String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
}

void setWiFi(const String& a_ssid, const String& a_password)
{
    // globals
    static String ssid;
    static String wifi_password;

    // Set up WiFi
    ssid = a_ssid;
    wifi_password = a_password;
    WiFi.mode(WIFI_STA);

    // One object to rule them all.
    static TaskHandle_t wifi_task = NULL;

    // If this isn't the first time this has been called.
    if (NULL != wifi_task)
    {
        vTaskDelete(wifi_task);
        wifi_task = NULL;
        WiFi.disconnect();
    }

    xTaskCreate(
        [](void*)
        {
            constexpr int MIN_DELAY_MS(2 * 1000); // 2 seconds
            constexpr int MAX_DELAY_MS(3 * 60 * 1000); // 3 minutes
            int delay_ms(MIN_DELAY_MS);

            debugPrintln("[WiFi]:\tStarting WiFi thread");
            while(1) // forever
            {
                if (WiFiUp())
                {
                    // We aren't doing anything here. Let's just wait.
                    vTaskDelay(100/portTICK_PERIOD_MS);
                    continue;
                }

                debugPrint("[WiFi]:\tConnecting to ");
                debugPrintln(ssid.c_str());
                WiFi.begin(ssid.c_str(), wifi_password.c_str());

                // Wait for some result
                if (WiFi.waitForConnectResult() == WL_CONNECTED)
                {
                    debugPrint("[WiFi]:\tConnected to ");
                    debugPrintln(ssid.c_str());
                    delay_ms = MIN_DELAY_MS;
                }
                else
                {
                    // We need to reconnect
                    debugPrintln("[WiFi]:\tDisconnecting from WiFi");
                    WiFi.disconnect(); // tear down.

                    // You aren't very good at connecting to wifi. Maybe you should wait.
                    vTaskDelay(delay_ms/portTICK_PERIOD_MS);

                    // Make the delay longer next time
                    delay_ms *= 2;
                    if (delay_ms > MAX_DELAY_MS)
                    {
                        delay_ms = MAX_DELAY_MS;
                    }
                }
            }
            debugPrintln("[WiFi]:\tAAH! I should never leave this WiFi thread!");
        }, "WiFi", 4096, NULL, ESPBB_WIFI_TASK_PRIORITY, &wifi_task);
}

} // namespace

