
#include <ESP32BackBone.h>

/// Display Example
///
/// Getting the platformio.ini right for this example is tricky. This shows one way to set up the
/// build_flags to include the display for the LORA esp32 module:
///
/// ~~~
/// [env:esp32]
/// platform = espressif32
/// board = esp32dev
/// framework = arduino
/// lib_deps =
///     ESPBackBone
///
/// build_flags =
///     -DBBDEBUG
///     -DESPBB_DISPLAY_LORA
/// ~~~
///
/// Replacing ESPBB_DISPLAY_LORA with ESPBB_DISPLAY_SSD1306 is suitable for other OLED boards, like
/// the "D-Duino 32" with the WeMos sticker on it.
///

// Global display data. Simple types.
String name = "ESPBB Demo";
int loop_count = 0;
int display_count = 0;

void setup() {

    // "Required" before starting anything else in the backbone.
    espbb::setup();

    // Initialize the display.
    espbb::setDisplay();

    // Add two "pages" to the display. One says "Hello" and one says "World".
    //
    // Page 1 also shows how to use global data on the page.
    //
    espbb::addPage(
        [](OLEDDisplay& display)
        {
            // Draw the name
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(0, 0, name);

            // Draw the loop count
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(128, 0, String(loop_count));

            // Draw the display count. Notice how this is updated in the display thread, so it's
            // much higher than the loop counter.
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setFont(ArialMT_Plain_10);
            display.drawString(128, 12, String(display_count));

            // Say "Hello"
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_24);
            display.drawString(0, 20, "Hello" );

            display_count += 1;
        });

    espbb::addPage(
        [](OLEDDisplay& display)
        {
            // Say "World"
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_24);
            display.drawString(0, 20, "World" );
        });

    // Start the pages rotating every 7.5 seconds
    espbb::autoRotatePages(7.5);
}

void loop() {

    // Count the number of loops, to illustrate the way the display is multithreaded.
    loop_count += 1;

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
}
