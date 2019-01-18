#include <SPI.h>
#include <RH_RF95.h>
#include "HX711.h"

// enable Debugging (uncomment to activate)
#define DEBUG 1

//Feather m0 w/wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"

#define RF95_FREQ 868.0
#define calibration_factor -405000.0
#define zero_factor -37744.0
#define DOUT  15
#define CLK  14
#define serialDataRate 115200
#define second 1000
#define wait 100
#define shortWait 10
#define endlessLoop 1
#define scaleThreshold 0.15
#define transmitPower 23
#define radiopacketLength 20

typedef enum state{
    NO_WEIGHT, WEIGHT
}state;

boolean deactivateSender = true;
boolean hadWeight;
float messuredWeight = 0.0;

HX711 scale(DOUT, CLK);

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  #ifdef DEBUG
  Serial.begin(serialDataRate);
  #endif
  
  delay(shortWait);

  scale.set_scale(calibration_factor);

  digitalWrite(RFM95_RST, LOW);
  delay(shortWait);
  digitalWrite(RFM95_RST, HIGH);
  delay(shortWait);

  while (!rf95.init()) {
    #ifdef DEBUG
    Serial.println("LoRa radio init failed");
    #endif
    
    while (endlessLoop);
  }

  #ifdef DEBUG
  Serial.println("LoRa radio init OK!");
  #endif

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
  
  rf95.setTxPower(transmitPower, false);
}

void loop()
{
  delay(second);
  checkWeight(weigh());
}

state weigh(){
  messuredWeight = scale.get_units();
  
  #ifdef DEBUG
  Serial.print("Messured Weight: ");
  Serial.print(messuredWeight);
  Serial.println(" kg");
  #endif
  
  if (messuredWeight > scaleThreshold){
    return WEIGHT;
  } else {
    return NO_WEIGHT;
  }
}

void checkWeight(state state){

  switch(state) {
    case NO_WEIGHT:
      hadWeight = false;
      deactivateSender = true;
      break;
    case WEIGHT:
      deactivateSender = false;
      
      if (!hadWeight){
        while (!deactivateSender){
          sendLoraMessage();
          
          if(rf95.waitAvailableTimeout(second)){
            uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
            uint8_t len = sizeof(buf);
              
              if (rf95.recv(buf, &len)){
                #ifdef DEBUG
                Serial.print("Got reply: ");
                Serial.println((char*)buf);
                Serial.print("RSSI: ");
                Serial.println(rf95.lastRssi(), DEC);
                #endif   
                
                deactivateSender = true; 
              } else {
                #ifdef DEBUG
                Serial.println("Receive failed");
                #endif
              }
          } else {
            #ifdef DEBUG
            Serial.println("No reply!");
            #endif
          }
        }
      }
      hadWeight = true;
      break;
  }
}

void sendLoraMessage(){
  
  char radiopacket[radiopacketLength] = "Sie haben Post!"; // could also transmit messured weight here
  radiopacket[radiopacketLength-1] = 0;
  
  #ifdef DEBUG
  Serial.print("Sending: ");
  Serial.println(radiopacket);
  #endif
  
  delay(shortWait);
  rf95.send((uint8_t *)radiopacket, radiopacketLength);

  #ifdef DEBUG
  Serial.println("Waiting for packet to complete...");
  #endif
  
  delay(shortWait);
  rf95.waitPacketSent();
}
