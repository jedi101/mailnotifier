#include <Servo.h>
#include <SPI.h>
#include <WiFi101.h>
#include <RH_RF95.h>
#include <Adafruit_SleepyDog.h>
#include "arduino_secrets.h"

// enable debugging mode (uncomment to activate)
//#define DEBUG 1

// Configuring Feather M0 LoRa (RFM95) wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"

#define RF95_FREQ 868.0 // setting frequency of the LoRa module
#define serialDataRate 115200
#define longWait 10000
#define wait 100
#define shortWait 10
#define endlessLoop 1
#define transmitPower 23
#define sslPort 443
#define watchdogTimer 20000

// pin used to control the servo
#define SERVO_PIN 12
 
// Flag's up position, in degrees
#define FLAG_UP 112
 
// Flag's down position, in degrees
#define FLAG_DOWN 18
 
// How long to hold the flag up, in seconds
#define FLAG_DELAY 10

// Creating a servo object to operate the servo with
Servo servo;

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Flash red led when LoRa packet has arrived
#define LED 13 // onboard RED led

int watchdogCounter;

// Please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS; // idlestate of the ATWINC1500 wifi chip
// name address for slack's api server 
// (don't forget to add the slack certificate into the flash of the ATWINC1500 firmware beforehand
// see the documentation how this is done
char server[] = "hooks.slack.com";

String SLACK_API_KEY="/services/TEBT7GSKY/BEFJ15M9Q/DKq9wFoLj9dfOkrDeK9ay2Qp"; // lena-slack-workspace
//String SLACK_API_KEY="/services/T2UBM4X2Q/BE6K532UA/cEymzrXABADK6NJRWo9JlF1t"; // iot-labslack-workspace
//String SLACK_API_KEY="/services/T2UBM4X2Q/BFP0FF11V/3sAzSFZ9GAdgIVXsSbJy7qHg"; // you know

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiSSLClient client; // createing an ssl encrypted wifi client

void setup() {

  // tell the servo class which pin we are using
  servo.attach(SERVO_PIN);

  // write flag to down position
  servo.write(FLAG_DOWN);

  // setting up the LoRa module 
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  #ifdef DEBUG
  Serial.begin(serialDataRate);
  while (!Serial) {
    delay(1);
  }
  #endif
  
  delay(wait);
  watchdogCounter = Watchdog.enable(watchdogTimer);

  #ifdef DEBUG
  Serial.println("Feather LoRa RX Test!");
  #endif

  // resetting LoRa chip for initialisation
  digitalWrite(RFM95_RST, LOW);
  delay(shortWait);
  digitalWrite(RFM95_RST, HIGH);
  delay(shortWait);

  // try to initialise the LoRa module
  while (!rf95.init()) {
    #ifdef DEBUG
    Serial.println("LoRa radio init failed");
    #endif
    
    while (endlessLoop);
  }

  #ifdef DEBUG
  Serial.println("LoRa radio init OK!");
  #endif

  // setting the frequency (use 868.0 MHz for usage in Europe)
  // Defaults after init are 434.0MHz, +13dBm (using PA_BOOST), modulation GFSK_Rb250Fd250
  // Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  if (!rf95.setFrequency(RF95_FREQ)) {
    #ifdef DEBUG
    Serial.println("setFrequency failed");
    #endif
    
    while (endlessLoop);
  }

  #ifdef DEBUG
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);
  #endif
  
  // setting transmitter power (5 to 23 dBm is possible)
  rf95.setTxPower(transmitPower, false);

  //Configure pins for Adafruit ATWINC1500 Featherwing
  WiFi.setPins(8,7,4,2); 

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    #ifdef DEBUG
    Serial.println("WiFi shield not present");
    #endif
    
    // don't continue if shield is not present
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    #ifdef DEBUG
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    #endif
    
    // Connect to WPA/WPA2 network.
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection to establish
    delay(longWait);
    Watchdog.reset();
    #ifdef DEBUG
    Serial.println("Watchdog reset!");
    #endif
  }

  #ifdef DEBUG
  Serial.println("Connected to wifi");
  printWiFiStatus();
  Serial.println("Starting connection to server...");
  #endif
}

void loop() {
  Watchdog.reset();
  #ifdef DEBUG
  Serial.println("Watchdog reset!");
  #endif

  // check if the LoRa module is responding
  if (rf95.available()){
    // configuring receiving message buffer
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      String received_message = (char*)buf;

      if(received_message == "lower_flag"){
        #ifdef DEBUG
        Serial.println("-----------------------------------");
        Serial.println("lowering flag detected!");
        Serial.println("-----------------------------------");
        #endif
        lowerTheFlag();
      }else if(received_message == "mail"){
        #ifdef DEBUG
        Serial.println("-----------------------------------");
        Serial.println("You got new Mail!");
        Serial.println("-----------------------------------");
        #endif
        sendSlackMessage();
        raiseTheFlag();
        digitalWrite(LED, HIGH);
  
        #ifdef DEBUG
        RH_RF95::printBuffer("Received: ", buf, len);
        Serial.print("Got: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
        #endif
        
        // Send a reply that we've got the message
        uint8_t data[] = "Message received!";
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
  
        #ifdef DEBUG
        Serial.println("Sent a reply");
        #endif
        
        digitalWrite(LED, LOW);
        
      }else{
        #ifdef DEBUG
        Serial.println("Received an invalid LoRa message!");
        #endif
      }
      
    }
    else
    {
      #ifdef DEBUG
      Serial.println("Receive failed");
      #endif
    }
  }
  #ifdef DEBUG
  Serial.println(watchdogCounter);
  #endif
}

void raiseTheFlag(){
  #ifdef DEBUG
  Serial.println("Raised the flag!");
  #endif

  servo.write(FLAG_UP);
}

void lowerTheFlag(){
  #ifdef DEBUG
  Serial.println("Lowering the flag!");
  #endif

  servo.write(FLAG_DOWN);
}

void printWiFiStatus() {
  #ifdef DEBUG
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  #endif

  IPAddress ip = WiFi.localIP();
  
  #ifdef DEBUG
  Serial.print("IP Address: ");
  Serial.println(ip);
  #endif

  long rssi = WiFi.RSSI();
  
  #ifdef DEBUG
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  #endif
}

void sendSlackMessage(){

  // define message to send here
  String data = "{\"text\": \"Sie haben Post!\"}";
  String post_url = "POST ";
  post_url += SLACK_API_KEY;
  post_url += " HTTP/1.1";

  // connect to the slack server api
  if (client.connect(server, sslPort)) {
    #ifdef DEBUG
    Serial.println("connected to server");
    #endif
    
    // Make a HTTPs POST request:
    //client.println("POST  /services/TEBT7GSKY/BEFJ15M9Q/DKq9wFoLj9dfOkrDeK9ay2Qp HTTP/1.1");
    client.println(post_url);
    client.println("Host: hooks.slack.com");
    client.println("Content-type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
  }

  if (!client.connected()) {
    #ifdef DEBUG
    Serial.println();
    Serial.println("disconnecting from server.");
    #endif
    
    client.stop();
  }

  // as long as we receive any data back from the server print it
  while (client.available()) {
    char c = client.read();

    #ifdef DEBUG
    Serial.write(c);
    #endif
  }
}
