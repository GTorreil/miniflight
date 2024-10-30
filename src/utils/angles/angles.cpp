#include "angles.hpp"

/** Unwrap an angle to be between -PI and PI */
float unwrap_angle(float current_angle, float last_angle)
{
    float delta = current_angle - last_angle;
    if (delta > M_PI)
    {
        current_angle -= 2.0f * M_PI;
    }
    else if (delta < -M_PI)
    {
        current_angle += 2.0f * M_PI;
    }
    return current_angle;
}

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
