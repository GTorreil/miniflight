#ifndef IMU_HPP
#define IMU_HPP

#include "Arduino.h"
#include "configuration.h"
#include "Wire.h"

void imu_init();
void imu_loop();
void imu_get_attitude(float *yaw, float *pitch, float *roll);

bool imu_available();

#endif