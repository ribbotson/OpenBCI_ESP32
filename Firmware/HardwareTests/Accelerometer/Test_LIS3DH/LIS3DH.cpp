/*
 LIS3DH.cpp
 Library File for LIS3DH Accelerometer for OpenBCI_ESP Base board
 License: GNU General Public License V3
       
 (C) Copyright 2022 Richard Ibbotson
 */
#include "OBCIESP.h"
#include "LIS3DH.h"
#include <Wire.h>

LIS3DH::LIS3DH(void)
{
  
}

void LIS3DH::begin()
{
  Wire.begin (ACCEL_SDA_PIN, ACCEL_SCL_PIN);   // sda= GPIO_21 /scl= GPIO_22
}

void LIS3DH::init_accel(uint8_t g)
{
  
  
  byte setting = g | 0x08;          // mask the g range for REG4
  pinMode(ACCEL_INT_PIN, INPUT);   // setup dataReady interupt from accelerometer
  write(TMP_CFG_REG, 0x00);  // DISable ADC inputs, enable temperature sensor
  write(CTRL_REG1, 0x08);    // disable accel, low power mode
  write(CTRL_REG2, 0x00);    // don't use the high pass filter
  write(CTRL_REG3, 0x00);    // no interrupts yet
  write(CTRL_REG4, setting); // set scale to g, high resolution
  write(CTRL_REG5, 0x00);    // no boot, no fifo
  write(CTRL_REG6, 0x00);
  write(REFERENCE, 0x00);
  DRDYpinValue = lastDRDYpinValue = digitalRead(ACCEL_INT_PIN); // take a reading to seed these variables
}

void LIS3DH::enable_accel(uint8_t Hz)
{
  for (int i = 0; i < 3; i++)
  {
    axisData[i] = 0; // clear the axisData array so we don't get any stale news
  }
  uint8_t setting = Hz | 0x07;         // mask the desired frequency
  write(CTRL_REG1, setting); // set freq and enable all axis in normal mode
  write(CTRL_REG3, 0x10);    // enable DRDY1 on INT1 (tied to PIC pin 0, LIS3DH_DRDY)
}

void LIS3DH::disable_accel()
{
  write(CTRL_REG1, 0x08); // power down, low power mode
  write(CTRL_REG3, 0x00); // disable DRDY1 on INT1
}

uint8_t LIS3DH::get_device_ID()
{
  return read(WHO_AM_I);
}

boolean LIS3DH::DataAvailable()
{
  boolean x = false;
  if ((read(STATUS_REG2) & 0x08) > 0)
    x = true; // read STATUS_REG
  return x;
}

boolean LIS3DH::DataReady()
{
  boolean r = false;
  DRDYpinValue = digitalRead(ACCEL_INT_PIN); // take a look at LIS3DH_DRDY pin
  if (DRDYpinValue != lastDRDYpinValue)
  { // if the value has changed since last looking
    if (DRDYpinValue == HIGH)
    {           // see if this is the rising edge
      r = true; // if so, there is fresh data!
    }
    lastDRDYpinValue = DRDYpinValue; // keep track of the changing pin
  }
  return r;
}




void LIS3DH::zeroAxisData(void)
{
  for (int i = 0; i < 3; i++)
  {
    axisData[i] = 0;
  }
}

uint8_t LIS3DH::read(uint8_t reg)
{
  uint8_t inByte;
  Wire.beginTransmission(ACCEL_I2C_ADDRESS);
  Wire.write(reg |= 0x80);
  Wire.endTransmission();
  Wire.requestFrom(ACCEL_I2C_ADDRESS,1);
  inByte = Wire.read();
  
  return inByte;
}

void LIS3DH::write(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(ACCEL_I2C_ADDRESS);
  Wire.write(reg |= 0x80);
  Wire.write(value);
  Wire.endTransmission();
}

int16_t LIS3DH::read16(uint8_t reg)
{ // use for reading axis data.
  int inData;
  Wire.beginTransmission(ACCEL_I2C_ADDRESS);
  Wire.write(reg |= 0x80); //auto increment
  Wire.endTransmission();
  Wire.requestFrom(ACCEL_I2C_ADDRESS,2);
  inData = Wire.read();
  inData |= Wire.read() << 8;
  return inData;
}

int16_t LIS3DH::get_X()
{
  return read16(OUT_X_L);
}

int16_t LIS3DH::get_Y()
{
  return read16(OUT_Y_L);
}

int16_t LIS3DH::get_Z()
{
  return read16(OUT_Z_L);
}

void LIS3DH::updateAxisData()
{
  axisData[0] = get_X();
  axisData[1] = get_Y();
  axisData[2] = get_Z();
}
