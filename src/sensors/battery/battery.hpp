#ifndef BATTERY_HPP
#define BATTERY_HPP

#include "Arduino.h"
#include "configuration.h"

void battery_init();
float battery_read_voltage();

#endif
