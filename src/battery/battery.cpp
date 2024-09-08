#include "battery.hpp"

/** Tension de batterie */
float bat_voltage = 0;

/** Nombre de boucles depuis la dernière mise à jour de la tension de la batterie */
uint16_t loops_since_bat_voltage_update = 0;

void battery_init()
{
    pinMode(PIN_VBAT, INPUT);
}

void read_battery()
{
    float raw_voltage = analogRead(PIN_VBAT) * 3.3 / 1024;
    bat_voltage = raw_voltage * VBAT_MULTIPLIER; // Facteur du diviseur de tension
    loops_since_bat_voltage_update = 0;
}

void battery_loop()
{
    if (loops_since_bat_voltage_update > VBAT_LOOPS_BETWEEN_UPDATES)
    {
        read_battery();
    }
    loops_since_bat_voltage_update++;
}

float battery_get_voltage()
{
    return bat_voltage;
}
