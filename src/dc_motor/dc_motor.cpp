#include "dc_motor.hpp"

void dc_motor_init()
{
    pinMode(PIN_MOTOR_PWM, OUTPUT);

    

}

void dc_motor_set_power_100(uint8_t power)
{
    // La puissance est entre 0 et 100 ; on la map entre 0 et 255
    analogWrite(PIN_MOTOR_PWM, map(power, 0, 100, 0, 255));
}
