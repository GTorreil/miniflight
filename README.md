# Miniflight

Miniflight is a simple wing flight controller based on ESP32-C3 and a CRSF-compatible radio. It was designed to be small, simple and cheap for a mini-airplane. 

For stabilized flight, an MPU6050 is required. Stabilization features are automatically enabled if the gyro is detected. If there is no gyro, the flight controller still works, but with stabilization features disabled.

Current Features:
- CRSF Demux : unpack RC channels and send them to the servos/ESCs
- Rate stabilization : rate-mode Gyro stabilization
- Failsafe : apply default values to outputs if radio signal is lost

Feature wishlist:
- Stabilized attitude control (fly by wire)
- Telemetry (VBat) (currently not working)

## Hardware
- ESP32-C3 board : https://s.click.aliexpress.com/e/_DBJMH69
- MPU6050 module : https://s.click.aliexpress.com/e/_EuXEx1B
- Voltage divider : TODO

### Pins

MPU6050

| ESP32-C3 | MPU6050 |
|----------|---------|
| 3.3V     | VCC     |
| GND      | GND     |
| GPIO8    | SDA     |
| GPIO9    | SCL     |
| GPIO10   | INT     |

PWM outputs

| ESP32-C3 | Servo    |
|----------|----------|
| GPIO0    | ESC 1    |
| GPIO2    | Elevator |
| GPIO4    | Aileron  |

### Voltage divider

The voltage divider is used to reduce the voltage of the battery to a value that the ESP32-C3 can handle.

## Notes
- ADC : ESP32-C3 has a limited range of ADC values : between 0V and 2.5V with an attenuation of 11dB.
