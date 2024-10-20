#include "crsf.hpp"

CRSFforArduino *crsf = nullptr;

unsigned long lastPacketTime = 0;

rcChannelsNorm_t rcChannels;

/**
 * Get RC channel values, normalized (-1 to 1 float)
 */
rcChannelsNorm_t *crsf_get_rc_channels_norm()
{
    return &rcChannels;
}

/**
 * Normaliser une valeur RC
 */
float rc_to_normalized(uint16_t rc)
{
    // Clamper entre 1000 et 2000
    if (rc > 2000)
    {
        rc = 2000;
    }
    else if (rc < 1000)
    {
        rc = 1000;
    }

    // mettre à l'échelle
    return (rc - 1000) / 1000.0f;
}

void write_telemetry()
{
    crsf->telemetryWriteBattery(42.0f, 0.0f, 0.0f, 0.0f); // Debug -- TODO: get battery voltage
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
        rcChannels.yaw = rc_to_normalized(rcChannelsCrsf->value[0]);
        rcChannels.pitch = rc_to_normalized(rcChannelsCrsf->value[1]);
        rcChannels.roll = rc_to_normalized(rcChannelsCrsf->value[2]);
        rcChannels.throttle = rc_to_normalized(rcChannelsCrsf->value[3]);
        rcChannels.aux1 = rc_to_normalized(rcChannelsCrsf->value[4]);
        rcChannels.aux2 = rc_to_normalized(rcChannelsCrsf->value[5]);
        rcChannels.aux3 = rc_to_normalized(rcChannelsCrsf->value[6]);
        rcChannels.aux4 = rc_to_normalized(rcChannelsCrsf->value[7]);
    }
    else
    {
        // Set the failsafe values
        rcChannels.failsafe = true;
        rcChannels.yaw = rc_to_normalized(FAILSAFE_AILERON_POSITION);
        rcChannels.pitch = rc_to_normalized(FAILSAFE_ELEVATOR_POSITION);
        rcChannels.roll = rc_to_normalized(FAILSAFE_THROTTLE);
        rcChannels.throttle = rc_to_normalized(FAILSAFE_THROTTLE);

        // For the
        rcChannels.aux1 = rc_to_normalized(FAILSAFE_AUX1);
        rcChannels.aux2 = rc_to_normalized(FAILSAFE_AUX2);
        rcChannels.aux3 = rc_to_normalized(FAILSAFE_AUX3);
        rcChannels.aux4 = rc_to_normalized(FAILSAFE_AUX4);
    }

    // Send telemetry
    write_telemetry();
}

void crsf_loop()
{
    crsf->update();
}

void crsf_init()
{
    Serial1.begin(400000, SERIAL_8N1, 20, 21);
    crsf = new CRSFforArduino(&Serial1);

    if (!crsf->begin())
    {
        Serial.println("CRSF for Arduino failed to initialise. Will reboot now.");

        delete crsf;
        crsf = nullptr;

        esp_restart();
    }

    crsf->setRcChannelsCallback(onReceiveRcChannels);
}
