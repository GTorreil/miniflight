#include "crsf.hpp"

CRSFforArduino *crsf = nullptr;

unsigned long lastPacketTime = 0;

rcChannelsNorm_t rcChannels;

float mapFloat(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

/**
 * Get RC channel values, normalized (-1 to 1 float)
 */
rcChannelsNorm_t *crsf_get_rc_channels_norm()
{
    return &rcChannels;
}

/**
 * Normalize an RC value
 */
float rc_to_normalized(uint16_t rc)
{

    // Scale to normalized (-1 to 1 float)
    float mapped = mapFloat(rc, 150.0f, 1850.0f, -1.0f, 1.0f);

    // Constrain between -1 and 1
    if (mapped > 1.0f)
    {
        mapped = 1.0f;
    }
    else if (mapped < -1.0f)
    {
        mapped = -1.0f;
    }

    return mapped;
}

void telem_write_battery(float voltage)
{
    crsf->telemetryWriteBattery(voltage * 100.0F, 0.0f, 0.0f, 0.0f);
}

void setFailsafeValues()
{
    rcChannels.failsafe = true;
    rcChannels.roll = rc_to_normalized(FAILSAFE_AILERON_POSITION);
    rcChannels.pitch = rc_to_normalized(FAILSAFE_ELEVATOR_POSITION);
    rcChannels.yaw = rc_to_normalized(FAILSAFE_AILERON_POSITION);
    rcChannels.throttle = rc_to_normalized(FAILSAFE_THROTTLE);

    rcChannels.aux1 = rc_to_normalized(FAILSAFE_AUX1);
    rcChannels.aux2 = rc_to_normalized(FAILSAFE_AUX2);
    rcChannels.aux3 = rc_to_normalized(FAILSAFE_AUX3);
    rcChannels.aux4 = rc_to_normalized(FAILSAFE_AUX4);
}

void onReceiveRcChannels(serialReceiverLayer::rcChannels_t *rcChannelsCrsf)
{

    const unsigned long now = millis();
    const unsigned long packetInterval = now - lastPacketTime;
    lastPacketTime = now;

    static bool initialised = false;
    static bool lastFailSafe = false;
    if (rcChannelsCrsf->failsafe != lastFailSafe || !initialised)
    {
        initialised = true;
        lastFailSafe = rcChannelsCrsf->failsafe;
    }

    if (rcChannelsCrsf->failsafe == false)
    {

        rcChannels.failsafe = false;
        rcChannels.roll = rc_to_normalized(rcChannelsCrsf->value[0]);
        rcChannels.pitch = rc_to_normalized(rcChannelsCrsf->value[1]);
        rcChannels.throttle = rc_to_normalized(rcChannelsCrsf->value[2]);
        rcChannels.yaw = rc_to_normalized(rcChannelsCrsf->value[3]);
        rcChannels.aux1 = rc_to_normalized(rcChannelsCrsf->value[4]);
        rcChannels.aux2 = rc_to_normalized(rcChannelsCrsf->value[5]);
        rcChannels.aux3 = rc_to_normalized(rcChannelsCrsf->value[6]);
        rcChannels.aux4 = rc_to_normalized(rcChannelsCrsf->value[7]);

        // Serial.print("Raw pitch: ");
        // Serial.print(rcChannelsCrsf->value[1]);
        // Serial.print(" - Pitch: ");
        // Serial.println(rc_to_normalized(rcChannelsCrsf->value[1]));
    }
    else
    {
        setFailsafeValues();
    }
}

void crsf_loop()
{
    crsf->update();
}

void crsf_init()
{
    // Start with failsafe values
    setFailsafeValues();

    crsf = new CRSFforArduino(&Serial1, 20, 21);

    if (!crsf->begin())
    {
        Serial.println("CRSF for Arduino failed to initialise. Will reboot now.");

        delete crsf;
        crsf = nullptr;

        esp_restart();
    }

    crsf->setRcChannelsCallback(onReceiveRcChannels);
}
