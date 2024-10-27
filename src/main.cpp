/**
 * @file main.cpp
 */

#include "Arduino.h"
#include "CRSFforArduino.hpp"
#include "sensors/imu/imu.hpp"
#include "sensors/battery/battery.hpp"
#include "servos/servos.hpp"
#include "radio/crsf/crsf.hpp"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }
  delay(1500); // Debug -- wait a bit for the serial monitor to attach.

  battery_init();
  servos_init();

  crsf_init();

  imu_init();

  Serial.println("Setup done");
}

#pragma region mixer

typedef struct
{
  float aileron;
  float elevator;
  float throttle;
} mixer_result_t;

mixer_result_t mixer_output;

void mixer()
{
  rcChannelsNorm_t *rcChannels = crsf_get_rc_channels_norm();

  // For now, we don't mix anything and just pass the RC channels directly.
  mixer_output.aileron = rcChannels->roll;
  mixer_output.elevator = rcChannels->pitch;
  mixer_output.throttle = rcChannels->throttle;
}

#pragma endregion

/** Convert a normalized value to an angle (0 to 180 degrees) */
uint8_t norm_to_angle(float norm)
{
  return (uint8_t)(90.0f + norm * 90.0f);
}

void set_outputs()
{
  servos_aileron_set_angle(norm_to_angle(mixer_output.aileron));
  servos_elevator_set_angle(norm_to_angle(mixer_output.elevator));
  servos_esc_set_angle(norm_to_angle(mixer_output.throttle));
}

void loop()
{
  crsf_loop();
  battery_loop();
  imu_loop();
  mixer();
  set_outputs();
}
