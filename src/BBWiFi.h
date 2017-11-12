
#pragma once

#include <Arduino.h>

namespace espbb
{

// Returns true if the wifi is connected and working (as far as I can tell).
bool WiFiUp();

// Returns a String object with the current IP address in it. It is 0.0.0.0 if it's not connected.
String getWiFiIp();

// Start the wifi client service, which will set up wifi with reasonable defaults, and spawn a task
// to manage the connection and try to reconnect if there's a problem.
void setWiFi(const String& ssid, const String& password);

} // namespace

