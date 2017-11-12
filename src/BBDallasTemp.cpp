
#include "BBDallasTemp.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include "Debug.h"

#ifndef ESPBB_DALLAS_TEMP_TASK_PRIORITY
#define ESPBB_DALLAS_TEMP_TASK_PRIORITY 35
#endif

namespace espbb
{

String toString(const DeviceAddress& address)
{
    String rv;
    for (size_t i=0; i < 8; i++)
    {
        rv += String(address[i], HEX) + String(":");
    }
    return rv;
}

// This only supports one sensor at a time. With this interface, we could support more in the future
// if they were on separate pins. How would we support them if they were on the same wire?
void setDallasTemp(int pin, int a_resolution, const std::function<void(double)>& a_callback)
{
    // Static variables.
    static bool deviceFound = false;
    static DeviceAddress deviceAddress;

    static int resolution = 12;
    resolution = a_resolution;

    static OneWire wire(pin);
    static DallasTemperature sensors(&wire);

    static std::function<void(double)> callback = a_callback;

    static TaskHandle_t dt_task = NULL;

    // If this isn't the first time this has been called.
    if (NULL != dt_task)
    {
        vTaskDelete(dt_task);
        dt_task = NULL;
    }

    xTaskCreate(
        [](void*)
        {
            debugPrintln("[TEMP]:\tStarting Dallas Temp thread");
            while(1) // forever
            {
                if (not deviceFound)
                {
                    // Either we're not initialized, or somethings wrong, so just start from the
                    // beginning.
                    sensors.begin();
                    sensors.setWaitForConversion(false);
                    sensors.setResolution(resolution);

                    if (sensors.getAddress(deviceAddress, 0) and sensors.validFamily(deviceAddress))
                    {
                        // We found a device, and the address makes sense.
                        deviceFound = true;
                        debugPrintln("[TEMP]:\tAddress: 0x" + toString(deviceAddress));
                    }
                }

                if (deviceFound)
                {
                    if (sensors.isConnected(deviceAddress))
                    {
                        // The sensor is still there. Let's request the temp.
                        sensors.requestTemperaturesByAddress(deviceAddress);
                        while(not sensors.isConversionComplete())
                        {
                            vTaskDelay(10/portTICK_PERIOD_MS);
                        }

                        double raw = sensors.getTempC(deviceAddress);
                        if (raw > -120 and raw < 84.9)
                        {
                            // We received a good value, send it to the user.
                            callback(DallasTemperature::toFahrenheit(raw));
                        }
                    }
                }

                vTaskDelay(100/portTICK_PERIOD_MS);
            }
            debugPrintln("[TEMP]:\tAAH! I should never leave this Dallas Temp thread!");
        }, "DallasTemp", 4096, NULL, ESPBB_DALLAS_TEMP_TASK_PRIORITY, &dt_task);
}

} // namespace

