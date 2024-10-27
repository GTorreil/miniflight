
// Battery measurement pin.
#define PIN_VBAT A1
// Voltage divider factor. Calculate it from the voltage divider.
#define VBAT_MULTIPLIER 2
// Number of loops between two battery voltage updates.
#define VBAT_LOOPS_BETWEEN_UPDATES 1000

//
// SERVOS
//
#define BRUSHLESS_ESC_PIN 4
#define PIN_SERVO_ELEVATOR 2
#define PIN_SERVO_AILERON 0

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
