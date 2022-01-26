// LIS3DH Test example for OpenBCI
// ESP32 DevKitC on OpenBCI_ESP - Arduino IDE 1.8.13
// Default Freq 100 KHz 

#include <Wire.h>
#include "OBCIESP.h"
#include "LIS3DH.h"

void setup()
{
  Serial.begin (115200);  
  Wire.begin (ACCEL_SDA_PIN, ACCEL_SCL_PIN);   // sda= GPIO_21 /scl= GPIO_22
}

void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}

void loop()
{
  Scanner ();
  delay (100);
}
