#include <RadioLib.h>

#define CTF1 14
#define CTF2 11
#define CTF3 10


// SX1262 has the following connections:
// NSS pin:   17
// DIO1 pin:  3
// NRST pin:  8
// BUSY pin:  2
SX1262 radio = new Module(17, 3, 8, 2);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1262 radio = RadioShield.ModuleA;

unsigned long baud = 500000;

//unsigned long baud = 57600;//Baud for Zigbee2MQTT
//unsigned long baud = 115200;//Baud for Zigbee2MQTT

//Pin declaration to enter bootloader mode on CC1352
#define Pin_Reset (1)
#define Pin_Boot (7)

void setup() {
  //Begin Serial ports
  Serial.begin(baud);
  Serial1.begin(baud);

  //Assign the attribute of OUTPUT to our pins
  pinMode(Pin_Reset, OUTPUT);
  pinMode(Pin_Boot, OUTPUT);

  //Enter bootloader mode function
  digitalWrite(Pin_Boot, LOW);
  delay(100);
  digitalWrite(Pin_Reset, LOW);
  delay(100);
  digitalWrite(Pin_Reset, HIGH);
  delay(100);
  digitalWrite(Pin_Boot, HIGH);
  while(!Serial);

  pinMode(CTF1, OUTPUT);
  pinMode(CTF2, OUTPUT);
  pinMode(CTF3, OUTPUT);

  digitalWrite(CTF1,  HIGH);
  digitalWrite(CTF2,  LOW);
  digitalWrite(CTF3,  LOW);
  
  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  //debing(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, int8_t power, uint16_t preambleLength, float tcxoVoltage, bool useRegulatorLDO)
  int state = radio.begin(915.0, 250, 7, 5, 0x34, 20, 10, 0, false);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // some modules have an external RF switch
  // controlled via two pins (RX enable, TX enable)
  // to enable automatic control of the switch,
  // call the following method
  // RX enable:   4
  // TX enable:   5
  
  radio.setRfSwitchPins(16, 15);
  
}

void loop() {
  //SerialPassthrough
  if (Serial.available()) {      // If anything comes in Serial (USB),
    Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial1.read());   // read it and send it out Serial (USB)
  }
}
