# Miniflight

This is a simple wing flight controller based on ESP32-C3 and a CRSF-compatible radio. It was designed to be small, simple and cheap for a mini-airplane. 

For stabilized flight, an MPU6050 is required. Stabilization features are automatically enabled if the gyro is detected. If there is no gyro, the flight controller still works, but with stabilization features disabled.

Current Features:
- CSRF Demux : unpack RC channels and send them to the servos
- Rate stabilization : Gyro stabilization

Wishlist features:
- Stabilized attitude control
- Telemetry (VBat)

## Hardware
- ESP32-C3 board : https://s.click.aliexpress.com/e/_DBJMH69
- Voltage divider : TODO

## Notes
- ADC : ESP32-C3 has a limited range of ADC values : between 0V and 2.5V with an attenuation of 11dB.
