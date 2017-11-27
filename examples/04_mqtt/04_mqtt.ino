
#include <ESP32BackBone.h>

void setup() {

    // "Required" before starting anything else in the backbone.
    espbb::setup();

    // Configure the wifi using build flags (or you can replace these flags with your wifi info, as
    // strings like "ssid", "secret".
    espbb::setWiFi(MY_SSID, MY_WIFI_PASSWORD);

    // Configure OTA.
    String hostname = "OTAExample";
    espbb::setOta(MY_OTA_PASSWORD, hostname.c_str());

    // Configure Mqtt
    espbb::setMqtt(IPAddress(MY_MQTT_ADDRESS), "ESPBB_MQTT");

    // Test with:
    //    mosquitto_pub -h 10.0.2.50 -t /echo -m "Hello from the server."
    espbb::subscribe(String("/echo"),
        [](byte* payload, unsigned int length)
        {
            // If the payload isn't a string, it will read forever.
            payload[length] = '\0';
            debugPrintln("[Main]:\tMQTT Rx: " + String((char*)payload));
        });
    // Turn on/off the light with:
    //    mosquitto_pub -h 10.0.2.50 -t /led -m "1"
    //    mosquitto_pub -h 10.0.2.50 -t /led -m "0"
    espbb::subscribe(String("/something"),
        [](byte* payload, unsigned int length)
        {
            if (int((char*)payload) == 1)
            {
                digitalWrite(25, HIGH);
            }
            else if (int((char*)payload) == 0)
            {
                digitalWrite(25, LOW);
            }
        });

    // LED controlled by mqtt
    pinMode(25,OUTPUT);

    // Press to publish
    pinMode(0,INPUT_PULLUP);
}

void loop() {

    static bool pressed = false;

    if (LOW == digitalRead(0))
    {
        if (not pressed)
        {
            espbb::publish("/button", "Pressed");
            pressed = true;
        }
    }
    else
    {
        if (pressed)
        {
            espbb::publish("/button", "Released");
            pressed = false;
        }
    }

    // This is a slower sleep, to make the button reading faster.
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
