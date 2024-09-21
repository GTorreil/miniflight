

## Infos sur la carte ESP32-C3 utilisée
- Annonce Aliexpress avec des schémas : https://fr.aliexpress.com/item/1005006000170847.html
- Sur l’ESP32-C3, la plage de l’ADC est encore plus limitée que sur l’ESP32 : entre 0V et 2.5V au maximum avec une atténuation de 11dB.

## Diviseur de tension

```
VBat - 100k - 100k - GND
```
 
U(bat) = U(mes) * 2

## Notes diverses


Tension minimale de la batterie
- EP1Rx : J'ai fait des essais de tension minimale sur le EP1 Rx : la liaison coupe à 1.8V, et elle est parfaitement stable à 2.25V avec une portée qui semble similaire à celle à 5V. Il n'y a donc aucun risque de couper la liaison même avec une batterie bien déchargée.
- ESP32-C3 : En théorie, le régulateur de tension utilisé par la carte a un dropdout de 100mV ; et l'ESP32-C3 est donné pour une entrée min de 3.0V. On pourrait donc descendre à 3.1V sur la batterie "en théorie". En réalité, on peut peut être descendre plus bas, mais cela n'est pas très sûr. TODO : tester le comportement en basse tension de l'ensemble.

--> tension de batterie déchargée = 3.1V