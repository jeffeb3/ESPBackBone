
#include <ESP32BackBone.h>

String name = "ESPBB TEMP";
double temperatureF(0.0);

void setup() {

    // "Required" before starting anything else in the backbone.
    espbb::setup();

    // Configure the wifi using build flags (or you can replace these flags with your wifi info, as
    // strings like "ssid", "secret".
    espbb::setWiFi(MY_SSID, MY_WIFI_PASSWORD);

    // Configure OTA.
    String hostname = "OTAExample";
    espbb::setOta(MY_OTA_PASSWORD, hostname.c_str());

    // Initialize the display.
    espbb::setDisplay();

    // Set up the temperature sensor
    constexpr int one_wire_pin = 5;
    constexpr int resolution = 10;
    espbb::setDallasTemp(one_wire_pin, resolution,
        [](double tempF)
        {
            temperatureF = tempF;
        });

    // Just have one page, and it won't switch.
    espbb::addPage(
        [](OLEDDisplay& display)
        {
            // Draw the name
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, 0, name);

            // Draw the time
            display.setTextAlignment(TEXT_ALIGN_CENTER);
            display.setFont(ArialMT_Plain_24);
            display.drawString(64, 40, String(temperatureF) + " F");
        });
}

void loop() {

    // This is a slower sleep, to make the button reading faster.
    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
}
