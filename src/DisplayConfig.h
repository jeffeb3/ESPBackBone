
#pragma once

/// This file is intended to be included before any display code is. It will parse simple defines
/// from the user's platform.ini files, and create the flags and tags needed to manage a display (or
/// disable the display).
///

#ifdef ESPBB_DISPLAY_LORA
    #include "SSD1306.h"
    #define ESPBB_OLED_DISPLAY SSD1306
    #define ESPBB_OLED_ADDRESS 0x3c
    #define ESPBB_OLED_SDA     4
    #define ESPBB_OLED_SCL     15
    #define ESPBB_OLED_RESET   16
#endif

#ifdef ESPBB_DISPLAY_SSD1306
    #include "SSD1306.h"
    #define ESPBB_OLED_DISPLAY SSD1306
    #define ESPBB_OLED_ADDRESS 0x3c
    #define ESPBB_OLED_SDA     5
    #define ESPBB_OLED_SCL     4
    #undef ESPBB_OLED_RESET
#endif // display


