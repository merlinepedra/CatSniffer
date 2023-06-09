/*
   RadioLib SX126x Ping-Pong Example

   For default module settings, see the wiki page
   https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx126x---lora-modem

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

#define CTF1 14
#define CTF2 11
#define CTF3 10

// uncomment the following only on one
// of the nodes to initiate the pings
//#define INITIATING_NODE

// SX1262 has the following connections:
// NSS pin:   10
// DIO1 pin:  2
// NRST pin:  3
// BUSY pin:  9
SX1262 radio = new Module(17, 3, 8, 2);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1262 radio = RadioShield.ModuleA;

// save transmission states between loops
int transmissionState = ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

// this function is called when a complete packet
// is transmitted or received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }

  // we sent aor received  packet, set the flag
  operationDone = true;
}

void setup() {
  Serial.begin(9600);
   while(!Serial);

 pinMode(CTF1, OUTPUT);
  pinMode(CTF2, OUTPUT);
  pinMode(CTF3, OUTPUT);

  digitalWrite(CTF1,  HIGH);
  digitalWrite(CTF2,  LOW);
  digitalWrite(CTF3,  LOW);
  
  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin(915.0, 250, 7, 5, 0x34, 20, 10, 0, false);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(setFlag);

  #if defined(INITIATING_NODE)
    // send the first packet on this node
    Serial.print(F("[SX1262] Sending first packet ... "));
    transmissionState = radio.startTransmit("Hello World!");
    transmitFlag = true;
  #else
    // start listening for LoRa packets on this node
    Serial.print(F("[SX1262] Starting to listen ... "));
    state = radio.startReceive();
    if (state == ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
      while (true);
    }
  #endif
}

void loop() {
  // check if the previous operation finished
  if(operationDone) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    operationDone = false;

    if(transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == ERR_NONE) {
        // packet was successfully sent
        Serial.println(F("transmission finished!"));
  
      } else {
        Serial.print(F("failed, code "));
        Serial.println(transmissionState);
  
      }

      // listen for response
      radio.startReceive();
      transmitFlag = false;
      
    } else {
      // the previous operation was reception
      // print data and send another packet
      String str;
      int state = radio.readData(str);

      if (state == ERR_NONE) {
        // packet was successfully received
        Serial.println(F("[SX1262] Received packet!"));
      
        // print data of the packet
        Serial.print(F("[SX1262] Data:\t\t"));
        Serial.println(str);
  
        // print RSSI (Received Signal Strength Indicator)
        Serial.print(F("[SX1262] RSSI:\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));
  
        // print SNR (Signal-to-Noise Ratio)
        Serial.print(F("[SX1262] SNR:\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));
  
      }

      // wait a second before transmitting again
      delay(1000);

      // send another one
      Serial.print(F("[SX1262] Sending another packet ... "));
      transmissionState = radio.startTransmit("Hello World!");
      transmitFlag = true;
    }

    // we're ready to process more packets,
    // enable interrupt service routine
    enableInterrupt = true;
    
  }
}
