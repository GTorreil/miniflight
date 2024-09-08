

## Infos sur la carte ESP32-C3 utilisée
- Annonce Aliexpress avec des schémas : https://fr.aliexpress.com/item/1005006000170847.html
- Sur l’ESP32-C3, la plage de l’ADC est encore plus limitée que sur l’ESP32 : entre 0V et 2.5V au maximum avec une atténuation de 11dB.

## Diviseur de tension

```
VBat - 10k - 13k - GND
```
 
U(bat) = U(mes) * 1.75

