
#pragma once

#include "Display.h"
#include "Debug.h"
#include "BBWiFi.h"
#include "BBOTA.h"
#include "BBNTP.h"
#include "BBDallasTemp.h"
#include "MQTT.h"

namespace espbb
{

// Generic stuff gets set up here. Call this from first, from setup()
void setup();

} // namespace

