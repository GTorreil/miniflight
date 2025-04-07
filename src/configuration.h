
// Battery measurement pin.
#define PIN_VBAT A1
// Voltage divider factor. Calculate it from the voltage divider.
#define VBAT_MULTIPLIER 2
// Number of loops between two battery voltage updates.
#define VBAT_LOOPS_BETWEEN_UPDATES 1000

//
// SERVOS
//
#define PIN_ESC_1 4
#define PIN_SERVO_ELEVATOR 2
#define PIN_SERVO_FLAPERON_0 0
#define PIN_SERVO_FLAPERON_1 1

//
// Flaps
//

#define FLAP_MIN_ANGLE 0.0f
#define FLAP_MAX_ANGLE 0.65f

//
// TELEMETRY
//

#define TELEMETRY_INTERVAL_MS 200 // ms

//
// FAILSAFE
//
#define FAILSAFE_AILERON_POSITION 90  // Servo degrees
#define FAILSAFE_ELEVATOR_POSITION 90 // Servo degrees
#define FAILSAFE_THROTTLE 0           // Servo degrees
#define FAILSAFE_AUX1 90              // Servo degrees
#define FAILSAFE_AUX2 90              // Servo degrees
#define FAILSAFE_AUX3 90              // Servo degrees
#define FAILSAFE_AUX4 90              // Servo degrees

//
// IMU
//

// MPU6050 I2C address
// AD0 Low -> 0x68
// AD0 High -> 0x69
#define MPU6050_ADDRESS 0x68
// MPU6050 interrupt pin
#define MPU_INTERRUPT_PIN 10
