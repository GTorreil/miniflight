#include "servos.hpp"

Servo servo_aileron;
Servo servo_elevator;
Servo servo_esc;

void servos_init()
{
    servo_aileron.setPeriodHertz(50);
    servo_aileron.attach(PIN_SERVO_AILERON);
    
    servo_elevator.setPeriodHertz(50);
    servo_elevator.attach(PIN_SERVO_ELEVATOR);

    servo_esc.setPeriodHertz(50);
    servo_esc.attach(BRUSHLESS_ESC_PIN);
}

void servos_aileron_set_usec(uint16_t usec)
{
    servo_aileron.writeMicroseconds(usec);
}

void servos_elevator_set_usec(uint16_t usec)
{
    servo_elevator.writeMicroseconds(usec);
}

void servos_esc_set_usec(uint16_t usec){
    servo_esc.writeMicroseconds(usec);
}
