#ifndef IMU_HPP
#define IMU_HPP

#include "Arduino.h"
#include "configuration.h"
#include <MPU6050_light.h>

void imu_init();
void imu_loop();
void imu_get_accel(float *x, float *y, float *z);
void imu_get_gyro(float *yaw, float *pitch, float *roll);

#endif