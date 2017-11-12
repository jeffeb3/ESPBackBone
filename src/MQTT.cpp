
#include "MQTT.h"
#include "Debug.h"
#include "BBWiFi.h"

#include <PubSubClient.h>
#include <WiFiClient.h>

#include <memory>
#include <map>
#include <list>

#ifndef ESPBB_MQTT_TASK_PRIORITY
#define ESPBB_MQTT_TASK_PRIORITY 20
#endif

namespace espbb
{

// Internal namespace will keep "private" details from users.
namespace internal
{
    //Cache for connected value
    bool connected = false;

    // Keep things thread safe.
    SemaphoreHandle_t mqtt_mutex = NULL;

    // Collects functions to call based on the topic recieved.
    //  - String topic name
    //  - function: void (byte* payload, unsigned int length)
    std::map<String, espbb::CallbackFun_t> callbacks;

    // Collection of payloads to be sent.
    std::list<std::pair<String, String>> publishQueue;
} // namespace

void lock()
{
    xSemaphoreTake(internal::mqtt_mutex, portMAX_DELAY);
}

void unlock()
{
    xSemaphoreGive(internal::mqtt_mutex);
}

bool connected()
{
    return internal::connected;
}

void setMqtt(const IPAddress& address, const String& a_name, const int port)
{
    static std::shared_ptr<PubSubClient> mqtt;
    static WiFiClient wifi;
    static TaskHandle_t mqtt_task = NULL;

    if (NULL == internal::mqtt_mutex)
    {
        internal::mqtt_mutex = xSemaphoreCreateMutex();
    }

    lock();

    // If this isn't the first time this has been called.
    if (NULL != mqtt_task)
    {
        vTaskDelete(mqtt_task);
        mqtt_task = NULL;
        mqtt->disconnect();
    }

    // Unique name for this mqtt client (unique on the list of things connected to the mqtt server).
    static String name = "ESP32BackBone";
    name = a_name;

    mqtt.reset(new PubSubClient(wifi));
    mqtt->setServer(address, port);
    mqtt->setCallback(
        [](char* topic, byte* payload, unsigned int length)
        {
            lock();
            auto cb_iter = internal::callbacks.find(String(topic));
            if (cb_iter == internal::callbacks.end())
            {
                // No callback found
                return;
            }

            cb_iter->second(payload, length);
            unlock();
        });

    xTaskCreate(
        [](void*)
        {
            debugPrintln("[MQTT]:\tStarting MQTT thread");
            while(1) // forever
            {
                if (espbb::WiFiUp())
                {
                    if (not mqtt->connected())
                    {
                        if (mqtt->connect(name.c_str()))
                        {
                            // newly connected
                            debugPrintln("[MQTT]:\tMqtt Connected");
                            for (const auto& subscription : internal::callbacks)
                            {
                                mqtt->subscribe(subscription.first.c_str());
                            }
                        }
                        else
                        {
                            vTaskDelay(100/portTICK_PERIOD_MS);
                            continue;
                        }
                    }

                    lock();
                    for (const auto& payload : internal::publishQueue)
                    {
                        mqtt->publish(payload.first.c_str(), payload.second.c_str());
                        debugPrintln("[MQTT]:\tPublish: " + payload.first + ": " + payload.second);
                    }
                    internal::publishQueue.clear();
                    unlock();
                    mqtt->loop();
                    internal::connected = mqtt->connected();
                }
                vTaskDelay(10/portTICK_PERIOD_MS);
            }
            debugPrintln("[MQTT]:\tAAH! I should never leave this MQTT thread!");
        }, "MQTT", 4096, NULL, ESPBB_MQTT_TASK_PRIORITY, &mqtt_task);
    unlock();
}

void subscribe(const String& topic, const CallbackFun_t& callback)
{
    lock();
    internal::callbacks[topic] = callback;
    unlock();
}

void publish(const String& topic, const String& payload)
{
    lock();
    internal::publishQueue.emplace_back(std::make_pair(topic, payload));
    if (internal::publishQueue.size() > 50)
    {
        internal::publishQueue.pop_front();
    }
    unlock();
}

} // namespace

