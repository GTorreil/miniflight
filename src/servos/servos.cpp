#include "servos.hpp"

Pwm pwm = Pwm();

void servos_init()
{

    // Write failsafe values
    servos_aileron_set_angle(FAILSAFE_AILERON_POSITION);
    servos_elevator_set_angle(FAILSAFE_ELEVATOR_POSITION);
    servos_esc_set_angle(FAILSAFE_THROTTLE);
}

void servos_aileron_set_angle(uint8_t angle)
{
    pwm.writeServo(PIN_SERVO_AILERON, angle);
}

void servos_elevator_set_angle(uint8_t angle)
{
    pwm.writeServo(PIN_SERVO_ELEVATOR, angle);
}

void servos_esc_set_angle(uint8_t angle)
{

    // Clamp the value
    if (angle > 180)
    {
        angle = 180;
    }
    else if (angle < 40)
    {
        angle = 40;
    }

    pwm.writeServo(BRUSHLESS_ESC_PIN, angle);
}
