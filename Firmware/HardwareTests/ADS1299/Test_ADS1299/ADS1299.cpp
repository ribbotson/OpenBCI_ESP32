/*
 LIS3DH.cpp
 Library File for LIS3DH Accelerometer for OpenBCI_ESP Base board
 License: GNU General Public License V3
       
 (C) Copyright 2022 Richard Ibbotson
 */
#include "OBCIESP.h"
#include "ADS1299.h"
#include <SPI.h>

SPIClass SPI2(HSPI);

ADS1299::ADS1299(void)
{
  
}

void ADS1299::begin()
{
  pinMode( ADS1299_RESET_PIN, OUTPUT);
  digitalWrite( ADS1299_RESET_PIN, HIGH); //set the ADS1299 reset high
  pinMode( ADS1299_SSEL1_PIN, OUTPUT);
  pinMode( ADS1299_SSEL2_PIN, OUTPUT);
  
  csHigh(ADS_1);
  csHigh(ADS_2);
  
  SPI2.begin(ADS1299_SCK_PIN, ADS1299_MISO_PIN, ADS1299_MOSI_PIN, ADS1299_SSEL1_PIN); // sck, miso, mosi, ss (ss can be any GPIO)
 
  // defines SPI comunication type and initializes module
  SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1)); // SPI setup SPI communication
  delay(200); // SPI delay for initialization
  hw_reset();
 
  RESET(ADS_1); // send reset command
  RESET(ADS_2);
  SDATAC(ADS_1);  // stop continuous mode
  SDATAC(ADS_2);
  
}

void ADS1299::hw_reset()  // Hardware Reset"
{
 digitalWrite( ADS1299_RESET_PIN, LOW);
 delay(ADS1299_RESET_PULSE);
 digitalWrite( ADS1299_RESET_PIN , HIGH);
 delay(ADS1299_RESET_DELAY); 
 }


//SPI communication method
byte ADS1299::xfer(byte _data)
{
  byte inByte;
  inByte = SPI2.transfer(_data);
  return inByte;
}

//SPI chip select method
void ADS1299::csLow(int16_t SS)
{ // select an SPI slave to talk to
  switch (SS)
  {
  case ADS_1:
    digitalWrite(ADS1299_SSEL1_PIN, LOW);
    break;
  case ADS_2:
    digitalWrite(ADS1299_SSEL2_PIN, LOW);
    break;
  
  case BOTH_ADS:
    
    digitalWrite(ADS1299_SSEL1_PIN, LOW);
    digitalWrite(ADS1299_SSEL2_PIN, LOW);
    break;
  default:
    break;
  }
}

void ADS1299::csHigh(int16_t SS)
{ // deselect SPI slave
  delayMicroseconds(4);
  switch (SS)
  {
  case ADS_1:
    digitalWrite(ADS1299_SSEL1_PIN, HIGH);
    break;
  case ADS_2:
    digitalWrite(ADS1299_SSEL2_PIN, HIGH);
    break;
  case BOTH_ADS:
    digitalWrite(ADS1299_SSEL1_PIN, HIGH);
    digitalWrite(ADS1299_SSEL2_PIN, HIGH);
    break;
  default:
    break;
  }
}

uint8_t ADS1299::ADS_getDeviceID(int16_t targetSS)
{ // simple hello world com check
  uint8_t data = RREG(ID_REG, targetSS);
  return data;
}

//System Commands
void ADS1299::WAKEUP(int16_t targetSS)
{
  csLow(targetSS);
  xfer(_WAKEUP);
  csHigh(targetSS);
  delayMicroseconds(3); //must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
}

void ADS1299::STANDBY(int16_t targetSS)
{ // only allowed to send WAKEUP after sending STANDBY
  csLow(targetSS);
  xfer(_STANDBY);
  csHigh(targetSS);
}

void ADS1299::RESET(int16_t targetSS)
{ // reset all the registers to default settings
  csLow(targetSS);
  xfer(_RESET);
  delayMicroseconds(12); //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
  csHigh(targetSS);
}

void ADS1299::START(int16_t targetSS)
{ //start data conversion
  csLow(targetSS);
  xfer(_START); // KEEP ON-BOARD AND ON-DAISY IN SYNC
  csHigh(targetSS);
}

void ADS1299::STOP(int16_t targetSS)
{ //stop data conversion
  csLow(targetSS);
  xfer(_STOP); // KEEP ON-BOARD AND ON-DAISY IN SYNC
  csHigh(targetSS);
}

void ADS1299::RDATAC(int16_t targetSS)
{
  csLow(targetSS);
  xfer(_RDATAC); // read data continuous
  csHigh(targetSS);
  delayMicroseconds(3);
}
void ADS1299::SDATAC(int16_t targetSS)
{
  csLow(targetSS);
  xfer(_SDATAC);
  csHigh(targetSS);
  delayMicroseconds(10); //must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
  delay(210);
}

//  THIS NEEDS CLEANING AND UPDATING TO THE NEW FORMAT
void ADS1299::RDATA(int16_t  targetSS)
{                  //  use in Stop Read Continuous mode when DRDY goes low
  uint8_t inByte;     //  to read in one sample of the channels
  csLow(targetSS); //  open SPI
  xfer(_RDATA);    //  send the RDATA command
  for (int i = 0; i < 3; i++)
  { //  read in the status register and new channel data
    inByte = xfer(0xff);
    boardStat = (boardStat << 8) | inByte; //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
  }
  if (targetSS == ADS_1)
  {
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 3; j++)
      { //  read in the new channel data
        inByte = xfer(0x00);
        boardChannelDataInt[i] = (boardChannelDataInt[i] << 8) | inByte;
      }
    }
    for (int i = 0; i < 8; i++)
    {
      if (bitRead(boardChannelDataInt[i], 23) == 1)
      { // convert 3 byte 2's compliment to 4 byte 2's compliment
        boardChannelDataInt[i] |= 0xFF000000;
      }
      else
      {
        boardChannelDataInt[i] &= 0x00FFFFFF;
      }
    }
  }
  else
  {
    for (int i = 0; i < 8; i++)
    {
      for (int j = 0; j < 3; j++)
      { //  read in the new channel data
        inByte = xfer(0x00);
        daisyChannelDataInt[i] = (daisyChannelDataInt[i] << 8) | inByte;
      }
    }
    for (int i = 0; i < 8; i++)
    {
      if (bitRead(daisyChannelDataInt[i], 23) == 1)
      { // convert 3 byte 2's compliment to 4 byte 2's compliment
        daisyChannelDataInt[i] |= 0xFF000000;
      }
      else
      {
        daisyChannelDataInt[i] &= 0x00FFFFFF;
      }
    }
  }
  csHigh(targetSS); //  close SPI
}

uint8_t ADS1299::RREG(uint8_t _address, int16_t targetSS)
{                                 //  reads ONE register at _address
  uint8_t opcode1 = _address | 0x20; //  RREG expects 001rrrrr where rrrrr = _address
  
  
  csLow(targetSS);                //  open SPI

  xfer(opcode1);                  //  opcode1
  xfer(0x00);                     //  opcode2 one byte
  regData[_address] = xfer(0x00); //  update mirror location with returned byte
  csHigh(targetSS);               //  close SPI

  return regData[_address]; // return requested register value
}

// Read more than one register starting at _address
void ADS1299::RREGS(uint8_t _address, uint8_t _numRegistersMinusOne, int16_t targetSS)
{

  uint8_t opcode1 = _address | 0x20; //  RREG expects 001rrrrr where rrrrr = _address
  csLow(targetSS);                //  open SPI
  xfer(opcode1);                  //  opcode1
  xfer(_numRegistersMinusOne);    //  opcode2
  for (int16_t i = 0; i <= _numRegistersMinusOne; i++)
  {
    regData[_address + i] = xfer(0x00); //  add register byte to mirror array
  }
  csHigh(targetSS); //  close SPI
  

}

void ADS1299::WREG(uint8_t _address, uint8_t _value, int16_t target_SS)
{                                 //  Write ONE register at _address
  uint8_t opcode1 = _address + 0x40; //  WREG expects 010rrrrr where rrrrr = _address
  csLow(target_SS);               //  open SPI
  xfer(opcode1);                  //  Send WREG command & address
  xfer(0x00);                     //  Send number of registers to read -1
  xfer(_value);                   //  Write the value to the register
  csHigh(target_SS);              //  close SPI
  regData[_address] = _value;     //  update the mirror array

}

void ADS1299::WREGS(uint8_t _address, uint8_t _numRegistersMinusOne, int16_t targetSS)
{
  uint8_t opcode1 = _address + 0x40; //  WREG expects 010rrrrr where rrrrr = _address
  csLow(targetSS);                //  open SPI
  xfer(opcode1);                  //  Send WREG command & address
  xfer(_numRegistersMinusOne);    //  Send number of registers to read -1
  for (int16_t i = _address; i <= (_address + _numRegistersMinusOne); i++)
  {
    xfer(regData[i]); //  Write to the registers
  }
  csHigh(targetSS);
  
}
