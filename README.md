# Simple Wing Flight Controller

This is a simple wing flight controller based on ESP32-C3 and a CRSF radio. It was designed to be small and simple for a mini-airplane. 

For now it only unpacks the RC channels and sends them to the servos, but support for stabilized FBW modes is planned.

## Hardware
- ESP32-C3 board : https://s.click.aliexpress.com/e/_DBJMH69
- Voltage divider : TODO
- Low voltage checks : TODO

## Notes
- ADC : ESP32-C3 has a limited range of ADC values : between 0V and 2.5V with an attenuation of 11dB.
