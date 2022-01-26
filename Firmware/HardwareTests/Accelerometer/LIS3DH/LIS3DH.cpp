/*
 LIS3DH.cpp
 Library File for LIS3DH Accelerometer for OpenBCI_ESP Base board
 License: GNU General Public License V3
       
 (C) Copyright 2022 Richard Ibbotson
 */
#include "OBCIESP.h"
#include "LIS3DH.h"

void LIS3DH::initialize_accel(byte g)
{
  Wire.begin (ACCEL_SDA_PIN, ACCEL_SCL_PIN);   // sda= GPIO_21 /scl= GPIO_22
  
  byte setting = g | 0x08;          // mask the g range for REG4
  pinMode(ACCEL_DRDY_PIN, INPUT);   // setup dataReady interupt from accelerometer
  LIS3DH_write(TMP_CFG_REG, 0x00);  // DISable ADC inputs, enable temperature sensor
  LIS3DH_write(CTRL_REG1, 0x08);    // disable accel, low power mode
  LIS3DH_write(CTRL_REG2, 0x00);    // don't use the high pass filter
  LIS3DH_write(CTRL_REG3, 0x00);    // no interrupts yet
  LIS3DH_write(CTRL_REG4, setting); // set scale to g, high resolution
  LIS3DH_write(CTRL_REG5, 0x00);    // no boot, no fifo
  LIS3DH_write(CTRL_REG6, 0x00);
  LIS3DH_write(REFERENCE, 0x00);
  DRDYpinValue = lastDRDYpinValue = digitalRead(LIS3DH_DRDY); // take a reading to seed these variables
}

void LIS3DH::enable_accel(byte Hz)
{
  for (int i = 0; i < 3; i++)
  {
    axisData[i] = 0; // clear the axisData array so we don't get any stale news
  }
  byte setting = Hz | 0x07;         // mask the desired frequency
  LIS3DH_write(CTRL_REG1, setting); // set freq and enable all axis in normal mode
  LIS3DH_write(CTRL_REG3, 0x10);    // enable DRDY1 on INT1 (tied to PIC pin 0, LIS3DH_DRDY)
}

void LIS3DH::disable_accel()
{
  LIS3DH_write(CTRL_REG1, 0x08); // power down, low power mode
  LIS3DH_write(CTRL_REG3, 0x00); // disable DRDY1 on INT1
}

byte LIS3DH::LIS3DH_getDeviceID()
{
  return LIS3DH_read(WHO_AM_I);
}

boolean LIS3DH::LIS3DH_DataAvailable()
{
  boolean x = false;
  if ((LIS3DH_read(STATUS_REG2) & 0x08) > 0)
    x = true; // read STATUS_REG
  return x;
}

boolean LIS3DH::LIS3DH_DataReady()
{
  boolean r = false;
  DRDYpinValue = digitalRead(LIS3DH_DRDY); // take a look at LIS3DH_DRDY pin
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

void LIS3DH::LIS3DH_writeAxisDataSerial(void)
{
  for (int i = 0; i < 3; i++)
  {
    writeSerial(highByte(axisData[i])); // write 16 bit axis data MSB first
    writeSerial(lowByte(axisData[i]));  // axisData is array of type short (16bit)
  }
}

void LIS3DH::LIS3DH_writeAxisDataForAxisSerial(uint8_t axis)
{
  if (axis > 2)
    axis = 0;
  writeSerial(highByte(axisData[axis])); // write 16 bit axis data MSB first
  writeSerial(lowByte(axisData[axis]));  // axisData is array of type short (16bit)
}

// #ifdef USE_WIFI
void LIS3DH::LIS3DH_writeAxisDataWifi(void)
{
  for (int i = 0; i < 3; i++)
  {
    wifi.storeByteBufTx(highByte(axisData[i])); // write 16 bit axis data MSB first
    wifi.storeByteBufTx(lowByte(axisData[i]));  // axisData is array of type short (16bit)
  }
}
void LIS3DH::LIS3DH_writeAxisDataForAxisWifi(uint8_t axis)
{
  if (axis > 2)
    axis = 0;
  wifi.storeByteBufTx(highByte(axisData[axis])); // write 16 bit axis data MSB first
  wifi.storeByteBufTx(lowByte(axisData[axis]));  // axisData is array of type short (16bit)
}
// #endif

void LIS3DH::LIS3DH_zeroAxisData(void)
{
  for (int i = 0; i < 3; i++)
  {
    axisData[i] = 0;
  }
}

byte LIS3DH::LIS3DH_read(byte reg)
{
  reg |= READ_REG;                  // add the READ_REG bit
  csLow(LIS3DH_SS);                 // take spi
  spi.transfer(reg);                // send reg to read
  byte inByte = spi.transfer(0x00); // retrieve data
  csHigh(LIS3DH_SS);                // release spi
  return inByte;
}

void LIS3DH::LIS3DH_write(byte reg, byte value)
{
  csLow(LIS3DH_SS);    // take spi
  spi.transfer(reg);   // send reg to write
  spi.transfer(value); // write value
  csHigh(LIS3DH_SS);   // release spi
}

int LIS3DH::LIS3DH_read16(byte reg)
{ // use for reading axis data.
  int inData;
  reg |= READ_REG | READ_MULTI;                            // add the READ_REG and READ_MULTI bits
  csLow(LIS3DH_SS);                                        // take spi
  spi.transfer(reg);                                       // send reg to start reading from
  inData = spi.transfer(0x00) | (spi.transfer(0x00) << 8); // get the data and arrange it
  csHigh(LIS3DH_SS);                                       // release spi
  return inData;
}

int LIS3DH::getX()
{
  return LIS3DH_read16(OUT_X_L);
}

int LIS3DH::getY()
{
  return LIS3DH_read16(OUT_Y_L);
}

int LIS3DH::getZ()
{
  return LIS3DH_read16(OUT_Z_L);
}

void LIS3DH::LIS3DH_updateAxisData()
{
  axisData[0] = getX();
  axisData[1] = getY();
  axisData[2] = getZ();
}

void LIS3DH::LIS3DH_readAllRegs()
{

  byte inByte;

  for (int i = STATUS_REG_AUX; i <= WHO_AM_I; i++)
  {
    inByte = LIS3DH_read(i);
    printAll("0x");
    printHex(i);
    printAll(" ");
    printlnHex(inByte);
    delay(20);
  }
  printlnAll();

  for (int i = TMP_CFG_REG; i <= INT1_DURATION; i++)
  {
    inByte = LIS3DH_read(i);
    // printRegisterName(i);
    printAll("0x");
    printHex(i);
    printAll(" ");
    printlnHex(inByte);
    delay(20);
  }
  printlnAll();

  for (int i = CLICK_CFG; i <= TIME_WINDOW; i++)
  {
    inByte = LIS3DH_read(i);
    printAll("0x");
    printHex(i);
    printAll(" ");
    printlnHex(inByte);
    delay(20);
  }
}
