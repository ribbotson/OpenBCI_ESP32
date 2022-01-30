/*
 OBCIESP_HW.h
 Header File with hardware definitions for OpenBCI_ESP Base board
 License: GNU General Public License V3
       
 (C) Copyright 2022 Richard Ibbotson
 */

#ifndef __OBCIESP_HW_H__
#define __OBCIESP_HW_H__

#define BOARD               DEVKIT_C

#define SERIAL_0_PORT       BTSERIAL // The primary serial Port will use the Bluetooth serial

#define SERIAL_1_PORT       ESP_SERIAL_0  // The serial debug and test port will be over the USB port
#define SERIAL_TX_PIN       3
#define SERIAL_RX_PIN       1

#define ACCEL_PORT          I2C
#define ACCEL_SDA_PIN       21
#define ACCEL_SCL_PIN       22
#define ACCEL_DRDY_PIN      27
#define ACCEL_I2C_ADDRESS   0x30


#define SDCARD_PORT         VSPI
#define SDCARD_MOSI_PIN     23
#define SDCARD_MISO_PIN     19
#define SDCARD_SCK_PIN      18
#define SDCARD_SSEL_PIN     5
#define SDCARD_CD_PIN       32

#define ADS1299_PORT        HSPI
#define ADS1299_MOSI_PIN    13
#define ADS1299_MISO_PIN    12
#define ADS1299_SCK_PIN     14
#define ADS1299_SSEL1_PIN   15
#define ADS1299_SSEL2_PIN   16
#define ADS1299_RESET_PIN   17

#define BATT_PORT       ADC1_CH0  // Ooops forgot this!
#define BATT_PIN        36    // Not connected

#endif // __OBCIESP_HW_H__

