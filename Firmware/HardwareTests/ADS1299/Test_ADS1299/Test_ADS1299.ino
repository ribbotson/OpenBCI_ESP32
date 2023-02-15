// ADS1299 Test example for OpenBCI
// ESP32 DevKitC on OpenBCI_ESP - Arduino IDE 1.8.13

#include "ADS1299.h"

ADS1299 input_board;

void setup() {
   Serial.begin (115200);  
   input_board.begin();

   //input_board.RESET(ADS_1);
   
   Serial.print("ADS1299 ID = ");
   Serial.println(input_board.ADS_getDeviceID(ADS_1),HEX);

}

void loop() {
  for(int reg = 0; reg < 0x19; reg++){
    Serial.print("reg = ");
    Serial.print(reg,HEX);
    Serial.print(" value = ");
    Serial.println(input_board.RREG(reg, ADS_1),HEX);
  }

}
