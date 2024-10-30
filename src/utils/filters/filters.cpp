#include "filters.hpp"

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