## This repository showcase a CAISSE automation system built with ESP32 modules

### The whole operation is devided into two following module.
  1. **SYSTEM MODULE**
  2. **CAISSE MODULE**

## HARDWARE SETUP

### SYSTEM MODULE

The system module utilizes the following components:
1.  [ESP32 board](https://www.espressif.com/en/products/socs/esp32)
2.  [MAX6675 thermocouple module](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX6675.pdf)
3.  Four 5V LEDs is connected with the ESP32. The number of LEDs may vary depending on the number of CAISSE module.

**ESP32 board** pinout diagram:
![ESP32 pinout](https://github.com/arrafi-musabbir/temporary/blob/main/ESP32-SPI-pinout-for-microSD-card-module.png)

**MAX6675** thermocouple module pinout:
![MAX6675 thermocouple module](https://github.com/arrafi-musabbir/temporary/blob/main/MAX6675-thermocouple-module-pinout-diagram.jpg)

The MAX6675 thermocouple is connecteded to the ESP32 as follorwing:
1.  S0 pin to GPIO27
2.  CS pin to GPIO14
3.  SCK pin to GPIO12

The four LEDs indicating CAISSE module with be connected with #define GPIO 19, 18 ,5, 17 of the ESP32

![MAX6675 thermocoupleto ESP32](https://github.com/arrafi-musabbir/temporary/blob/main/system.png)


### CAISSE MODULE

The caisse module utilizes the following components: 
1.  [ESP32 board](https://www.espressif.com/en/products/socs/esp32)
2.  [SW-420 vibration-sensor-module](https://components101.com/sensors/sw-420-vibration-sensor-module)
3.  [2x16 LCD I2C display](https://components101.com/displays/16x2-lcd-pinout-datasheet)
4.  [8 channel 5V relay module](https://components101.com/switches/5v-eight-channel-relay-module-pinout-features-applications-working-datasheet)
5.  A custom made PCB board

**SW-420** vibration sensor module pinout diagram:
![SW-420 vibration-sensor-module pinout](https://github.com/arrafi-musabbir/temporary/blob/main/SW-420-sensor-768x349.jpg)

**2x16 LCD I2C display** module pinout:
![2x16 LCD I2C display pinout](https://github.com/arrafi-musabbir/temporary/blob/main/I2C-LCD-Module-Hardware-Overview2.png)
![2x16 LCD I2C display pinout](https://github.com/arrafi-musabbir/temporary/blob/main/I2C-LCD-Module-Hardware-Overview.png)

**8 channel 5V relay** module pinout:
![8 channel 5V relay pinout](https://github.com/arrafi-musabbir/temporary/blob/main/8-channel%20relay.jpg)

The SW-420 vibration-sensor-module is connecteded to the ESP32 as follorwing:
1.  D0 pin to GPIO13

The 2x16 LCD I2C display module is connecteded to the ESP32 as follorwing:
1.  SCL pin to GPIO22
2.  SDA pin to GPIO21

The 8 channel 5V relay module is connecteded to the ESP32 as follorwing:
1.  Relay 1 input pin to GPIO19
2.  Relay 2 input pin to GPIO18
3.  Relay 3 input pin to GPIO5
4.  Relay 4 input pin to GPIO17
5.  Relay 5 input pin to GPIO16
6.  Relay 6 input pin to GPIO4
7.  Relay 7 input pin to GPIO0
8.  Relay 8 input pin to GPIO15

#### CAISSE module pcb wiring layout

![CAISSE module pcb wiring](https://github.com/arrafi-musabbir/temporary/blob/main/caisse.png)
![CAISSE module pcb wiring](https://github.com/arrafi-musabbir/temporary/blob/main/caisse-wiring-pcb.png)

