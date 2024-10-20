#ifndef CRSF_HPP
#define CRSF_HPP

#include "Arduino.h"
#include "configuration.h"
#include <CRSFforArduino.hpp>


typedef struct
{
    bool failsafe;
    float yaw;
    float pitch;
    float roll;
    float throttle;
    float aux1;
    float aux2;
    float aux3;
    float aux4;
} rcChannelsNorm_t;

void crsf_init();
void crsf_loop();
rcChannelsNorm_t *crsf_get_rc_channels_norm();

#endif
