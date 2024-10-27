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
  crsf_init();
  imu_init();

  servos_init();

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

unsigned long last_loop_time = 0; // last loop time in milliseconds
float loop_delta_s = 0;           // delta time in seconds

float angular_speed_roll = 0;  // degrees per second
float angular_speed_pitch = 0; // degrees per second
float angular_speed_yaw = 0;   // degrees per second

float last_angle_roll = 0;  // degrees
float last_angle_pitch = 0; // degrees
float last_angle_yaw = 0;   // degrees

float target_angular_speed_roll = 0;  // degrees per second
float target_angular_speed_pitch = 0; // degrees per second
float target_angular_speed_yaw = 0;   // degrees per second

float error_angular_speed_roll = 0;  // degrees per second
float error_angular_speed_pitch = 0; // degrees per second
float error_angular_speed_yaw = 0;   // degrees per second

// Define configurable variables for the rate of change of angles
#define ROLL_RATE 180.0f // degrees per second
#define PITCH_RATE 90.0f // degrees per second
#define YAW_RATE 20.0f   // degrees per second

void update_target_angles()
{
  rcChannelsNorm_t *rcChannels = crsf_get_rc_channels_norm();

  // Calculate target angular rates based on RC input
  target_angular_speed_roll = rcChannels->roll * ROLL_RATE;
  target_angular_speed_pitch = rcChannels->pitch * PITCH_RATE;
  target_angular_speed_yaw = rcChannels->yaw * YAW_RATE;

  // Calculate the current angular rates
  float current_angle_roll = 0;
  float current_angle_pitch = 0;
  float current_angle_yaw = 0;
  imu_get_angles(&current_angle_yaw, &current_angle_pitch, &current_angle_roll);
  angular_speed_roll = (current_angle_roll - last_angle_roll) / loop_delta_s;
  angular_speed_pitch = (current_angle_pitch - last_angle_pitch) / loop_delta_s;
  angular_speed_yaw = (current_angle_yaw - last_angle_yaw) / loop_delta_s;

  // Note the current angles as the old ones
  last_angle_roll = current_angle_roll;
  last_angle_pitch = current_angle_pitch;
  last_angle_yaw = current_angle_yaw;

  // Calculate the error
  error_angular_speed_roll = target_angular_speed_roll - angular_speed_roll;
  error_angular_speed_pitch = target_angular_speed_pitch - angular_speed_pitch;
  error_angular_speed_yaw = target_angular_speed_yaw - angular_speed_yaw;
}

#define ROLL_P 0.5f
#define PITCH_P 0.5f
#define YAW_P 0.5f

#define LPF_ALPHA 0.1f

/**
 * Applies a low-pass filter to smooth a signal by blending the new input value with the previous filtered value.
 *
 * @param new_value The latest input value to be filtered (e.g., a new sensor reading).
 * @param prev_value The previous filtered value, which acts as the "state" of the filter.
 * @param alpha A smoothing factor between 0.0 and 1.0 that controls the filter's sensitivity:
 *              - Values close to 1.0 make the filter more responsive to new input values (less smoothing).
 *              - Values close to 0.0 make the filter less responsive to changes, providing more smoothing.
 *              - Typical values for alpha range from 0.1 (strong smoothing) to 0.5 (mild smoothing).
 *
 * @return The new filtered value, which combines the previous state with the new input based on `alpha`.
 *
 * Example usage:
 * float filtered_value = low_pass_filter(new_sensor_value, previous_filtered_value, 0.3f);
 */
float low_pass_filter(float new_value, float prev_value, float alpha)
{
  return alpha * new_value + (1.0f - alpha) * prev_value;
}

void mixer()
{
  rcChannelsNorm_t *rcChannels = crsf_get_rc_channels_norm();

  if (imu_available())
  {

    Serial.println(rcChannels->aux2);

    // If stabilization is disabled, we assign RC channels directly.
    if (rcChannels->aux2 < -0.1f)
    {
      mixer_output.aileron = rcChannels->roll;
      mixer_output.elevator = rcChannels->pitch;
      mixer_output.throttle = rcChannels->throttle;
      return;
    }

    // Normalize the errors so we can add them to the target angles
    const float error_roll = error_angular_speed_roll / ROLL_RATE;
    const float error_pitch = error_angular_speed_pitch / PITCH_RATE;
    const float error_yaw = error_angular_speed_yaw / YAW_RATE;

    // Compute new mixer outputs
    const float mix_out_aileron = rcChannels->roll + error_roll * ROLL_P;
    const float mix_out_elevator = rcChannels->pitch + error_pitch * PITCH_P;
    const float mix_out_throttle = rcChannels->throttle + error_yaw * YAW_P;

    // Blend the new mixer outputs with the old ones
    mixer_output.aileron = low_pass_filter(mix_out_aileron, mixer_output.aileron, LPF_ALPHA);
    mixer_output.elevator = low_pass_filter(mix_out_elevator, mixer_output.elevator, LPF_ALPHA);
    mixer_output.throttle = rcChannels->throttle; // Pass the throttle as is
  }
  else
  {
    // If we don't have an IMU, we can't stabilize the aircraft
    // so we just pass the RC channels directly.

    mixer_output.aileron = rcChannels->roll;
    mixer_output.elevator = rcChannels->pitch;
    mixer_output.throttle = rcChannels->throttle;
    return;
  }
}

#pragma endregion

/** Convert a normalized value to an angle (0 to 180 degrees) */
uint8_t norm_to_angle(float norm)
{
  if (norm > 1.0f)
  {
    norm = 1.0f;
  }
  else if (norm < -1.0f)
  {
    norm = -1.0f;
  }

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
  // Compute the delta time
  const unsigned long now = millis();
  loop_delta_s = (now - last_loop_time) / 1000.0f;
  last_loop_time = now;

  // I/O Loop
  crsf_loop();
  battery_loop();
  imu_loop();

  // Stabilization pipeline

  // Update the target angles
  update_target_angles();

  mixer();
  set_outputs();
}
