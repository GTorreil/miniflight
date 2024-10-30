#ifndef ANGLES_HPP
#define ANGLES_HPP

#include "Arduino.h"

float unwrap_angle(float current_angle, float last_angle);
uint8_t norm_to_angle(float norm);

#endif