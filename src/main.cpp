/**
 * @file main.cpp
 */

#include "Arduino.h"
#include "CRSFforArduino.hpp"
#include "sensors/imu/imu.hpp"
#include "sensors/battery/battery.hpp"
#include "servos/servos.hpp"
#include "radio/crsf/crsf.hpp"
#include "utils/angles/angles.hpp"
#include "utils/filters/filters.hpp"
#include "bluetooth/bluetooth.hpp"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }
  delay(1500); // Debug -- wait a bit for the serial monitor to attach.

  bluetooth_setup();
  battery_init();
  crsf_init();
  imu_init();

  servos_init();

  Serial.println("Setup done");
}

#pragma region mixer

// Angular speeds
float angular_speed_roll = 0;  // radians per second
float angular_speed_pitch = 0; // radians per second
float angular_speed_yaw = 0;   // radians per second

float last_angle_roll = 0;  // radians
float last_angle_pitch = 0; // radians
float last_angle_yaw = 0;   // radians

// Define configurable variables for the rate of change of angles
#define ROLL_RATE_DEG 180.0f // degrees per second
#define PITCH_RATE_DEG 90.0f // degrees per second
#define YAW_RATE_DEG 20.0f   // degrees per second

#define ROLL_RATE_RAD (ROLL_RATE_DEG * M_PI / 180.0f)
#define PITCH_RATE_RAD (PITCH_RATE_DEG * M_PI / 180.0f)
#define YAW_RATE_RAD (YAW_RATE_DEG * M_PI / 180.0f)

#pragma region delta_time

unsigned long last_loop_time = 0; // last loop time in milliseconds
float loop_delta_s = 0;           // delta time in seconds

void update_delta_time()
{
  // Compute the delta time
  const unsigned long now = millis();
  loop_delta_s = (now - last_loop_time) / 1000.0f;
  last_loop_time = now;
}

#pragma endregion

#pragma region radio

float rc_throttle, rc_roll, rc_pitch, rc_yaw, rc_aux1, rc_aux2, rc_aux3, rc_aux4, rc_failsafe;

void radio_loop()
{
  crsf_loop();
  const rcChannelsNorm_t *rcChannels = crsf_get_rc_channels_norm();

  rc_throttle = rcChannels->throttle;
  rc_roll = rcChannels->roll;
  rc_pitch = rcChannels->pitch;
  rc_yaw = rcChannels->yaw;
  rc_aux1 = rcChannels->aux1;
  rc_aux2 = rcChannels->aux2;
  rc_aux3 = rcChannels->aux3;
  rc_aux4 = rcChannels->aux4;
  rc_failsafe = rcChannels->failsafe;
}
#pragma endregion

#pragma region imu

float current_angle_roll,
    current_angle_pitch, current_angle_yaw; // Radians

void read_imu()
{
  // Calculate the current angular rates
  float imu_roll, imu_pitch, imu_yaw; // Radians
  imu_get_attitude(&imu_yaw, &imu_pitch, &imu_roll);

  const float unwrapped_roll = unwrap_angle(imu_roll, current_angle_roll);
  const float unwrapped_pitch = unwrap_angle(imu_pitch, current_angle_pitch);
  const float unwrapped_yaw = unwrap_angle(imu_yaw, current_angle_yaw);

  // Assign the new values to the current angles.
  // Note that the roll and pitch angles are swapped to account for the IMU's orientation.
  current_angle_yaw = unwrapped_yaw;
  current_angle_pitch = unwrapped_roll;
  current_angle_roll = unwrapped_pitch;
}

#pragma endregion

#pragma region rates
float target_angular_speed_roll, target_angular_speed_pitch, target_angular_speed_yaw; // radians per second

void update_rates()
{
  // Calculate target angular rates based on RC input
  target_angular_speed_roll = rc_roll * ROLL_RATE_RAD;
  target_angular_speed_pitch = rc_pitch * PITCH_RATE_RAD;
  target_angular_speed_yaw = rc_yaw * YAW_RATE_RAD;
}
#pragma endregion

#pragma region rates_error

float error_rate_roll, error_rate_pitch, error_rate_yaw; // radians per second

void update_rates_error()
{
  angular_speed_roll = (current_angle_roll - last_angle_roll) / loop_delta_s;
  angular_speed_pitch = (current_angle_pitch - last_angle_pitch) / loop_delta_s;
  angular_speed_yaw = (current_angle_yaw - last_angle_yaw) / loop_delta_s;

  // Serial.printf("roll %f/%f, pitch %f/%f, yaw %f/%f\n", angular_speed_roll, target_angular_speed_roll, angular_speed_pitch, target_angular_speed_pitch, angular_speed_yaw, target_angular_speed_yaw);

  // Note the current angles as the old ones
  last_angle_roll = current_angle_roll;
  last_angle_pitch = current_angle_pitch;
  last_angle_yaw = current_angle_yaw;

  // Calculate the error
  error_rate_roll = target_angular_speed_roll - angular_speed_roll;
  error_rate_pitch = target_angular_speed_pitch - angular_speed_pitch;
  error_rate_yaw = target_angular_speed_yaw - angular_speed_yaw;
}

#pragma endregion

#define ROLL_P 0.55f
#define PITCH_P 0.75f
#define YAW_P 0.5f

#define LPF_ALPHA 0.025f

float mix_out_aileron, mix_out_elevator, mix_out_throttle;

void mixer()
{

  if (imu_available())
  {

    // If stabilization is disabled, we assign RC channels directly.
    if (rc_aux2 < -0.1f)
    {
      mix_out_aileron = rc_roll;
      mix_out_elevator = rc_pitch;
      mix_out_throttle = rc_throttle;
      return;
    }

    // Normalize the errors so we can add them to the target angles
    const float error_roll = error_rate_roll / ROLL_RATE_RAD;
    const float error_pitch = error_rate_pitch / PITCH_RATE_RAD;
    const float error_yaw = error_rate_yaw / YAW_RATE_RAD;

    // Compute new mixer outputs
    const float new_aileron = rc_roll + error_roll * ROLL_P;
    const float new_elevator = rc_pitch + error_pitch * PITCH_P;
    const float new_throttle = rc_throttle + error_yaw * YAW_P;

    // Blend the new mixer outputs with the old ones
    mix_out_aileron = exponential_moving_average(new_aileron, mix_out_aileron, LPF_ALPHA);
    mix_out_elevator = exponential_moving_average(new_elevator, mix_out_elevator, LPF_ALPHA);
    mix_out_throttle = rc_throttle; // Pass the throttle as is
  }
  else
  {
    // If we don't have an IMU, we can't stabilize the aircraft
    // so we just pass the RC channels directly.

    mix_out_aileron = rc_roll;
    mix_out_elevator = rc_pitch;
    mix_out_throttle = rc_throttle;
    return;
  }
}

void set_outputs()
{

  if (rc_failsafe)
  {
    servos_aileron_set_angle(FAILSAFE_AILERON_POSITION);
    servos_elevator_set_angle(FAILSAFE_ELEVATOR_POSITION);
    servos_esc_set_angle(FAILSAFE_THROTTLE);
  }
  else
  {
    servos_aileron_set_angle(norm_to_angle(mix_out_aileron));
    servos_elevator_set_angle(norm_to_angle(mix_out_elevator));
    servos_esc_set_angle(norm_to_angle(mix_out_throttle));
  }
}

void telemetry_loop()
{
  static unsigned long last_telemetry_time = 0;
  if (millis() - last_telemetry_time < TELEMETRY_INTERVAL_MS)
    return;

  const float battery_voltage = battery_read_voltage();
  telem_write_battery(battery_voltage);
  // Serial.println(battery_voltage);
}

void loop()
{
  // I/O Loop
  bluetooth_loop();
  radio_loop();
  telemetry_loop();
  imu_loop();

  if (imu_has_new_data())
  {
    // FC/Stabilization pipeline
    update_delta_time();
    update_rates();
    read_imu();
    update_rates_error();
    update_rates();
  }

  mixer();
  set_outputs();
}
