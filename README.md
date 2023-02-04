### This repository showcase a CAISSE automation system built with ESP32 modules


 
## The whole operation is devided into the following module.

 * [**HARDWARE SETUP**](#hardware-setup)
    * [**SYSTEM MODULE**](#system-module)
    * [**CAISSE MODULE**](#caisse-module)

 * [**SOFTWARE SETUP**](#software-setup)
    * [**SYSTEM MODULE**](#module-system)
    * [**CAISSE MODULE**](#module-caisse)
    
 * [**INITIALISATION**](#initialisation)
    * [**SYSTEM INITIALISATION**](#system-initialisation)
    * [**CAISSE INITIALISATION**](#caisse-initialisation)
 
# HARDWARE SETUP

## SYSTEM MODULE

The system module utilizes the following components:
1.  [ESP32 board](https://www.espressif.com/en/products/socs/esp32)
2.  [MAX6675 thermocouple module](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX6675.pdf)
3.  Four 5V LEDs is connected with the ESP32. The number of LEDs may vary depending on the number of CAISSE module.

**ESP32 board** pinout diagram:
![ESP32 pinout](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/esp32-devkit-pinout.png)

**MAX6675** thermocouple module pinout:
![MAX6675 thermocouple module](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/MAX6675-thermocouple-module-pinout-diagram.jpg)

The MAX6675 thermocouple is connecteded to the ESP32 as follorwing:
1.  S0 pin to GPIO27
2.  CS pin to GPIO14
3.  SCK pin to GPIO12

The four LEDs indicating CAISSE module with be connected with #define GPIO 19, 18 ,5, 17 of the ESP32

![MAX6675 thermocoupleto ESP32](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/system.png)


## CAISSE MODULE

The caisse module utilizes the following components: 
1.  [ESP32 board](https://www.espressif.com/en/products/socs/esp32)
2.  [SW-420 vibration-sensor-module](https://components101.com/sensors/sw-420-vibration-sensor-module)
3.  [2x16 LCD I2C display](https://components101.com/sites/default/files/components/16x2-LCD-Module.jpg)
4.  [8 channel 5V relay module](https://m.media-amazon.com/images/I/51BrNgOyHdL.jpg)
5.  A custom made PCB board (See GERBERS file)

**SW-420** vibration sensor module pinout diagram:
![SW-420 vibration-sensor-module pinout](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/SW-420-sensor-768x349.jpg)

**2x16 LCD I2C display** module pinout:
![2x16 LCD I2C display pinout](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/I2C-LCD-Module-Hardware-Overview.png?raw=true)

**8 channel 5V relay** module pinout:
![8 channel 5V relay pinout](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/8-channel%20relay.jpg?raw=true)


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

![CAISSE module pcb wiring](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/caisse.png?raw=true)
![CAISSE module pcb schematics](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Pictures/caisse-wiring-pcb.png?raw=true)

# SOFTWARE SETUP

### ARDUINO IDE
Download and install the arduino ide by following this link ![ARDUINO IDE](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-downloading-and-installing)

### ESP32 Add-on
Install ESP32 board add-on in Arduino IDE by following this link ![ESP32 ADD-ON](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)

## Module SYSTEM

### WIFI manager library
The ![*WiFiManager*](https://github.com/tzapu/WiFiManager) library for ESP32 provides an easy way to configure and manage WiFi connections in your projects. It offers features such as:
* Captive portal for setting WiFi credentials
* Ability to store WiFi credentials in Non-Volatile Memory (NVM)
* Option to automatically connect to a previously saved network
* Option to have multiple access points with custom settings
* Support for custom parameters in the configuration portal
To use the library, you can follow ![this tutorial](https://microdigisoft.com/esp32-with-wifimanager-to-manage-ssid-and-password-no-hard-coding/) to setup and use the wifi-manager library. Then, you can initialize the WiFiManager object and use its functions to set up and control the WiFi connection.

### HTTPClient library
Install following this link ![HTTPClient](https://github.com/amcewen/HttpClient)

### Thermocouple library
Install ![MAX6675 library](https://github.com/adafruit/MAX6675-library) from the arduino library manager

### Universal Telegram Bot library
Install telegram bot library in Arduino IDE by following this link ![UniversalTelegramBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)

### Setup wifi as soft access point
Input your: ssid, password, channel, hide_SSID=true/false, max_connection

``` WiFi.softAP("SYSTEM_CAISSE", "123456789", CHANNEL, false, NUMSLAVES); ```

### Setup the google-script-id

``` String GOOGLE_SCRIPT_ID = "input your google-script-id here" ; ```

### Initialize Telegram BOT

``` String BOTtoken = "input your telegram bot-token here"; ```

### Compile and upload the code
Finally you can compile and upload the ![system-module code](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/System_Auto_V3.ino) to your system module

## Module CAISSE

### LiquidCrystal_I2C library
Install ![LiquidCrystal_I2C library](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) from the arduino library manager

### Compile and upload the code
Finally you can compile and upload the ![caisse-module code](https://github.com/MamaxeFinders/CarWash_IHM/blob/main/Caisse_Auto_V3.ino) to your system module


# INITIALISATION

## SYSTEM INITIALISATION

* Turn on module SYSTEM by powering it to 5V DC. 
   * **Warning:** *Providing more than 5V will damage the module*
* Use your mobile device in order to connect to the ESP32 WiFi. Select SYSTEM_LAVAGE and connect using the password (123456789 by default).
* Wait for a windows to pop up. It will display the available networks found by the module. Click to the router you wish the module needs to connect to internet. Enter the password and click SAVE.
* Wait until the page disappear. If you go back to your WiFi settings you should now see an available SSID called SYSTEM_CAISSE which confirm the module is properly set *(DO NOT CONNECT TO THIS NEW NETWORK)*.

## CAISSE INITIALISATION
* Once the SYSTEM is connected, you can initiate the module CAISSE.
* As there can be multiple CAISSE, use the micro switch in order to address the module from 1 to 7 using the binary switches (we only used p to 4 CAISSEs here). 
* Then power it up and wait for the LED to turn on. If the module did not manage to connect to SYSTEM, the LED will blink 3 time before it restarts.
* An isolate mode is available if all switches are put to zero. The module CAISSE will start without connecting to the SYSTEM and can be used for test purpose.
* Once the module is connected and you can see the message *BONJOUR* on the LCD screen, turn OFF the module SYSTEM and ON again. Wait until the LED linked to the CAISSE id turns ON. This means both modules are ready to communicate as their mac addresses have been saved to both memories.
* Perform the same process as 2 for all other module CAISSE.

