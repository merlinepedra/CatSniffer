/*
  CatSniffer - Use LoRa for communication with the SX1262 module
  
  Andres Sabas @ Electronic Cats
  Original Creation Date: Jal 23, 2021
  This code is beerware; if you see me (or any other Electronic Cats
  member) at the local, and you've found our code helpful,
  please buy us a round!
  Distributed as-is; no warranty is given.
*/

#define SERIALCOMMAND_HARDWAREONLY
 
#include <SerialCommand.h>
#include <RadioLib.h>

#define CTF1 14
#define CTF2 11
#define CTF3 10

SerialCommand SCmd;

float fwVersion= 0.1;

float frequency = 915;
int spreadFactor = 8;
int bwReference = 7;
int codingRate = 5;
byte syncWord = 0x12;
int preambleLength = 8;
int outputPower = 20;
int channel = 0;
bool rx_status = false;

// SX1262 has the following connections:
// NSS pin:   17
// DIO1 pin:  3
// NRST pin:  8
// BUSY pin:  2
SX1262 radio = new Module(17, 3, 8, 2);

// or using RadioShield
// https://github.com/jgromes/RadioShield
//SX1262 radio = RadioShield.ModuleA;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  Serial.println("Welcome to the LoRa Sniffer CLI " + String(fwVersion,1) + "v\n");
  Serial.println("With this sketch you can scan the LoRa spectrum");
  Serial.println("Changing the Frequency, Spreading Factor, BandWidth or the IQ signals of the radio.");
  Serial.println("Type help to get the available commands.");
  Serial.println("Electronic Cats ® 2020");

  // Setup callbacks for SerialCommand commands 
  SCmd.addCommand("help",help); 
  SCmd.addCommand("set_rx",set_rx);
  SCmd.addCommand("set_tx",set_tx0);
  SCmd.addCommand("set_tx_hex",set_tx1);
  SCmd.addCommand("set_tx_ascii",set_tx2);
  SCmd.addCommand("set_freq",set_freq);
  SCmd.addCommand("set_sf",set_sf);
  SCmd.addCommand("set_bw",set_bw);
  SCmd.addCommand("set_cr",set_cr);
  SCmd.addCommand("set_chann",set_chann);
  SCmd.addCommand("set_sw",set_sw);
  SCmd.addCommand("set_op",set_op);
  SCmd.addCommand("set_pl",set_pl);

  SCmd.addCommand("set_rx",set_rx);

  SCmd.addCommand("get_config",get_config);
  SCmd.addCommand("get_freq",get_freq);
  SCmd.addCommand("get_sf",get_sf);  
  SCmd.addCommand("get_bw",get_bw);
  SCmd.addCommand("get_cr",get_cr);
  SCmd.addCommand("get_op",get_op);
  SCmd.addCommand("get_sw",get_sw);
  SCmd.addCommand("get_pl",get_pl);
  
  SCmd.setDefaultHandler(unrecognized);  // Handler for command that isn't matched  (says "What?") 


  pinMode(CTF1, OUTPUT);
  pinMode(CTF2, OUTPUT);
  pinMode(CTF3, OUTPUT);

  digitalWrite(CTF1,  HIGH);
  digitalWrite(CTF2,  LOW);
  digitalWrite(CTF3,  LOW);

  
  // initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  //debing(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t syncWord, int8_t power, uint16_t preambleLength, float tcxoVoltage, bool useRegulatorLDO)

  int state = radio.begin(915.0, 250, 7, 5, 0x12, 20, 8, 0, false);

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

  rx_status = false;
  
  // some modules have an external RF switch
  // controlled via two pins (RX enable, TX enable)
  // to enable automatic control of the switch,
  // call the following method
  // RX enable:   4
  // TX enable:   5
  
  radio.setRfSwitchPins(16, 15);
  
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void loop() {

  SCmd.readSerial();     // We don't do much, just process serial commands

  // check if the flag is set
  if(receivedFlag && rx_status) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    // you can also read received data as byte array
    /*
      byte byteArr[8];
      int state = radio.readData(byteArr, 8);
    */

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

    } else if (state == ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);

    }

    // put module back to listen mode
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }

}
void help(){
  Serial.println("Fw version: " + String(fwVersion,1)+"v");
  Serial.println("\tConfiguration commands:");
  Serial.println("\tset_rx");
  Serial.println("\tset_tx");
  Serial.println("\tset_tx_hex");
  Serial.println("\tset_tx_ascii");
  Serial.println("\tset_chann");
  Serial.println("\tset_freq");
  Serial.println("\tset_sf");
  Serial.println("\tset_bw");
  Serial.println("\tset_cr");
  Serial.println("\tset_sw");
  Serial.println("\tset_pl");
  Serial.println("\tset_op");

  Serial.println("Monitor commands:");
  Serial.println("\tget_freq");
  Serial.println("\tget_sf");
  Serial.println("\tget_bw");
  Serial.println("\tget_cr");
  Serial.println("\tget_sw");
  Serial.println("\tget_pl");
  Serial.println("\tget_op");
  Serial.println("\tget_config");

  Serial.println("..help");

}

void set_tx0(){
  char *arg;
  byte data[64];
  int i;

  arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
  if(arg != NULL){
    for(i = 0; arg != NULL; i++){  
        if((arg[0] > 47 && arg[0]< 58) && (arg[1] > 47 && arg[1]< 58) && (arg[2] > 47 && arg[2]< 58) && arg[3] == 0){
                
          data[i] = (byte)strtoul(arg, NULL, 10);
          //Serial.println(data[i],BIN);
        }
        else {
          Serial.println("Use a series of xxx values separated by spaces. The value xxx represents a 3-digit number < 255. ");
          return;
        }
        arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
    }
  
    for(int j = 0; j < i; j++){
      Serial.print(data[j]);
      Serial.print(" ");
    }
          
    int state = radio.transmit(data, i);

    if (state == ERR_NONE) {
      // the packet was successfully transmitted
      Serial.println(F("success!"));
  
      // print measured data rate
      Serial.print(F("[SX1262] Datarate:\t"));
      Serial.print(radio.getDataRate());
      Serial.println(F(" bps"));
  
    } else if (state == ERR_PACKET_TOO_LONG) {
      // the supplied packet was longer than 256 bytes
      Serial.println(F("too long!"));
  
    } else if (state == ERR_TX_TIMEOUT) {
      // timeout occured while transmitting packet
      Serial.println(F("timeout!"));
  
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
  
    }

    Serial.println();
    Serial.print(i);
    Serial.println(" byte(s) sent"); 
    
    rx_status = false; 
  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_tx1(){
  char *arg;  
  byte data[64];
  int i;

  arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
  if(arg != NULL){
    for(i = 0; arg != NULL; i++){ 
      
      if((arg[0] > 64 && arg[0]< 71 || arg[0] > 47 && arg[0]< 58) && (arg[1] > 64 && arg[1]< 71 || arg[1] > 47 && arg[1]< 58) && arg[2] == 0){
  
          data[i] = 0;
          data[i] = nibble(*(arg))<<4;
          data[i] = data[i]|nibble(*(arg + 1));   
      }
      else{
        Serial.println("Use a series of yy values separated by spaces. The value yy represents any pair of hexadecimal digits. ");
        return;
      }
      arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
    }
  
    for(int j = 0; j < i; j++){
      Serial.print(data[j], HEX);
      Serial.print(" ");
    }

    int state = radio.transmit(data, i);

    if (state == ERR_NONE) {
      // the packet was successfully transmitted
      Serial.println(F("success!"));
  
      // print measured data rate
      Serial.print(F("[SX1262] Datarate:\t"));
      Serial.print(radio.getDataRate());
      Serial.println(F(" bps"));
  
    } else if (state == ERR_PACKET_TOO_LONG) {
      // the supplied packet was longer than 256 bytes
      Serial.println(F("too long!"));
  
    } else if (state == ERR_TX_TIMEOUT) {
      // timeout occured while transmitting packet
      Serial.println(F("timeout!"));
  
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
  
    }

    
    Serial.println();
    Serial.print(i);
    Serial.println(" byte(s) sent"); 
    
    rx_status = false; 

  } 
  else {
    Serial.println("No argument"); 
  }
}


void set_tx2(){
  char *arg;  
  arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL){
          
     Serial.print(F("[SX1262] Transmitting packet ... "));
  
    // you can transmit C-string or Arduino string up to
    // 256 characters long
    // NOTE: transmit() is a blocking method!
    //       See example SX126x_Transmit_Interrupt for details
    //       on non-blocking transmission method.
    int state = radio.transmit("Hello World!");
    
    // you can also transmit byte array up to 256 bytes long
    /*
      byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
      int state = radio.transmit(byteArr, 8);
    */
  
    if (state == ERR_NONE) {
      // the packet was successfully transmitted
      Serial.println(F(" Success!, ASCII message sent"));
  
      // print measured data rate
      Serial.print(F("[SX1262] Datarate:\t"));
      Serial.print(radio.getDataRate());
      Serial.println(F(" bps"));
  
    } else if (state == ERR_PACKET_TOO_LONG) {
      // the supplied packet was longer than 256 bytes
      Serial.println(F("too long!"));
  
    } else if (state == ERR_TX_TIMEOUT) {
      // timeout occured while transmitting packet
      Serial.println(F("timeout!"));
  
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
  
    }
    
    rx_status = false;
  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_freq(){
  char *arg;  
  arg = SCmd.next();
  frequency = atof(arg);
  if (arg != NULL){
    if(frequency > 902 && frequency < 923){

        if (radio.setFrequency(frequency) == ERR_INVALID_FREQUENCY) {
          Serial.println(F("Selected frequency is invalid for this module!"));
          return;
        }
  
      Serial.println("Frequency set to " + String(frequency) + " MHz");
      rx_status = false;
    }
    else{
      Serial.println("Error setting the frequency");
      Serial.println("Value must be between 902 MHz and 923 MHz");
    }
  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_chann(){
  char *arg;  
  arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
  channel = atoi(arg);
  if (arg != NULL){
    if(channel > -1 && channel < 64){
    long freq = 902300000 + channel*125000;
    frequency = (float)freq/1000000;
    
    //LoRa.setFrequency(freq);
    if (radio.setFrequency(frequency) == ERR_INVALID_FREQUENCY) {
      Serial.println(F("Selected frequency is invalid for this module!"));
      return;
    }    
    
    Serial.println("Uplink channel set to " + String(channel));
    rx_status = false;
    }
    else if(channel > 63 && channel < 72){
    long freq = 903000000 + (channel - 64)*500000;
    frequency = (float)freq/1000000;

    if (radio.setFrequency(frequency) == ERR_INVALID_FREQUENCY) {
      Serial.println(F("Selected frequency is invalid for this module!"));
      return;
    }    
        
    Serial.println("Downlink channel set to " + String(channel));
    rx_status = false;
    }
    else{
      Serial.println("Error setting the channel");
      Serial.println("Value must be between 0 and 63");
    }
  }  
  else {
    Serial.println("No argument"); 
  }
}


void set_sf(){
  char *arg;  
  arg = SCmd.next();  
  if (arg != NULL){
    spreadFactor = atoi(arg);
    if(spreadFactor < 6 || spreadFactor > 12){
      Serial.println("Error setting the Spreading factor");
      Serial.println("Value must be between 6 and 12");
      return;
    }
    else{

        if (radio.setSpreadingFactor(spreadFactor) == ERR_INVALID_SPREADING_FACTOR) {
          Serial.println(F("Selected spreading factor is invalid for this module!"));
          return;
        }
      Serial.println("Spreading factor set to " + String(spreadFactor));
      rx_status = false;
    }

  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_cr(){
  char *arg;  
  arg = SCmd.next();  
  if (arg != NULL){
    codingRate = atoi(arg);
    if(codingRate > 8 || codingRate < 5){
      Serial.println("Error setting the Coding Rate");
      Serial.println("Value must be between 5 and 8");
      return;
    }
    else{

        if (radio.setCodingRate(codingRate) == ERR_INVALID_CODING_RATE) {
          Serial.println(F("Selected coding rate is invalid for this module!"));
          return;
        }
      Serial.println("CodingRate set to 4/" + String(codingRate));
      rx_status = false;
    }

  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_bw(){
  char *arg;  
  arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
  int bwRefResp = bwReference; //save the previous data 
  bwReference = atoi(arg);
  if (arg != NULL){
      switch (bwReference){
        case 0:
          if (radio.setBandwidth(7.8) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }
          rx_status = false;
          Serial.println("Bandwidth set to 7.8 kHz");
          break;
        case 1:
          if (radio.setBandwidth(10.4) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 10.4 kHz");
          break;
        case 2:
          //LoRa.setSignalBandwidth(15.6E3);
          if (radio.setBandwidth(15.6) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 15.6 kHz");
          break;
        case 3:
          if (radio.setBandwidth(20.8) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 20.8 kHz");
          break;
        case 4:
          if (radio.setBandwidth(31.25) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 31.25 kHz");
          break;
        case 5:
          if (radio.setBandwidth(41.7) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 41.7 kHz");
          break;
        case 6:
          if (radio.setBandwidth(62.5) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 62.5 kHz");
          break;
        case 7:
          if (radio.setBandwidth(125) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 125 kHz");
          break;
        case 8:
          if (radio.setBandwidth(250.0) == ERR_INVALID_BANDWIDTH) {
            Serial.println(F("Selected bandwidth is invalid for this module!"));
            return;
          }          
          rx_status = false;
          Serial.println("Bandwidth set to 250 kHz");
          break;

        default:
          Serial.println("Error setting the bandwidth value must be between 0-8");
          bwReference = bwRefResp; //if there's no valid data restore previous value
          break;
      }
  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_op(){
  char *arg;  
  arg = SCmd.next();  
  if (arg != NULL){
    outputPower = atoi(arg);
    if(outputPower > -16 || outputPower < 23){
      Serial.println("Error setting the output power");
      Serial.println("Value must be between -17 and 22 dBm");
      return;
    }
    else{

        if (radio.setOutputPower(outputPower) == ERR_INVALID_OUTPUT_POWER) {
          Serial.println(F("Selected output power is invalid for this module!"));
          return;
        }
      Serial.println("Output power set to " + String(outputPower));
      rx_status = false;
    }

  } 
  else {
    Serial.println("No argument"); 
  }
}

byte nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;  // Not a valid hexadecimal character
}

void set_sw(){
  char *arg;  
  byte data;
  int i;

  arg = SCmd.next();    // Get the next argument from the SerialCommand object buffer
  if(arg != NULL){
      
      if((arg[0] > 64 && arg[0]< 71 || arg[0] > 47 && arg[0]< 58) && (arg[1] > 64 && arg[1]< 71 || arg[1] > 47 && arg[1]< 58) && arg[2] == 0){
  
          data = 0;
          data = nibble(*(arg))<<4;
          data = data|nibble(*(arg + 1));
          if (radio.setSyncWord(data) != ERR_NONE) {
            Serial.println(F("Unable to set sync word!"));
            return;
          }
          Serial.println("Sync word set to 0x" + String(data));
      }
      else{
        Serial.println("Use yy value. The value yy represents any pair of hexadecimal digits. ");
        return;
      }
  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_pl(){
  char *arg;  
  arg = SCmd.next();  
  if (arg != NULL){
    preambleLength = atoi(arg);
    if(preambleLength > -1 || preambleLength < 65536){
      Serial.println("Error setting the preamble length");
      Serial.println("Value must be between 0 and 65535");
      return;
    }
    else{

        if (radio.setPreambleLength(preambleLength) == ERR_INVALID_CODING_RATE) {
          Serial.println(F("Selected preamble length is invalid for this module!"));
          return;
        }

      Serial.println("Preamble length set to " + String(preambleLength));
      rx_status = false;
    }

  } 
  else {
    Serial.println("No argument"); 
  }
}

void set_rx(){
  char *arg;  
  arg = SCmd.next(); 
  if (arg != NULL){
    frequency = atof(arg);
    if(frequency > 902 && frequency < 923){

      if (radio.setFrequency(frequency) == ERR_INVALID_FREQUENCY) {
        Serial.println(F("Selected frequency is invalid for this module!"));
        return;
      }

      Serial.println("LoRa radio receiving at " + String(frequency) + " MHz");
      
      // start listening for LoRa packets
      Serial.print(F("[SX1262] Starting to listen ... "));
      int state = radio.startReceive();
      if (state == ERR_NONE) {
        Serial.println(F("success!"));
      } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
      }
       
      rx_status = true;
    }
    else{
      Serial.println("Error setting the frequency");
      Serial.println("Value must be between 902 MHz and 923 MHz");
    }
  } 
  else {
    Serial.println("LoRa radio receiving at " + String(frequency) + " MHz");
    // start listening for LoRa packets
    Serial.print(F("[SX1262] Starting to listen ... "));
    int state = radio.startReceive();
    if (state == ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
      while (true);
    }
    rx_status = true;
  }
}

/**********Get information**************/

void get_freq(){
  Serial.print("Frequency = ");
  Serial.println(frequency);
}

void get_sf(){
  Serial.print("Spreading factor = ");
  Serial.println(spreadFactor);
}

void get_cr(){
  Serial.print("Coding Rate = ");
  Serial.println(codingRate);
}

void get_sw(){
  Serial.print("Sync Word = 0x");
  Serial.println(syncWord, HEX);
}

void get_pl(){
  Serial.print("Preamble Length = ");
  Serial.println(preambleLength);
}

void get_op(){
  Serial.print("Output Power = ");
  Serial.println(outputPower);
}

void get_bw(){
  Serial.println("Bandwidth = ");
  switch (bwReference){
    case 0:
      Serial.println("7.8 kHz");
      break;
    case 1:
      Serial.println("10.4 kHz");
      break;
    case 2:
      Serial.println("15.6 kHz");
      break;
    case 3:
      Serial.println("20.8 kHz");
      break;
    case 4:
      Serial.println("31.25 kHz");
      break;
    case 5:
      Serial.println("41.7 kHz");
      break;
    case 6:
      Serial.println("62.5 kHz");
      break;
    case 7:
      Serial.println("125 kHz");
      break;
    case 8:
      Serial.println("250 kHz");
      break;
    default:
      Serial.println("Error setting the bandwidth value must be between 0-8");
      break;
  }
}

void get_config(){
  Serial.println("\nRadio configurations: ");
  Serial.println("Frequency = " + String(frequency) + " MHz");
  Serial.print("Bandwidth = ");
  switch (bwReference){
    case 0:
      Serial.println("7.8 kHz");
      break;
    case 1:
      Serial.println("10.4 kHz");
      break;
    case 2:
      Serial.println("15.6 kHz");
      break;
    case 3:
      Serial.println("20.8 kHz");
      break;
    case 4:
      Serial.println("31.25 kHz");
      break;
    case 5:
      Serial.println("41.7 kHz");
      break;
    case 6:
      Serial.println("62.5 kHz");
      break;
    case 7:
      Serial.println("125 kHz");
      break;
    case 8:
      Serial.println("250 kHz");
      break;
  }
  Serial.println("Spreading Factor = " + String(spreadFactor));
  Serial.println("Coding Rate = 4/" + String(codingRate));
  Serial.print("Sync Word = 0x");
  Serial.println(syncWord, HEX);
  Serial.println("Preamble Length = " + String(preambleLength));
  Serial.println("Output Power = " + String(outputPower));  
  Serial.println("Rx active = " + String(rx_status));
}

// This gets set as the default handler, and gets called when no other command matches. 
void unrecognized(const char *command) {
  Serial.println("Command not found, type help to get the valid commands"); 
}
