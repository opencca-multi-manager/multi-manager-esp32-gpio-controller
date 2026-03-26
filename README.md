## ESP32 GPIO Controller

Firmware to drive and read individual GPIO pins of an ESP32 via serial. Used by [multi-manager-rktool](https://github.com/opencca-multi-manager/multi-manager-rktool)
to control the boards' maskrom buttons.

### Setup
- Dependency: [platformio](https://platformio.org/)
- Connect board to USB
- Flash board: `pio run -t upload`
- Open serial monitor: `pio device monitor`


### Interface

```
SET <pin> [HIGH | LOW] // sets pin mode to output and drives pin to requested state
GET <pin>              // reads from pin, if previously unset sets mode to input
```

Valid Pins:
```
[2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36, 39]
```
