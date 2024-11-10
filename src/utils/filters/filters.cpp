#include "filters.hpp"

/**
 * Applies an Exponential Moving Average (EMA) to smooth a signal by giving more weight to recent observations.
 *
 * @param new_value The latest input value to be filtered (e.g., a new sensor reading).
 * @param prev_ema The previous EMA value, which acts as the "state" of the filter.
 * @param alpha A smoothing factor between 0.0 and 1.0 that controls the EMA sensitivity:
 *              - Values close to 1.0 make the EMA more responsive to new input values (less smoothing).
 *              - Values close to 0.0 make the EMA less responsive to changes, providing more smoothing.
 *              - Typical values for alpha range from 0.1 (strong smoothing) to 0.5 (mild smoothing).
 *
 * @return The new EMA value, which is updated with the new input based on `alpha`.
 *
 * Example usage:
 * float ema_value = filter_exponential_moving_average(new_sensor_value, previous_ema_value, 0.3f);
 */
float filter_exponential_moving_average(float new_value, float prev_ema, float alpha)
{
    return alpha * new_value + (1.0f - alpha) * prev_ema;
}