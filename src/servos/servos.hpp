#ifndef SERVOS_HPP
#define SERVOS_HPP

#include "Arduino.h"
#include "configuration.h"
#include <ESP32Servo.h>

void servos_init();
void servos_aileron_set_usec(uint16_t usec); 
void servos_elevator_set_usec(uint16_t usec);
void servos_esc_set_usec(uint16_t usec);

#endif