## Firmware for Weatherstation

Using an ESP8266 for communication over wifi to our backend hosted on digital ocean, log and send weather data to the "cloud".

### Doxygen docs
Running ```doxygen doxygen_config``` suffices.

### Compiling
Make sure to install the following libraries:
- DHT Sensor Library by Adafruit (Version 1.1.1)
- Sparkfun MPL3115A2 by Sparkfun (Version 1.2.0)
- TimerOne by Jesse (Version 1.1.0)

Afterwards, simply compiling suffices.

### Configuration
There are only three defines in ```firmware.ino``` which need to be modified,
- SSID (SSID of the wifi network you want to connect to)
- password (Password of the wifi you want to connect to)
- location (Name of the dataseries you want to send data too)
