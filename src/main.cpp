/**
 * @file main.cpp
 */

#include "Arduino.h"
#include "CRSFforArduino.hpp"
#include "battery/battery.hpp"
#include "servos/servos.hpp"

#ifdef USE_DC_MOTOR
#include "dc_motor/dc_motor.hpp"
#endif

CRSFforArduino *crsf = nullptr;

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels);

/** Définit les valeurs failsafe aux différentes sorties */
void set_failsafe_values()
{
  servos_aileron_set_usec(FAILSAFE_AILERON_POSITION);
  servos_elevator_set_usec(FAILSAFE_ELEVATOR_POSITION);
  servos_esc_set_usec(FAILSAFE_THROTTLE);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }

  // Debug
  delay(1500);
  // Serial pour le debug
  Serial.println("Starting setup");

  // Serial 1 pour CrossFire
  Serial1.begin(400000, SERIAL_8N1, 20, 21);

  crsf = new CRSFforArduino(&Serial1);

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

  // Écrire les valeurs failsafe aux différentes sorties
  set_failsafe_values();

  Serial.println("Setup done");
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

unsigned long lastPacketTime = 0;

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannels)
{

  const unsigned long now = millis();
  const unsigned long packetInterval = now - lastPacketTime;
  lastPacketTime = now;

  // Serial.print("i=");
  // Serial.println(packetInterval);

  // Serial.print("Rc Channels <A: ");
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
    servos_esc_set_usec(crsf->rcToUs(rcChannels->value[2]));

    // const uint8_t motor_power = map(crsf->rcToUs(rcChannels->value[2]), 990, 2010, 0, 100);
    // dc_motor_set_power_100(motor_power);
    // Serial.println(motor_power);
  }
  else
  {
    set_failsafe_values();
  }

  // Et enfin la télémétrie
  write_telemetry();
}