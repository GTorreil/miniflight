
// Pin de mesure de la tension de la batterie.
#define PIN_VBAT A1
// Facteur appliqué à la tension brute. Calculez-le à partir du pont diviseur.
#define VBAT_MULTIPLIER 2
// Nombre de boucles entre deux mises à jour de la tension de la batterie.
#define VBAT_LOOPS_BETWEEN_UPDATES 1000

// 
// SERVOS
// 
#define PIN_SERVO_AILERON 1
#define PIN_SERVO_ELEVATOR 2
#define BRUSHLESS_ESC_PIN 4

// 
// FAILSAFE
// 
#define FAILSAFE_AILERON_POSITION 1500 // uSec
#define FAILSAFE_ELEVATOR_POSITION 1500 // uSec
#define FAILSAFE_THROTTLE 990 // uSec
#define FAILSAFE_AUX1 1500 // uSec
#define FAILSAFE_AUX2 1500 // uSec
#define FAILSAFE_AUX3 1500 // uSec
#define FAILSAFE_AUX4 1500 // uSec
