
#pragma once

#include "DisplayConfig.h"

// This display interface will be used even if there is no display, but things will be replaced with
// no-ops. If you have an esp32 with an SSD1306 display, then you need to define the flag
// DISPLAY_SSD1306 before compiling this code. For platform.io, you would add something like this to
// your platform.ini:
//
// build_flags =
//     -D ESPBB_DISPLAY_SSD1306
//
#ifndef ESPBB_OLED_DISPLAY
    typedef int OLEDDisplay;
#endif // display

#include <functional>

namespace espbb
{

//! Initialize the display.
//! @param startThread. For some reason, the display is all garbled when I run it from a thread. The
//! workaround is to just set this to false, and call doDisplay().
void setDisplay(bool startThread = true);

typedef std::function<void(OLEDDisplay&)> DisplayFunc_t;

//! This sets a single idle display function that is used when the machine is up and running.
void setIdleFunction(const DisplayFunc_t& idleFunc);

//! This will add a function as a page.
void addPage(const DisplayFunc_t& pageFunc);

//! Force flip to the next page.
void nextPage();

//! Set the pages to rotate on their own.
//! @param period_seconds The amount of time between rotations (in seconds).
void autoRotatePages(double period_seconds);

//! Stop displaying the idle function or pages, and instead, display using this alert function.
void setAlert(const DisplayFunc_t& alertFunc);

//! Go back to using the idle or pages.
void clearAlert();

//! Call this instead of running in a thread
void doDisplay();

} // namespace

