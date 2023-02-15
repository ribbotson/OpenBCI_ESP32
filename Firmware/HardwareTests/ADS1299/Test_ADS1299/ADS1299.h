/*
 ADS1299.h
 Header File for ADS1299 for OpenBCI_ESP Base board
 License: GNU General Public License V3
       
 (C) Copyright 2022 Richard Ibbotson
 */

#ifndef __ADS1299_H__
#define __ADS1299_H__

#include <Arduino.h>
#include <SPI.h>

//ADS1299 SPI Command Definition uint8_t Assignments
#define _WAKEUP 0x02 // Wake-up from standby mode
#define _STANDBY 0x04 // Enter Standby mode
#define _RESET 0x06 // Reset the device registers to default
#define _START 0x08 // Start and restart (synchronize) conversions
#define _STOP 0x0A // Stop conversion
#define _RDATAC 0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 0x11 // Stop Read Data Continuous mode
#define _RDATA 0x12 // Read data by command supports multiple read back

//ASD1299 Register Addresses
#define ADS_ID  0x3E  // product ID for ADS1299
#define ID_REG  0x00  // this register contains ADS_ID
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 0x04
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define GPIO 0x14
#define MISC1 0x15
#define MISC2 0x16
#define CONFIG4 0x17

//gainCode choices
#define ADS_GAIN01 (0b00000000)  // 0x00
#define ADS_GAIN02 (0b00010000) // 0x10
#define ADS_GAIN04 (0b00100000) // 0x20
#define ADS_GAIN06 (0b00110000) // 0x30
#define ADS_GAIN08 (0b01000000) // 0x40
#define ADS_GAIN12 (0b01010000) // 0x50
#define ADS_GAIN24 (0b01100000) // 0x60

//inputType choices
#define ADSINPUT_NORMAL     (0b00000000)
#define ADSINPUT_SHORTED    (0b00000001)
#define ADSINPUT_BIAS_MEAS  (0b00000010)
#define ADSINPUT_MVDD       (0b00000011)
#define ADSINPUT_TEMP       (0b00000100)
#define ADSINPUT_TESTSIG    (0b00000101)
#define ADSINPUT_BIAS_DRP   (0b00000110)
#define ADSINPUT_BIAL_DRN   (0b00000111)

//test signal choices...ADS1299 datasheet page 41
#define ADSTESTSIG_AMP_1X (0b00000000)
#define ADSTESTSIG_AMP_2X (0b00000100)
#define ADSTESTSIG_PULSE_SLOW (0b00000000)
#define ADSTESTSIG_PULSE_FAST (0b00000001)
#define ADSTESTSIG_DCSIG (0b00000011)
#define ADSTESTSIG_NOCHANGE (0b11111111)

#define ADS1299_CONFIG1_DAISY (0b10110000)
#define ADS1299_CONFIG1_DAISY_NOT (0b10010000)

//Lead-off signal choices
#define LOFF_MAG_6NA        (0b00000000)
#define LOFF_MAG_24NA       (0b00000100)
#define LOFF_MAG_6UA        (0b00001000)
#define LOFF_MAG_24UA       (0b00001100)
#define LOFF_FREQ_DC        (0b00000000)
#define LOFF_FREQ_7p8HZ     (0b00000001)
#define LOFF_FREQ_31p2HZ    (0b00000010)
#define LOFF_FREQ_FS_4      (0b00000011)
#define PCHAN (0)
#define NCHAN (1)
#define OFF (0)
#define ON (1)

#define ADS_1    1          // Slave Select ADS Input board 1
#define ADS_2    2          // Slave Select ADS Input board 2
#define BOTH_ADS 3          // Slave Select Both ADS chips

#define CHANNELS_PER_ADS_SAMPLE 24


class ADS1299 {

public:

// Variables

uint8_t    regData[24];
int16_t    boardStat; // used to hold the status register
int16_t    boardChannelDataInt[CHANNELS_PER_ADS_SAMPLE];    // array used when reading channel data as ints
int16_t    daisyChannelDataInt[CHANNELS_PER_ADS_SAMPLE];    // array used when reading channel data as ints

// Functions
  ADS1299();
  void      begin(void);
  void      hw_reset(void);
  void      RDATA(int16_t);   // read data one-shot
  void      RDATAC(int16_t);  // go into read data continuous mode
  void      RESET(int16_t);   // set all register values to default
  uint8_t   RREG(uint8_t,int16_t);            // read one ADS register
  void      RREGS(uint8_t,uint8_t,int16_t);      // read multiple ADS registers
  void      SDATAC(int16_t);  // get out of read data continuous mode
  void      STANDBY(int16_t); // go into low power mode
  void      START(int16_t);   // start data acquisition
  void      STOP(int16_t);    // stop data acquisition
  void      WAKEUP(int16_t);  // get out of low power mode
  void      WREG(uint8_t,uint8_t,int16_t);       // write one ADS register
  void      WREGS(uint8_t,uint8_t,int16_t);      // write multiple ADS registers
  uint8_t   xfer(uint8_t);        // SPI Transfer function
  void      csLow(int16_t);
  void      csHigh(int16_t);
  void      resetADS(int16_t);     // reset all the ADS1299's settings
  uint8_t   ADS_getDeviceID(int16_t); // get the device ID register from the ADS1299

  
};

#endif // __ADS1299_H__
