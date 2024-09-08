#ifndef DC_MOTOR_HPP
#define DC_MOTOR_HPP

#include "Arduino.h"
#include "configuration.h"

void dc_motor_init();
void dc_motor_set_power_100(uint8_t power);

#endif