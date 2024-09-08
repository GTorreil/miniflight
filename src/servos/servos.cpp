#include "servos.hpp"



Servo servo_aileron;
Servo servo_elevator;

void servos_init()
{
    servo_aileron.setPeriodHertz(50);
    servo_aileron.attach(PIN_SERVO_AILERON);
    servo_elevator.setPeriodHertz(50);
    servo_elevator.attach(PIN_SERVO_AILERON);
}

void servos_aileron_set_usec(uint16_t usec)
{
    servo_aileron.writeMicroseconds(usec);
}

void servos_elevator_set_usec(uint16_t usec)
{
    servo_elevator.writeMicroseconds(usec);
}
