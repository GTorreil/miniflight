#include "battery.hpp"

void battery_init()
{
    pinMode(PIN_VBAT, INPUT);
}

float battery_read_voltage()
{

    return analogRead(PIN_VBAT) * 3.3f / 1024.0f * VBAT_MULTIPLIER; // Voltage divider factor

    // Serial.print("Battery voltage: ");
    // Serial.println(bat_voltage);
}
