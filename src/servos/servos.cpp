#include "servos.hpp"

Pwm pwm = Pwm();

unsigned long init_time = 0;
bool enable_outputs = false;

void servos_init()
{
    // Write failsafe values
    // servos_aileron_set_angle(FAILSAFE_AILERON_POSITION);
    // servos_elevator_set_angle(FAILSAFE_ELEVATOR_POSITION);
    // servos_esc_set_angle(FAILSAFE_THROTTLE);

    init_time = millis();
}

bool outputs_enabled()
{
    if (enable_outputs == true)
        return true;
    if (millis() - init_time > 3000)
    {
        enable_outputs = true;
        return true;
    }

    return false;
}

void servos_aileron_set_angle(uint8_t angle)
{
    if (outputs_enabled() == false)
        return;

    pwm.writeServo(PIN_SERVO_AILERON, angle);
}

void servos_elevator_set_angle(uint8_t angle)
{
    if (outputs_enabled() == false)
        return;

    pwm.writeServo(PIN_SERVO_ELEVATOR, angle);
}

void servos_esc_set_angle(uint8_t angle)
{

    if (outputs_enabled() == false)
        return;

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
