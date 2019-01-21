## Welcome

Mailnotifier, as the name suggests, notifies you when you get mail. It uses encrypted messages via LoRa and WiFi to send you a message within Slack as soon as your mail is in your mailbox. To archive this goal we devided this project into two distinct parts:

#### The Transmitter, consisting of: A Load Cell, a Load Cell Amplifier to get better weight measurements, an Adafruit Feather M0 with attached LoRa Feather wing on top for LoRa communication and an attached LiPo Batterie) and...

#### The Receiver part consisting of: An Adafruit Feather M0 with onboard Wifi connectivity via the ATWINC1500 wich sends the outgoing slackmassages if a transmission from the transmitter that we got new mail via the attached LoRa featherwing has arrived.

### Adafruit Feather M0 Wifi
![feather_m0_wifi](images/Adafruit-Feather-M0-WiFi---.png)

### Adafruit LoRa Featherwing
![wiring_schema2](images/wiring_schema2.png)

### HX711 Load cell amplifier
![HX711_Load_cell_amplifier](images/HX711_Load_cell_amplifier.png)

### Load cell
![LoadCell](images/LoadCell.png)

## Used Hardware

Here is a list of the hardware used for this project:
- [2x Adafruit Feather M0 WiFi with uFL - ATSAMD21 + ATWINC1500 with uFL Antenna and loose headers](https://www.adafruit.com/product/3061)
- [ - RadioFruit](https://www.adafruit.com/product/3231)
- [2x LoRa Antenna with uFL Connector](https://www.adafruit.com/product/1661)
- [WiFi Antenna with uFL Connector](https://www.adafruit.com/product/2308)
- [Load Cell](https://www.amazon.de/dp/B076PZFRDM)
- [SparkFun Load Cell Amplifier - HX711](https://www.sparkfun.com/products/13879)
- [JST PH 2-Pin Cable (Female Connector) (2x if you want to power the receiver with batteries too)](https://www.adafruit.com/product/261)
- [Battery holder for 18650 batteries (same as above)](https://www.mouser.de/ProductDetail/Keystone-Electronics/1043?qs=sGAEpiMZZMvxqoKe%252bDjhru3bNJqZAwi1iQWc%2fA0A0JQ%3d)
- [18650 batteries](https://www.adafruit.com/product/353)

### Why we have choosen Adafruits Feather M0 + ATWINC1500 wifi module as our underlying base:
* Measures 2.1" x 0.9" x 0.3" (53.65mm x 23mm x 8mm) without headers soldered in.
* ATSAMD21G18 @ 48MHz with 3.3V logic/power
256KB FLASH, 32KB SRAM, No EEPROM
* 3.3V regulator (AP2112K-3.3) with 600mA peak current output, WiFi can draw 300mA peak during xmit
* USB native support, comes with USB bootloader and serial port debugging
* 20 GPIO pins (For controling the servo for example)
* Hardware Serial, hardware I2C, hardware SPI support (We use SPI to communicate with the LoRa module)
* 8 x PWM pins
* 10 x analog inputs (We use pin 9 to measure batterie voltage)
* 1 x analog output
* Built in 200mA lipoly charger with charging status indicator LED
* Pin #13 red LED for general purpose blinking (we use it to show incoming LoRa packets)
* Power/enable pin
* 4 mounting holes
* Reset button

### Pinout
![feather_m0_wifi_pinout](images/feather_m0_wifi_pinout_v1.2-1.png)

## How To

Before you can use this code, you have to change a few things.

### The Transmitter wiring schema:
![wiring_schema_transmitter](images/wiring_schema_base.png)

### The Receiver part wiring schema:
![wiring_schema_receiver](images/Receiver_wiring_schema2.png)

### 1. Setup credentials for Wifi
First of all, you have to enter your WiFi information in [arduino_secrets.h](/code/mailnotifier_Receiver/arduino_secrets.h). To do this, you need to write your WiFi name (the SSID) within the first quotation marks and your password within the second quotation marks like so...


```
#define SECRET_SSID "YOURSSIDHERE"
#define SECRET_PASS "YOURSECRETPASSWORD"
```

### 2. Setup notifications
Secondly, you need to create a webhook for Slack to send messages when you receive mail. Just follow [this simple tutorial](https://api.slack.com/incoming-webhooks).

```
https://hooks.slack.com/services/T00000000/B00000000/XXXXXXXXXXXXXXXXXXXXXXXX
```

After you got the URL to the Channel you want to get the messages in, you need to find the line within [mailnotifier_Receiver](https://github.com/IoT-Lab-Minden/mailnotifier/blob/master/code/mailnotifier_Receiver/mailnotifier_Receiver.ino) that starts with **_client.println("POST_** and ends with **_HTTP/1.1");_**. Copy everything after the **_.com_** part of your URL and paste it in between this two parts. (As shown below)

```
// Make a HTTPs POST request:
    client.println("POST /services/TEBT7GSKY/BEFJ15M9Q/DKq9wFoLj9dfOkrDeK9ay2Qp HTTP/1.1");
```

### 3. Calibrate Loadcell
Lastly, you need to calibrate your load cell (do this after the load cell is placed in the mailbox). To do this, you need to run the calibration sketch. With it you'll find the **_calibration_factor_** and the **_zero_factor_**. Replace the numbers associated with these two factors within [*mailnotifier_Transmitter*](https://github.com/IoT-Lab-Minden/mailnotifier/blob/master/code/mailnotifier_Transmitter/mailnotifier_Transmitter.ino) with the numbers you just found with this sketch.

```
#define calibration_factor -405000.0
#define zero_factor -37744.0
```



## Diagrams

### System diagram (Mailnotifier)
![System diagram](Diagrams/system_diagram.svg "System diagram")

### Use Case (Mailnotifier)
![Use Case](Diagrams/UseCase.png "Use Case")

### Activity diagram (Mailnotifier)
![Activity diagram](Diagrams/ActivityDiagram.png "Activity diagram")

### Statechart (Mailnotifier)
![Statechart](Diagrams/StateChart.png "Statechart")

### Requirements diagram (Mailnotifier)
![RequirementsDiagram](Diagrams/RequirementsDiagram.svg "RequirementsDiagram")

## Used libraries

For this project we used the following libraries:
- SPI (Standard library used for communication to RF95 LoRa module via SPI) => [SPI documentation](https://www.arduino.cc/en/Reference/SPI)
- WiFi101 (Library for SSL encrypted communication via the ATWINC1500 wifi chip) =>
[Wifi101 documentation](https://www.arduino.cc/en/Reference/WiFi101)
- RH_RF95 (as part of the radiohead library for communication via LoRa => [github repo](https://github.com/adafruit/RadioHead)
- Adafruit_SleepyDog (Watchdog and Powersaving implementation) => [github repo](https://github.com/adafruit/Adafruit_SleepyDog)
- HX711 (For taking weight measurements) => [github repo](https://github.com/bogde/HX711)

## Datasheets

* [ATSAMD21 Datasheet - Its long, but its a good read](https://cdn-learn.adafruit.com/assets/assets/000/030/130/original/atmel-42181-sam-d21_datasheet.pdf?1453847579)
* [Atmel Software Programming guide for WINC1500 - this is for the underlying ASF codebase that is 'wrapped' in Adafruit_WINC1500 but its still very handy reference](https://cdn-learn.adafruit.com/assets/assets/000/030/129/original/atmel-42418-software-programming-guide-for-atwinc1500-wifi-using-samd21-xplained-pro_userguide.pdf?1453847486)
* [SX127x Datasheet - The RFM9X LoRa radio chip itself](https://cdn-shop.adafruit.com/product-files/3179/sx1276_77_78_79.pdf)
* [RFM9X - The radio module, which contains the SX1272 chipset](https://cdn-learn.adafruit.com/assets/assets/000/031/659/original/RFM95_96_97_98W.pdf?1460518717)
* [EagleCAD PCB Files on GitHub for Adafruit-Feather-M0-WiFi-WINC1500-PCB](https://github.com/adafruit/Adafruit-Feather-M0-WiFi-WINC1500-PCB)
* [Adafruit Feather PCM Soldermask](datasheets/feather_fabprint.png "Feather_fabprint")
* [Adafruit Feather wiring schema](datasheets/feather_schem.png "feather_schem")
* [Fritzing object for Adafruit-Feather-M0-WiFi-WINC1500 inside Fritzing library](https://github.com/adafruit/Fritzing-Library/tree/master/parts)
* [EagleCAD PCB files on GitHub for the LoRa FeatherWing](https://github.com/adafruit/Adafruit-Radio-FeatherWing-PCB)
* [Fritzing object for Adafruit LoRa FeatherWing inside Fritzing library](https://github.com/adafruit/Fritzing-Library)
