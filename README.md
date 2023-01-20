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
![SW-420 vibration-sensor-module pinout](https://github.com/MamaxeFinders/CarWash_IHM/tree/main/Pictures/SW-420-sensor-768x349.jpg)

**2x16 LCD I2C display** module pinout:
![2x16 LCD I2C display pinout](https://github.com/MamaxeFinders/CarWash_IHM/tree/main/Pictures/I2C-LCD-Module-Hardware-Overview2.png)
![2x16 LCD I2C display pinout](https://github.com/MamaxeFinders/CarWash_IHM/tree/main/Pictures/I2C-LCD-Module-Hardware-Overview.png)

**8 channel 5V relay** module pinout:
![8 channel 5V relay pinout](https://github.com/MamaxeFinders/CarWash_IHM/tree/main/Pictures//8-channel%20relay.jpg)

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

![CAISSE module pcb wiring](https://github.com/MamaxeFinders/CarWash_IHM/tree/main/Pictures/caisse.png)
![CAISSE module pcb wiring](https://github.com/MamaxeFinders/CarWash_IHM/tree/main/Pictures/caisse-wiring-pcb.png)

#### INITITALISATION

##### Module SYSTEM:
Turn on module SYSTEM by powering it to 5Vdc.
Use your mobile device in order to connect to the ESP32 WiFi. Select SYSTEM_LAVAGE and connect using the password (123456789 by default).
Wait for a windows to pop up. It will display the available networks found by the module. Click to the router you wish the module needs to connect to internet. Enter the password and click SAVE.
Wait until the page disappear. If you go back to your WiFi settings you should now see an available SSID called SYSTEM_CAISSE which confirm the module is properly set (DO NOT CONNECT TO THIS NEW NETWORK).

##### Module CAISSE:
Once the SYSTEM is connected, you can initiate the module CAISSE.
As there can be multiple CAISSE, use the micro switch in order to address the module from 1 to 7 using the binary switches (we only use p to 4 CAISSEs here). 
Then power it up and wait for the LED to turn on. If the module did not manage to connect to SYSTEM, the LED will blink 3 time before it restarts.
An isolate mode is available if all switches are put to zero. The module CAISSE will start without connecting to the SYSTEM and can be used for test purpose.
Once the module is connected and you can see the message “BONJOUR” on the LCD screen, turn OFF the module SYSTEM and ON again. Wait until the LED linked to the CAISSE id turns ON. This means both modules are ready to communicate as their mac addresses have been saved to both memories.
Perform the same process as 2 for all other module CAISSE.

