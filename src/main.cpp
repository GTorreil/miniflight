/**
 * @file main.cpp
 */

#include "Arduino.h"
#include "CRSFforArduino.hpp"
#include "battery/battery.hpp"
#include "servos/servos.hpp"
#include "dc_motor/dc_motor.hpp"

CRSFforArduino *crsf = nullptr;

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels);

void setup()
{
  
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }

  crsf = new CRSFforArduino();

  if (!crsf->begin())
  {
    Serial.println("CRSF for Arduino failed to initialise.");

    delete crsf;
    crsf = nullptr;

    while (1)
    {
      delay(10);
    }
  }

  crsf->setRcChannelsCallback(onReceiveRcChannels);


  battery_init();
  servos_init();
  dc_motor_init();
}


void loop()
{
  crsf->update();
  battery_loop();
}

void write_telemetry()
{
  crsf->telemetryWriteBattery(battery_get_voltage(), 0.0f, 0.0f, 0.0f);
}

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels)
{
  static unsigned long lastPrint = millis();
  if (millis() - lastPrint >= 100)
  {
    lastPrint = millis();

    static bool initialised = false;
    static bool lastFailSafe = false;
    if (rcChannels->failsafe != lastFailSafe || !initialised)
    {
      initialised = true;
      lastFailSafe = rcChannels->failsafe;
      // Serial.print("FailSafe: ");
      // Serial.println(lastFailSafe ? "Active" : "Inactive");
    }

    if (rcChannels->failsafe == false)
    {
      // Serial.print("RC Channels <A: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[0]));
      // Serial.print(", E: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[1]));
      // Serial.print(", T: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[2]));
      // Serial.print(", R: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[3]));
      // Serial.print(", Aux1: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[4]));
      // Serial.print(", Aux2: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[5]));
      // Serial.print(", Aux3: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[6]));
      // Serial.print(", Aux4: ");
      // Serial.print(crsf->rcToUs(rcChannels->value[7]));
      // Serial.println(">");

      servos_aileron_set_usec(crsf->rcToUs(rcChannels->value[0]));
      servos_elevator_set_usec(crsf->rcToUs(rcChannels->value[1]));

      const uint8_t motor_power = map(crsf->rcToUs(rcChannels->value[0]), 1000, 2000, 0, 100);
      dc_motor_set_power_100(motor_power);
    } else {
      servos_aileron_set_usec(FAILSAFE_AILERON_POSITION);
      servos_elevator_set_usec(FAILSAFE_ELEVATOR_POSITION);
      dc_motor_set_power_100(FAILSAFE_THROTTLE);
    }

    // Et enfin la télémétrie
    write_telemetry();
  }
}