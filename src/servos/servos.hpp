#ifndef SERVOS_HPP
#define SERVOS_HPP

#include "Arduino.h"
#include "configuration.h"
#include <pwmWrite.h>

void servos_init();

void servos_elevator_set_angle(uint8_t angle);
void servos_esc_set_angle(uint8_t angle);

void servos_flaperon_0_set_angle(uint8_t angle);
void servos_flaperon_1_set_angle(uint8_t angle);

#endif