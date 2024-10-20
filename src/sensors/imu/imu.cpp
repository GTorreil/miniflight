#include "imu.hpp"

MPU6050 mpu(Wire);

void imu_init()
{
    Wire.begin();
    byte status = mpu.begin();
    while (status != 0)
    {
        // Error handling: IMU initialization failed
        Serial.println(F("IMU initialization failed. Check your connections."));
        delay(1000);
        status = mpu.begin();
    }

    Serial.println(F("IMU initialized successfully."));

    // Calibrate IMU
    Serial.println(F("Calibrating IMU. Do not move the device."));
    delay(1000);
    mpu.calcOffsets();
    Serial.println(F("IMU calibration complete."));
}

void imu_loop()
{
    mpu.update();
}

void imu_get_accel(float *x, float *y, float *z)
{
    *x = mpu.getAccX();
    *y = mpu.getAccY();
    *z = mpu.getAccZ();
}

void imu_get_gyro(float *yaw, float *pitch, float *roll)
{
    *yaw = mpu.getAngleZ();
    *pitch = mpu.getAngleY();
    *roll = mpu.getAngleX();
}
