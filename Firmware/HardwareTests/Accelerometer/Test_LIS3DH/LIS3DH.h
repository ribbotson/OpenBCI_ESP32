/*
 LIS3DH.h
 Header File for LIS3DH Accelerometer for OpenBCI_ESP Base board
 License: GNU General Public License V3
       
 (C) Copyright 2022 Richard Ibbotson
 */

#ifndef __LIS3DH_H__
#define __LIS3DH_H__

#include <Arduino.h>

#define READ_REG		0x80
#define READ_MULTI		0x40

#define STATUS_REG_AUX	0x07	// axis over-run and data available flags (see 0x27)
#define OUT_ADC1_L		0x08	//
#define OUT_ADC1_H		0x09	//
#define OUT_ADC2_L		0x0A	//	ADC input values (check DS)
#define OUT_ADC2_H		0x0B	//
#define OUT_ADC3_L		0x0C	//
#define OUT_ADC3_H		0x0D	//
#define INT_COUNTER_REG	0x0E	// ??
#define WHO_AM_I		0x0F	// DEVICE ID 0x33
#define TMP_CFG_REG		0x1F	// ADC enable (0x80) Temperature sensor enable (0x40)
#define CTRL_REG1		0x20	// Data Rate Power Mode X enable Y enable Z enable (on >0x10)
#define CTRL_REG2		0x21	// High Pass Filter Stuph
#define CTRL_REG3		0x22	// INT1 select register
#define CTRL_REG4		0x23	// Block update timing endian G-force resolution self test SPI pins
#define CTRL_REG5		0x24	// reboot FIFO enable latch 4D detection
#define CTRL_REG6		0x25	// ??
#define REFERENCE		0x26	// interrupt reference
#define STATUS_REG2		0x27	// axis overrun and availale flags (see 0x07)
#define OUT_X_L			0x28	//
#define OUT_X_H			0x29	//
#define OUT_Y_L			0x2A	//	tripple axis values (see 0x0A)
#define OUT_Y_H			0x2B	//
#define OUT_Z_L			0x2C	//
#define OUT_Z_H			0x2D	//
#define FIFO_CTRL_REG	0x2E	// FIFO mode trigger output pin select (?)
#define FIFO_SRC_REG	0x2F	// ??
#define INT1_CFG		0x30	// 6 degree control register
#define INT1_SOURCE		0x31	// axis threshold interrupt control
#define INT1_THS		0x32	// INT1 threshold
#define INT1_DURATION	0x33	// INT1 duration
#define CLICK_CFG		0x38	// click on axis
#define CLICK_SRC		0x39	// other click
#define CLICK_THS		0x3A	// more click
#define TIME_LIMIT		0x3B	// click related
#define TIME_LATENCY	0x3C	// and so on
#define TIME_WINDOW		0x3D	// contined click

#define SCALE_2G		0x00  //(b00000000)	// +/- 2G sensitivity
#define SCALE_4G		0x10  //(b00010000)	// +/- 4G sensitivity
#define SCALE_8G		0x20  //(b00100000)	// +/- 8G sensitivity
#define SCALE_16G		0x30  //(b00110000)	// +/- 16G sensitivity
#define RATE_1HZ		0x10  //(b00010000)	// 1Hz sample rate in normal or low-power mode
#define RATE_10HZ		0x20  //(b00100000)	// 10Hz sample rate in normal or low-power mode
#define RATE_25HZ		0x30  //(b00110000)	// 25Hz sample rate in normal or low-power mode
#define RATE_50HZ		0x40  //(b01000000)	// 50Hz sample rate in normal or low-power mode
#define RATE_100HZ		0x50  //(b01010000)	// 100Hz sample rate in normal or low-power mode
#define RATE_200HZ		0x60  //(b01100000)	// 200Hz sample rate in normal or low-power mode
#define RATE_400HZ		0x70  //(b01110000)	// 400Hz sample rate in normal or low-power mode
#define RATE_1600HZ_LP	0x80  //(b10000000)	// 1600Hz sample rate in low-power mode
#define RATE_1250HZ_N	0x90  //(b10010000)	// 1250Hz sample rate in normal mode
#define RATE_5000HZ_LP	0x90  //(b10010000)	// 5000Hz sample rate in low-power mode
#define ACCEL_AXIS_X    0x07 // x axis
#define ACCEL_AXIS_Y    0x08 // y axis
#define ACCEL_AXIS_Z    0x09 // z axis

class LIS3DH {

public:

// Variables

  uint8_t     DRDYpinValue;
  uint8_t     lastDRDYpinValue;
  int16_t     axisData[3];

// Functions
  LIS3DH();
  void    begin(void);
  void    init_accel(uint8_t);
  void    enable_accel(uint8_t);
  void    disable_accel(void);
  uint8_t    get_device_ID(void);
  uint8_t    read(uint8_t);     // read a register on LIS3DH
  int16_t    read16(uint8_t);    // read two uint8_ts, used to get axis data
  void    write(uint8_t,uint8_t);   // write a register on LIS3DH
  boolean DataReady(void); // check LIS3DH_DRDY pin
  boolean DataAvailable(void); // check LIS3DH STATUS_REG2
  void    updateAxisData(void);
  void    zeroAxisData(void);
  int16_t get_X(void);
  int16_t get_Y(void);
  int16_t get_Z(void);
};

#endif // __LIS3DH_H__
