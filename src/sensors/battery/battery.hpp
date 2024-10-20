#ifndef BATTERY_HPP
#define BATTERY_HPP

#include "Arduino.h"
#include "configuration.h"

void battery_init();
void battery_loop();
float battery_get_voltage();

#endif
