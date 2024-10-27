#include "imu.hpp"

MPU6050 mpu(Wire);

bool imu_initialized = false;

void imu_init()
{
    Wire.begin(8, 9);

    uint8_t retries = 0;
    byte status = mpu.begin();
    while (status != 0)
    {
        // Error handling: IMU initialization failed
        Serial.println(F("IMU initialization failed. Check your connections."));
        delay(200);
        status = mpu.begin();

        retries++;
        if (retries > 10)
        {
            Serial.println("IMU initialization failed 10 times. Giving up.");
            break;
        }
    }

    if (status == 0)
    {
        Serial.println(F("IMU initialized successfully."));
        imu_initialized = true;

        // Calibrate IMU
        Serial.println(F("Calibrating IMU. Do not move the device."));
        delay(1000);
        mpu.calcOffsets();
        Serial.println(F("IMU calibration complete."));
    }
}

void imu_loop()
{
    if (!imu_initialized)
        return;

    mpu.update();
}

void imu_get_accel(float *x, float *y, float *z)
{
    if (!imu_initialized)
    {
        *x = 0;
        *y = 0;
        *z = 0;
    }
    else
    {
        *x = mpu.getAccX();
        *y = mpu.getAccY();
        *z = mpu.getAccZ();
    }
}

void imu_get_gyro(float *yaw, float *pitch, float *roll)
{
    if (!imu_initialized)
    {
        *yaw = 0;
        *pitch = 0;
        *roll = 0;
    }
    else
    {
        *yaw = mpu.getAngleZ();
        *pitch = mpu.getAngleY();
        *roll = mpu.getAngleX();
    }
}
