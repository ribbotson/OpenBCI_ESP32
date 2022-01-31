// LIS3DH Test example for OpenBCI
// ESP32 DevKitC on OpenBCI_ESP - Arduino IDE 1.8.13
// Default Freq 100 KHz 

#include <Wire.h>
#include "OBCIESP.h"
#include "LIS3DH.h"

LIS3DH  accel;


void setup()
{
  Serial.begin (115200);  
  accel.begin();

  delay(10000);
   Serial.println("Registers:");

  for(uint8_t lreg = 0; lreg <= 0x30; lreg++){
    Serial.print(lreg, HEX);
    Serial.print(" = ");
    Serial.println(accel.read(lreg), HEX);
  }
  
  accel.init_accel(SCALE_2G);
  
  Serial.print("LIS3DH ID = ");
  Serial.println(accel.get_device_ID(),HEX);
  
  Serial.println("Registers:");

  for(uint8_t lreg = 0; lreg <= 0x30; lreg++){
    Serial.print(lreg, HEX);
    Serial.print(" = ");
    Serial.println(accel.read(lreg), HEX);
  }
  
 accel.enable_accel(RATE_50HZ); 
}


void loop()
{
  accel.updateAxisData();
  Serial.print("X = ");
  Serial.print((float) accel.axisData[0]/16384.0 );
  Serial.print(" Y = ");
  Serial.print((float) accel.axisData[1]/16384.0);
  Serial.print(" Z = ");
  Serial.println((float) accel.axisData[2] /16384.0);
  
  delay (1000);
}
