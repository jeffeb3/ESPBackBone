
#include <ESP32BackBone.h>

String name = "ESPBB NTP";
String timeString = "--:--";
String ampm = "?";
String day = "Noneday";
int seconds = 59;

void setup() {

    // "Required" before starting anything else in the backbone.
    espbb::setup();

    // Configure the wifi using build flags (or you can replace these flags with your wifi info, as
    // strings like "ssid", "secret".
    espbb::setWiFi(MY_SSID, MY_WIFI_PASSWORD);

    // Configure OTA.
    String hostname = "OTAExample";
    espbb::setOta(MY_OTA_PASSWORD, hostname.c_str());

    // Configure NTP (Network Time Protocol)
    espbb::setupNtp();

    // Initialize the display.
    espbb::setDisplay();

    // Just have one page, and it won't switch.
    espbb::addPage(
        [](OLEDDisplay& display)
        {
            // Draw the name
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, 0, name);

            // Draw the day of the week
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(128, 0, name);

            // Draw the time
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setFont(ArialMT_Plain_16);
            display.drawString(64, 20, timeString);

            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_16);
            display.drawString(66, 20, ampm);

            //TODO Show how to make a progressbar/rectangle to illustrate the seconds
        });

    // Press to change offset
    pinMode(0,INPUT_PULLUP);
}

void loop() {

    static bool pressed = false;

    if (LOW == digitalRead(0))
    {
        if (not pressed)
        {
            espbb::nextOffsetHour();
            pressed = true;
        }
    }
    else
    {
        pressed = false;
    }

    timeString = espbb::hour12() + ":" + espbb::minutes();
    ampm = espbb::ampm();
    day = espbb::dayOfWeek();
    seconds = espbb::seconds();

    // This is a slower sleep, to make the button reading faster.
    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
}
