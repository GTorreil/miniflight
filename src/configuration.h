
// Pin de mesure de la tension de la batterie.
#define PIN_VBAT A1
// Facteur appliqué à la tension brute. Calculez-le à partir du pont diviseur.
#define VBAT_MULTIPLIER 2
// Nombre de boucles entre deux mises à jour de la tension de la batterie.
#define VBAT_LOOPS_BETWEEN_UPDATES 1000

// 
// Moteur DC
// 
#define PIN_MOTOR_PWM 4

// 
// SERVOS
// 
#define PIN_SERVO_AILERON 1
#define PIN_SERVO_ELEVATOR 2

// 
// FAILSAFE
// 
#define FAILSAFE_AILERON_POSITION 1500 // uSec
#define FAILSAFE_ELEVATOR_POSITION 1500 // uSec
#define FAILSAFE_THROTTLE 0 // 0-100