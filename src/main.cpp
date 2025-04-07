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
#include "utils/math/math.hpp"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10);
  }
  delay(1500); // Debug -- wait a bit for the serial monitor to attach.

  // bluetooth_setup();
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

#define ROLL_P 0.2f
#define PITCH_P 0.05f
#define YAW_P 0.2f

#define LPF_ALPHA 0.015f

float mix_out_flaperon_0, mix_out_flaperon_1, mix_out_elevator, mix_out_throttle;
// Store previous filtered errors
float previous_filtered_error_roll = 0.0f;
float previous_filtered_error_pitch = 0.0f;
float previous_filtered_error_yaw = 0.0f;
float previous_filtered_flap_base_angle = 0.0f;
float prev_mix_out_flaperon_0, prev_mix_out_flaperon_1, prev_mix_out_elevator;

void mixer()
{
  // Normalize the errors
  const float normalized_error_roll = error_rate_roll / ROLL_RATE_RAD;
  const float normalized_error_pitch = error_rate_pitch / PITCH_RATE_RAD;
  const float normalized_error_yaw = error_rate_yaw / YAW_RATE_RAD;

  // Apply filtering to the errors
  float filtered_error_roll = filter_exponential_moving_average(normalized_error_roll, previous_filtered_error_roll, LPF_ALPHA);
  float filtered_error_pitch = filter_exponential_moving_average(normalized_error_pitch, previous_filtered_error_pitch, LPF_ALPHA);
  float filtered_error_yaw = filter_exponential_moving_average(normalized_error_yaw, previous_filtered_error_yaw, LPF_ALPHA);

  // Update previous filtered errors
  previous_filtered_error_roll = filtered_error_roll;
  previous_filtered_error_pitch = filtered_error_pitch;
  previous_filtered_error_yaw = filtered_error_yaw;

  // If stabilization is disabled, set the errors to zero
  if (rc_aux2 < -0.1f || imu_available() == false)
  {
    filtered_error_pitch = 0.0f;
    filtered_error_roll = 0.0f;
    filtered_error_yaw = 0.0f;
  }

  // Compute the flap base angle
  // const float flap_base_angle = filter_exponential_moving_average(mapFloat(rc_aux3, -1.0f, 1.0f, FLAP_MIN_ANGLE, FLAP_MAX_ANGLE), previous_filtered_flap_base_angle, 0.005f);
  // previous_filtered_flap_base_angle = flap_base_angle;

  // Disable the flaps
  const float flap_base_angle = 0.0f;

  // Compute mixer outputs
  mix_out_flaperon_0 = -1 * (rc_roll + filtered_error_roll * ROLL_P + flap_base_angle);
  mix_out_flaperon_1 = -1 * (rc_roll + filtered_error_roll * ROLL_P - flap_base_angle);
  mix_out_elevator = rc_pitch - filtered_error_pitch * PITCH_P; // Invert pitch to match the flight controller's orientation
  mix_out_throttle = rc_throttle;                               // Pass the throttle as is

  // Constrain values between -1 and 1
  mix_out_flaperon_0 = constrain(mix_out_flaperon_0, -1.0f, 1.0f);
  mix_out_flaperon_1 = constrain(mix_out_flaperon_1, -1.0f, 1.0f);
  mix_out_elevator = constrain(mix_out_elevator, -1.0f, 1.0f);
  mix_out_throttle = constrain(mix_out_throttle, -1.0f, 1.0f);

  // If failsafe, set failsafe values
  if (rc_failsafe)
  {
    mix_out_flaperon_0 = 0.0f;
    mix_out_flaperon_1 = 0.0f;
    mix_out_elevator = 0.0f;
    mix_out_throttle = 0.0f;
  }

  // Filter the outputs to be responsive enough, but not too noisy for the servos
  mix_out_flaperon_0 = filter_exponential_moving_average(mix_out_flaperon_0, prev_mix_out_flaperon_0, 0.05f);
  mix_out_flaperon_1 = filter_exponential_moving_average(mix_out_flaperon_1, prev_mix_out_flaperon_1, 0.05f);
  mix_out_elevator = filter_exponential_moving_average(mix_out_elevator, prev_mix_out_elevator, 0.05f);

  prev_mix_out_flaperon_0 = mix_out_flaperon_0;
  prev_mix_out_flaperon_1 = mix_out_flaperon_1;
  prev_mix_out_elevator = mix_out_elevator;
}

void set_outputs()
{
  servos_flaperon_0_set_angle(norm_to_angle(mix_out_flaperon_0));
  servos_flaperon_1_set_angle(norm_to_angle(mix_out_flaperon_1));

  servos_elevator_set_angle(norm_to_angle(mix_out_elevator));
  servos_esc_set_angle(norm_to_angle(mix_out_throttle));
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
  // bluetooth_loop();
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
