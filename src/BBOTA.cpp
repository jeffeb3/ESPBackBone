
#include "BBOTA.h"
#include "BBWiFi.h"
#include "Debug.h"
#include "Display.h"

#include <Arduino.h>
#include <ArduinoOTA.h>

#ifndef ESPBB_OTA_TASK_PRIORITY
#define ESPBB_OTA_TASK_PRIORITY 25
#endif

namespace espbb
{

void setOta(const char* password, const char* hostname)
{
    // Set up OTA
    ArduinoOTA.onStart([]()
        {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
            {
                type = "sketch";
            }
            else // U_SPIFFS
            {
                type = "filesystem";
            }

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            debugPrintln("[OTA]:\tStart updating " + type);
        });
    ArduinoOTA.onEnd([]()
        {
            debugPrintln("");
            debugPrintln("[OTA]:\tDone updating");
            WiFi.disconnect();

            #ifdef ESPBB_OLED_DISPLAY
            espbb::setAlert(
                [](OLEDDisplay& display)
                {
                    display.setTextAlignment(TEXT_ALIGN_LEFT);
                    display.setFont(ArialMT_Plain_24);
                    display.drawString(0, 0, "OTA Done");
                });
            #endif // display

            int timeout = 0;
            while(WiFi.status() != WL_DISCONNECTED && timeout < 200){
                delay(10);
                timeout++;
            }
            // clear the display
            espbb::clearAlert();
        });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
        {
            #ifdef ESPBB_OLED_DISPLAY
            espbb::setAlert(
                [progress, total](OLEDDisplay& display)
                {
                    display.setTextAlignment(TEXT_ALIGN_LEFT);

                    display.setFont(ArialMT_Plain_24);
                    display.drawString(0, 0, "OTA Up");
                    display.setFont(ArialMT_Plain_16);
                    display.drawString(0, 27, String(progress/1000.0) + "/" + String(total/1000.0));
                    display.drawProgressBar(0, 46, 127, 5, progress / (total / 100));
                });
            #endif // display

            debugPrint("[OTA]:\tProgress: " + String((progress / (total / 100))) + "%\r");
        });
    ArduinoOTA.onError([](ota_error_t error)
        {
            debugPrintln("\n[OTA]:\tOTA Error: " + String(error));
            if (error == OTA_AUTH_ERROR)
            {
                debugPrintln("[OTA]:\tAuth Failed");
            }
            else if (error == OTA_BEGIN_ERROR)
            {
                debugPrintln("[OTA]:\tBegin Failed");
            }
            else if (error == OTA_CONNECT_ERROR)
            {
                debugPrintln("[OTA]:\tConnect Failed");
            }
            else if (error == OTA_RECEIVE_ERROR)
            {
                debugPrintln("[OTA]:\tReceive Failed");
            }
            else if (error == OTA_END_ERROR)
            {
                debugPrintln("[OTA]:\tEnd Failed");
            }

            // clear the display
            espbb::clearAlert();
        });

    ArduinoOTA.setPassword(password);
    ArduinoOTA.setHostname(hostname);
    ArduinoOTA.setPort(3232);

    static TaskHandle_t ota_task = NULL;

    // If this isn't the first time this has been called.
    if (NULL != ota_task)
    {
        vTaskDelete(ota_task);
        ota_task = NULL;
    }

    xTaskCreate(
        [](void*)
        {
            debugPrintln("[OTA]:\tStarting OTA thread");
            while(1) // forever
            {
                if (espbb::WiFiUp())
                {
                    ArduinoOTA.begin(); // no-op after first time
                    ArduinoOTA.handle();
                }
                vTaskDelay(500/portTICK_PERIOD_MS);
            }
            debugPrintln("[OTA]:\tAAH! I should never leave this OTA thread!");
        }, "ArduinoOTA", 4096, NULL, ESPBB_OTA_TASK_PRIORITY, &ota_task);
}

} // namespace

