# Miniflight

This is a simple wing flight controller based on ESP32-C3 and a CRSF-compatible radio. It was designed to be small, simple and cheap for a mini-airplane. 

For stabilized flight, an MPU6050 is required. Stabilization features are automatically enabled if the gyro is detected. If there is no gyro, the flight controller still works, but with stabilization features disabled.

Current Features:
- CRSF Demux : unpack RC channels and send them to the servos/ESCs
- Rate stabilization : rate-mode Gyro stabilization

Feature wishlist:
- Stabilized attitude control
- Telemetry (VBat) (currently not working)

## Hardware
- ESP32-C3 board : https://s.click.aliexpress.com/e/_DBJMH69
- MPU6050 module : https://s.click.aliexpress.com/e/_EuXEx1B
- Voltage divider : TODO

## Notes
- ADC : ESP32-C3 has a limited range of ADC values : between 0V and 2.5V with an attenuation of 11dB.
