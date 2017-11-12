
#pragma once

#include <Arduino.h>

#include <functional>

namespace espbb
{

/// Start the service to look for a single Dallas Temperature (DS18B20 or similar).
///
/// @note This starts a task that will manage the searching, finding, error checking, etc. and just
/// call the onValue function if/when a good temperature is found. If you need finer grain control,
/// I suggest going straight to the OneWire or DallasTemperature library itself.
///
/// @note When I wrote this comment, the main OneWire did not work well on the esp. This is due
/// (mostly) to the fact that the OneWire library uses delay, and not delayMicroseconds to control
/// timing. This is erratic, due to the multithreaded nature of the esp32. This One Wire fork works
/// much better:
///     https://github.com/orgua/OneWire
///
/// @param pin The esp pin with the temperature sensor attached.
///
/// @param resolution the resolution (9-12) for the measurement. Measurements happen quicker with
/// lower resolution. 9 bit is 0.5C and 12 bit is 0.0625C.
///
/// @param onValue a function that will be called when there is a valid temp. It can be something as
/// simple as storing the value in a global variable:
///
/// setDallasTemp(25, 12,
///     [](double tempF)
///     {
///         temperature = tempF;
///     });
///
void setDallasTemp(int pin, int resolution, const std::function<void(double)>& onValue);

} // namespace

