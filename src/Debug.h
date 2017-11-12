
#pragma once

#include <Arduino.h>

// This code is used to debug the backbone stuff and can also be used in user land. For now, the
// only supported interface is the Serial library. Eventually, I could see someone using this
// interface to manage a tcpdump-like socket to also provide feedback over the air.
//
// This interface promises to be thread safe (you're welcome) and should be very similar in use to
// Serial's interface.
//
// The ideal is to isolate people from the code, but with templates (and variadic templates) it's
// tough to do. So instead, here are some detailed examples on how to use these debug functions:
//
// First, If you want to get status on the serial port, then -DBBDEBUG in the build flags i.e. (for
// platform.io (platform.ini):
//
// build_flags =
//     -DBBDEBUG
//
// Secondly, you can then put information into the debug stream with things like this:
//
// debugPrintln("Some sort of state");
//
// debugPrintln("Label: " + String(data));
//
// debugPrint("Multi ");
// debugPrint(" line ");
// debugPrint(" stuff:");
// debugPrintln(data);
//
// Note: The locks will make sure that a single call (like one debugPrintln) will be thread safe, if
// you make multiple calls in a row (like with multiple debugPrint calls) then there is a chance
// that other debug statements will get through.
//
// Note: Do not call Serial.print or any of it's siblings directly. That will break the thread
// safety. (Note Note: If you haven't defined BBDEBUG, then this lib will leave Serial alone.
//
// Note: If you don't call setup() before calling any of these things, then undefined behavior
// (probably very bad) will happen. This is especially possible if you call debugPrintln from any
// constructor, and you construct that object globally.

namespace espbb
{
namespace debug
{

// Sets up the debug environment, as configured by the build flags.
void setup();

// Waits for the debug to be available.
void lock();

// Releases the lock (mutex).
void unlock();

} // namespace
} // namespace

// -------------------------------------------
//  Public Functions - Sorry for the ugly, look at the examples above instead.
// -------------------------------------------
#ifdef BBDEBUG
template <typename... Arguments>
size_t debugPrint(Arguments... parameters)
{
    espbb::debug::lock();
    auto rv = Serial.print(parameters...);
    espbb::debug::unlock();
    return rv;
}

template <typename... Arguments>
size_t debugPrintln(Arguments... parameters)
{
    espbb::debug::lock();
    auto rv = Serial.println(parameters...);
    espbb::debug::unlock();
    return rv;
}
#else
    #define debugPrint(x) {;}
    #define debugPrintln(x) {;}
#endif

