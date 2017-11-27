
#include <ESP32BackBone.h>

/// To set the MY_SSID, MY_WIFI_PASSWORD and MY_OTA_PASSWORD, you can either replace the parts
/// below, or set the build_flags in platformio.ini:
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
///    -DMY_SSID=\"some_ssid\"
///    -DMY_WIFI_PASSWORD=\"secret\"
///    -DMY_OTA_PASSWORD=\"secret_2\"
/// ~~~
///
/// Then, once you have the software on the esp, you can add these lines to connect to flash via
/// wifi:
///
/// ~~~
/// upload_port = "OTAExample.local"
/// upload_flags = --auth=secret_2 --port 3232
/// ~~~

void setup() {

    // "Required" before starting anything else in the backbone.
    espbb::setup();

    // Configure the wifi using build flags (or you can replace these flags with your wifi info, as
    // strings like "ssid", "secret".
    espbb::setWiFi(MY_SSID, MY_WIFI_PASSWORD);

    // Configure OTA.
    String hostname = "OTAExample";
    espbb::setOta(MY_OTA_PASSWORD, hostname.c_str());

}

void loop() {

    vTaskDelay(1 * 1000 / portTICK_PERIOD_MS);
}
