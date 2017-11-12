
#include "Debug.h"
#include <freertos/FreeRTOS.h>

namespace espbb
{
namespace debug
{

namespace internal
{
    // The scope of this is only in this file.
    SemaphoreHandle_t debug_mutex = NULL;
} // namespace

void setup()
{
    #ifdef BBDEBUG
        internal::debug_mutex = xSemaphoreCreateMutex();
        Serial.begin(115200);
    #endif // BBDEBUG
}

void lock()
{
    // Only lock if we have something to lock
    #ifdef BBDEBUG
    xSemaphoreTake(internal::debug_mutex, portMAX_DELAY);
    #endif // BBDEBUG
}

void unlock()
{
    #ifdef BBDEBUG
    xSemaphoreGive(internal::debug_mutex);
    #endif // BBDEBUG
}

} // namespace
} // namespace
