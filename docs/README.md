## Welcome

Mailnotifier, as the name suggests, notifies you when you get mail. It uses LoRa and WiFi to send you a message within Slack as soon as your mail is in your mailbox.

## Used Hardware

Here is a list of the hardware used for this project:
- [2x Adafruit Feather M0 WiFi with uFL - ATSAMD21 + ATWINC1500 with uFL Antenna and loose headers](https://www.adafruit.com/product/3061)
- [2x Adafruit LoRa Radio FeatherWing - RFM95W 900 MHz - RadioFruit](https://www.adafruit.com/product/3231)
- [2x LoRa Antenna with uFL Connector](https://www.adafruit.com/product/1661)
- [WiFi Antenna with uFL Connector](https://www.adafruit.com/product/2308)
- [Load Cell](https://www.amazon.de/dp/B076PZFRDM)
- [SparkFun Load Cell Amplifier - HX711](https://www.sparkfun.com/products/13879)
- [JST PH 2-Pin Cable (Female Connector) (2x if you want to power the receiver with batteries too)](https://www.adafruit.com/product/261)
- [Battery holder for 18650 batteries (same as above)](https://www.mouser.de/ProductDetail/Keystone-Electronics/1043?qs=sGAEpiMZZMvxqoKe%252bDjhru3bNJqZAwi1iQWc%2fA0A0JQ%3d)
- [18650 batteries](https://www.adafruit.com/product/353)

## How To

Before you can use this code, you have to change a few things.

### 1.

First of all, you have to enter your WiFi information in [arduino_secrets.h](/code/mailnotifier_Receiver/arduino_secrets.h). To do this, you need to write your WiFi name (the SSID) within the first quotation marks and your password within the second quotation marks like so...


```
#define SECRET_SSID "YOURSSIDHERE"
#define SECRET_PASS "YOURSECRETPASSWORD"
```
### 2.

Secondly, you need to create a webhook for Slack to send messages when you receive mail. Just follow [this simple tutorial](https://api.slack.com/incoming-webhooks).

```
https://hooks.slack.com/services/T00000000/B00000000/XXXXXXXXXXXXXXXXXXXXXXXX
```

After you got the URL to the Channel you want to get the messages in, you need to find the line within [mailnotifier_Receiver](https://github.com/IoT-Lab-Minden/mailnotifier/blob/master/code/mailnotifier_Receiver/mailnotifier_Receiver.ino) that starts with **_client.println("POST_** and ends with **_HTTP/1.1");_**. Copy everything after the **_.com_** part of your URL and paste it in between this two parts. (As shown below)

```
// Make a HTTPs POST request:
    client.println("POST /services/TEBT7GSKY/BEFJ15M9Q/DKq9wFoLj9dfOkrDeK9ay2Qp HTTP/1.1");
```

### 3.

Lastly, you need to calibrate your load cell (do this after the load cell is placed in the mailbox). To do this, you need to run the calibration sketch. With it you'll find the **_calibration_factor_** and the **_zero_factor_**. Replace the numbers associated with these two factors within [*mailnotifier_Transmitter*](https://github.com/IoT-Lab-Minden/mailnotifier/blob/master/code/mailnotifier_Transmitter/mailnotifier_Transmitter.ino) with the numbers you just found with this sketch.

```
#define calibration_factor -405000.0
#define zero_factor -37744.0
```

## Used libraries

For this project we used the following libraries:
- SPI (Standard library used for communication to RF95 LoRa module via SPI) => [SPI documentation](https://www.arduino.cc/en/Reference/SPI)
- WiFi101 (Library for SSL encrypted communication via the ATWINC1500 wifi chip) =>
[Wifi101 documentation](https://www.arduino.cc/en/Reference/WiFi101)
- RH_RF95 (as part of the radiohead library for communication via LoRa => [github repo](https://github.com/adafruit/RadioHead)
- Adafruit_SleepyDog (Watchdog and Powersaving implementation) => [github repo](https://github.com/adafruit/Adafruit_SleepyDog)
- HX711 (For taking weight measurements) => [github repo](https://github.com/bogde/HX711)

## Diagrams

### System diagram (Mailnotifier)
![System diagram](Diagrams/SystemDiagram.svg "System diagram")

### Activity diagram (Mailnotifier)
![Activity diagram](Diagrams/ActivityDiagram.png "Activity diagram")

### Requirements diagram (Mailnotifier)
```pdf
	Diagrams/RequirementsDiagram.pdf
```
