
#include "ESP32BackBone.h"
#include "BBOTA.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace espbb
{

void setup()
{
    espbb::debug::setup();
}

} // namespace
