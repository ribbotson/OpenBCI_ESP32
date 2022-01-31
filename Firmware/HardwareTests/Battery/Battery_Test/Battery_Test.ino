// OpenBCI Bttery Voltage Test
// ESP32 DevKitC on OpenBCI_ESP - Arduino IDE 1.8.13
// Default Freq 100 KHz 

#include <Wire.h>
#include "OBCIESP.h"

void setup() {
   Serial.begin (115200);  

}

void loop() {
  Serial.print("Battery voltage = ");
  
  float battery_Volts = ((float) analogRead(BATT_PIN) / 4096) * BATT_MAX;
  Serial.print(battery_Volts,2);
  Serial.println(" Volts");
  delay (1000);

}
