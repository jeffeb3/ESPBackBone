
#pragma once

#include <Arduino.h>

namespace espbb
{

/// Start the over the air service, which will listen for incoming requests to update the firmware
/// over wifi.
///     https://github.com/espressif/arduino-esp32/tree/master/libraries/ArduinoOTA
///
/// @note
/// This will set up ArduinoOTA, with some callbacks to do reasonable things, and call update
/// periodically in a separate task, with it's own timer. This is a helper for people who want
/// something set up, but don't really want anything specific. If you want any more functionality,
/// like getting updates with progress, then I suggest going straight to the ArduinoOTA class.
///
/// @param password Set to the same thing you use to upload via wifi.
/// @param hostname (optional) Set to something unique if you want to be able to reach it via mDNS.
///     Must Be Unique!
///
void setOta(const char* password, const char* hostname = "");

} // namespace

