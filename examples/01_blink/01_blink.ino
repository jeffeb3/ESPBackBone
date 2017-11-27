
#include <ESP32BackBone.h>

void setup() {

    // "Required" before starting anything else in the backbone.
    espbb::setup();

    // For the Status LED
    pinMode(25,OUTPUT);
}

void loop() {

    digitalWrite(25,HIGH);
    debugPrintln("[MAIN]:\tOn");

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);

    debugPrintln("[MAIN]:\tOff");
    // This will slow down the loop(), This will sleep for 10 seconds, so the loop will run a little
    // slower than 10 seconds.
    digitalWrite(25,LOW);

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
}
