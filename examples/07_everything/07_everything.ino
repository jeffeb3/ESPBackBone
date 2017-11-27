
#include <ESP32BackBone.h>

#define OW_SIG 5

double tempF(0.0);

String name = "BEDROOM";
String dayOfWeek = "Funday";
String timeString = "00:00";
String am = "a";

void setup() {

    // "Required" before starting anything else in the backbone.
    espbb::setup();

    // Configure the wifi using build flags (or you can replace these flags with your wifi info, as
    // strings like "ssid", "secret".
    espbb::setWiFi(MY_SSID, MY_WIFI_PASSWORD);

    // Configure OTA.
    espbb::setOta(MY_OTA_PASSWORD);

    // Configure Mqtt
    espbb::setMqtt(IPAddress(MY_MQTT_ADDRESS), name);

    // Configure NTP
    espbb::setupNtp();

    // This is a little fishy. Either you have the esp32 with an OLED display, in which case, you
    // would set the DISPLAY_SSD1306 flag, and wouldn't need to check it, or you don't have the
    // display, in which case, you wouldn't write this at all. For the sake of this example, I have
    // written it, and put a flag around it. Sorry, bad examples :(
    espbb::setDisplay();
    espbb::addPage(
        [](OLEDDisplay& display)
        {
            display.setColor(WHITE);
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, 0, name);
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(128, 0, dayOfWeek);

            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setFont(ArialMT_Plain_24);
            display.drawString(68, 20, timeString );
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_16);
            display.drawString(69, 26, am );


            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setFont(ArialMT_Plain_16);
            display.drawString(128, 28,  "F");
            display.drawString(118, 28,  String(tempF,1));

        });
    espbb::autoRotatePages(7.5);

    // Configure Dallas Temperature.
    pinMode(17, OUTPUT);
    digitalWrite(17, LOW);
    pinMode(18, OUTPUT);
    digitalWrite(18, HIGH);
    static auto lastMqttTime = millis();
    constexpr int resolution = 10;
    espbb::setDallasTemp(OW_SIG, resolution,
        [](double temp)
        {
            tempF = temp;
            if ((millis() - lastMqttTime) > 1 * 1000)
            {
                espbb::publish("/evilhouse/ESP32/temperature", String(temp));
                lastMqttTime = millis();
            }
        });

    // For the Status LED
    pinMode(25,OUTPUT);

    pinMode(0, INPUT_PULLUP);
    delay(1000);
}

void loop() {

    digitalWrite(25,HIGH);

    timeString = espbb::hour12() + ":" + espbb::minutes();
    am = espbb::ampm().substring(0,1);
    dayOfWeek = espbb::dayOfWeek();

    if (0 == digitalRead(0))
    {
        espbb::nextOffsetHour();
    }

    // This will slow down the loop(), This will sleep for 10 seconds, so the loop will run a little
    // slower than 10 seconds.
    digitalWrite(25,LOW);

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
}
