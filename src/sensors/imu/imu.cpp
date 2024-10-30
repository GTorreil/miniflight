#include "imu.hpp"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu(MPU6050_ADDRESS);

// Device vars
bool imu_ready = false; // indicates if the IMU is ready to be used
uint8_t dev_status = 0; // status after each device operation (0 = success, !0 = error)

volatile bool interrupt_available = false;

void interrupt_handler()
{
    interrupt_available = true;
}

uint8_t int_status;     // holds the actual interrupt status byte from MPU
uint16_t packet_size;   // expected DMP packet size (default is 42 bytes)
uint16_t fifo_count;    // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// Motion vars
Quaternion q;
VectorFloat gravity;
float ypr[3];
float ypr_deg[3];

void i2c_init()
{
    Wire.begin(8, 9);
    Wire.setClock(400000); // 400kHz I2C clock.
}

void imu_init()
{
    i2c_init();

    // initialize device
    mpu.initialize();
    pinMode(MPU_INTERRUPT_PIN, INPUT);

    // verify connection
    const bool success = mpu.testConnection();
    if (!success)
    {
        // Mark the IMU as unavailable and proceed
        imu_ready = false;
        Serial.println(F("[IMU] IMU chip not detected ; stabilization disabled"));
        return;
    }

    // load and configure the DMP
    dev_status = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    // mpu.setXGyroOffset(220);
    // mpu.setYGyroOffset(76);
    // mpu.setZGyroOffset(-85);
    // mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (dev_status == 0)
    {
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);

        // Config Digital Low Pass Filter -> See table on getDLPFMode()
        mpu.setDLPFMode(3); // 0 = 260Hz, 1 = 184Hz, 2 = 94Hz, 3 = 44Hz, 4 = 21Hz, 5 = 10Hz, 6 = 5Hz
        // Gyroscope sample rate divider (divides 1KHz by that number + 1 when DLPF is ON)
        mpu.setRate(0); // 0 = 1kHz, 1 = 500Hz, 2 = 333Hz, 3 = 250Hz, 4 = 200Hz, 5 = 167Hz, 6 = 143Hz, 7 = 125Hz

        // Serial.println("DLPF Mode: " + String(mpu.getDLPFMode()));
        // Serial.println("Sample Rate: " + String(mpu.getRate()));

        // Enable interrupt
        interrupt_available = false;
        attachInterrupt(digitalPinToInterrupt(MPU_INTERRUPT_PIN), interrupt_handler, RISING);
        int_status = mpu.getIntStatus();

        // Mark the IMU as ready
        imu_ready = true;

        // Note the expected packet size (for checking if we received a full packet)
        packet_size = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        // Error table :
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        Serial.print(F("DMP init failed ; code "));
        Serial.println(dev_status);

        // Mark the IMU as unavailable and proceed
        imu_ready = false;
        return;
    }
}

void imu_loop()
{
    // Not ready -> do nothing
    if (!imu_ready)
        return;

    // Wait for a complete packet
    if (!interrupt_available && fifo_count < packet_size)
        return;

    // Reset interrupt flag and get INT_STATUS byte
    interrupt_available = false;
    int_status = mpu.getIntStatus();
    fifo_count = mpu.getFIFOCount();

    // Check if fifo is full (this should never happen if the code is fast enough)
    if ((int_status & 0x10) || fifo_count == 1024)
    {
        mpu.resetFIFO();
        Serial.println(F("[IMU] FIFO overflow!"));
    }

    // Let's check if the DMP has data available (this should happen very often)
    else if (int_status & 0x02)
    {
        // Wait for the packet to be fully available, this should be **very** quick
        while (fifo_count < packet_size)
            fifo_count = mpu.getFIFOCount();

        // Read one packet from the FIFO
        mpu.getFIFOBytes(fifoBuffer, packet_size);
        fifo_count -= packet_size;

        // Read attitude data
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);

        // Convert the quaternion to Euler angles
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        // Convert the angles to degrees
        ypr_deg[0] = ypr[0] * 180.0 / M_PI;
        ypr_deg[1] = ypr[1] * 180.0 / M_PI;
        ypr_deg[2] = ypr[2] * 180.0 / M_PI;
    }
}

void imu_get_attitude(float *yaw, float *pitch, float *roll)
{
    if (!imu_available)
    {
        return;
    }
    else
    {
        *yaw = ypr_deg[0];
        *pitch = ypr_deg[1];
        *roll = ypr_deg[2];
    }
}

bool imu_available()
{
    return imu_ready;
}
