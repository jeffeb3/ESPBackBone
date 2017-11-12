
#pragma once

#include <Arduino.h>

namespace espbb
{

// Start the ntp service, and update it in a task. Use the formats below to retrieve the time.
void setupNtp();

// Increase the offset by an hour. Seems weird, but tie it to a button or something, it will work
// well.
void nextOffsetHour();

// Some formats to return the time.
String hour24();
String hour12();
String ampm();
String minutes();
String dayOfWeek();
int seconds();

} // namespace
