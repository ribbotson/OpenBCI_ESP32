/**
 * ADS129X.cpp
 *
 * Arduino library for the TI ADS129X series of analog-front-ends for
 * biopotential measurements (EMG/EKG/EEG).
 *
 * This library offers two modes of operation: polling and interrupt.
 * Polling mode should only be used in situations where multiple devices
 * share the SPI bus. Interrupt mode is much faster (8kSPS on a Teensy 3.1),
 * but starts receiving immediately when DRDY goes high.
 * The API is the same for both modes. To activate polling mode add
 *     #define ADS129X_POLLING
 * as first line to your sketch.
 *
 * Based on code by Conor Russomanno (https://github.com/conorrussomanno/ADS1299)
 * Modified by Ferdinand Keil
 */



#include "ADS129X.h"
#include "OBCIESP.h"
#include <SPI.h>

SPIClass SPI2(HSPI);

#ifndef ADS129X_POLLING

int32_t ADS129X_data1[9];
int32_t ADS129X_data2[9];
bool ADS129X_newData1;
bool ADS129X_newData2;
void ADS129X_dataReadyISR1();
void ADS129X_dataReadyISR2();
#endif



/**
 * Initializes ADS129x library.
 
 */
ADS129X::ADS129X(void)
{

}

void ADS129X::begin()
{

    pinMode( ADS129X_RESET_PIN, OUTPUT);
    digitalWrite( ADS129X_RESET_PIN, HIGH); //set the ADS1299 reset high
    pinMode( ADS129X_SSEL1_PIN, OUTPUT);
    pinMode( ADS129X_SSEL2_PIN, OUTPUT);
    pinMode( ADS129X_DRDY1_PIN, INPUT_PULLUP);
    pinMode( ADS129X_DRDY2_PIN, INPUT_PULLUP);
  
    csHigh(ADS_1);
    csHigh(ADS_2);
  
    SPI2.begin(ADS129X_SCK_PIN, ADS129X_MISO_PIN, ADS129X_MOSI_PIN, ADS129X_SSEL1_PIN); // sck, miso, mosi, ss (ss can be any GPIO)
 
    // defines SPI comunication type and initializes module
    SPI2.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE1)); // SPI setup SPI communication clock 16MHz
    delay(200); // SPI delay for initialization
    hw_reset();
 
    RESET(ADS_1); // send reset command
    RESET(ADS_2);
    SDATAC(ADS_1);  // stop continuous mode
    SDATAC(ADS_2);
     

    ADS129X_newData1 = false;
    ADS129X_newData2 = false;

}

void ADS129X::hw_reset()  // Hardware Reset"
{
  digitalWrite( ADS129X_RESET_PIN, LOW);
  delay(ADS129X_RESET_PULSE);
  digitalWrite( ADS129X_RESET_PIN , HIGH);
  delay(ADS129X_RESET_DELAY); 
}


//SPI chip select method
void ADS129X::csLow(uint8_t ads)
{ // select an SPI slave to talk to
  switch (ads)
  {
  case ADS_1:
    digitalWrite(ADS129X_SSEL1_PIN, LOW);
    break;
  case ADS_2:
    digitalWrite(ADS129X_SSEL2_PIN, LOW);
    break;
  
  case BOTH_ADS:
    
    digitalWrite(ADS129X_SSEL1_PIN, LOW);
    digitalWrite(ADS129X_SSEL2_PIN, LOW);
    break;
  default:
    break;
  }

}

void ADS129X::csHigh(uint8_t ads)
{ // deselect SPI slave
  delayMicroseconds(4);
  switch (ads)
  {
  case ADS_1:
    digitalWrite(ADS129X_SSEL1_PIN, HIGH);
    break;
  case ADS_2:
    digitalWrite(ADS129X_SSEL2_PIN, HIGH);
    break;
  case BOTH_ADS:
    digitalWrite(ADS129X_SSEL1_PIN, HIGH);
    digitalWrite(ADS129X_SSEL2_PIN, HIGH);
    break;
  default:
    break;
  }
}

//System Commands

/**
 * Exit Standby Mode.
 */
void ADS129X::WAKEUP(uint8_t ads) {
    SPI2.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE1)); // SPI setup SPI communication clock 16MHz
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_WAKEUP);
    delayMicroseconds(2);
    csHigh(ads);
    delayMicroseconds(2);  //must way at least 4 tCLK cycles before sending another command (Datasheet, pg. 38)
    SPI2.endTransaction();
}

/**
 * Enter Standby Mode.
 */
void ADS129X::STANDBY(uint8_t ads) {
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_STANDBY);
    delayMicroseconds(2);
    csHigh(ads);
}

/**
 * Reset Registers to Default Values.
 */
void ADS129X::RESET(uint8_t ads) {
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_RESET);
    delayMicroseconds(12);
    csHigh(ads);
}

/**
 * Start/restart (synchronize) conversions.
 */
void ADS129X::START(uint8_t ads) {
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_START);
    delayMicroseconds(2);
    csHigh(ads);   
#ifndef ADS129X_POLLING
    switch (ads)
    {
      case ADS_1:
        attachInterrupt(ADS129X_DRDY1_PIN, ADS129X_dataReadyISR1, FALLING);
        break;
      case ADS_2:
        attachInterrupt(ADS129X_DRDY2_PIN, ADS129X_dataReadyISR2, FALLING);
        break;
      case BOTH_ADS:
        attachInterrupt(ADS129X_DRDY1_PIN, ADS129X_dataReadyISR1, FALLING);
        attachInterrupt(ADS129X_DRDY2_PIN, ADS129X_dataReadyISR2, FALLING);
        break;
    }
#endif
}

/**
 * Stop conversion.
 */
void ADS129X::STOP(uint8_t ads) {
#ifndef ADS129X_POLLING
    switch (ads)
    {
      case ADS_1:
        detachInterrupt(ADS129X_DRDY1_PIN);
        break;
      case ADS_2:
        detachInterrupt(ADS129X_DRDY2_PIN);
        break;
      case BOTH_ADS:
        detachInterrupt(ADS129X_DRDY1_PIN);
        detachInterrupt(ADS129X_DRDY2_PIN);
        break;
    }
#endif
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_STOP);
    delayMicroseconds(2);
    csHigh(ads);
}

/**
 * Enable Read Data Continuous mode (default).
 */
void ADS129X::RDATAC(uint8_t ads) {
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_RDATAC);
    delayMicroseconds(2);
    csHigh(ads);
    delayMicroseconds(2); //must way at least 4 tCLK cycles before sending another command (Datasheet, pg. 39)
}

/**
 * Stop Read Data Continuously mode.
 */
void ADS129X::SDATAC(uint8_t ads) {
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_SDATAC); //SDATAC
    delayMicroseconds(2);
    csHigh(ads);
}

/**
 * Read data by command; supports multiple read back.
 */
void ADS129X::RDATA(uint8_t ads) {
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_RDATA);
    delayMicroseconds(2);
    csHigh(ads);
}

/**
 * Read register at address _address.
 * @param  _address register address
 * @return          value of register
 */
uint8_t ADS129X::RREG(uint8_t ads, uint8_t _address) {
    uint8_t opcode1 = ADS129X_CMD_RREG | (_address & 0x1F); //001rrrrr; _RREG = 00100000 and _address = rrrrr
    csLow(ads);
    SPI2.transfer(opcode1); //RREG
    SPI2.transfer(0x00); //opcode2 1 register
    delayMicroseconds(1);
    uint8_t data = SPI2.transfer(0x00); // returned byte should match default of register map unless edited manually (Datasheet, pg.39)
    delayMicroseconds(2);
    csHigh(ads);
    return data;
}

/**
 * Read _numRegisters register starting at address _address.
 * @param _address      start address
 * @param _numRegisters number of registers
 * @param data          pointer to data array
 */
void ADS129X::RREG(uint8_t ads, uint8_t _address, uint8_t _numRegisters, uint8_t *_data) {
    uint8_t opcode1 = ADS129X_CMD_RREG | (_address & 0x1F); //001rrrrr; _RREG = 00100000 and _address = rrrrr
    csLow(ads);
    SPI2.transfer(opcode1); //RREG
    SPI2.transfer(_numRegisters-1); //opcode2
    for(uint8_t i = 0; i < _numRegisters; i++){
        *(_data+i) = SPI2.transfer(0x00); // returned byte should match default of register map unless previously edited manually (Datasheet, pg.39)
    }
    delayMicroseconds(2);
    csHigh(ads);
}

/**
 * Write register at address _address.
 * @param _address register address
 * @param _value   register value
 */
void ADS129X::WREG(uint8_t ads, uint8_t _address, uint8_t _value) {
    uint8_t opcode1 = ADS129X_CMD_WREG | (_address & 0x1F); //001rrrrr; _RREG = 00100000 and _address = rrrrr
    csLow(ads);
    SPI2.transfer(opcode1);
    SPI2.transfer(0x00); // opcode2; only write one register
    SPI2.transfer(_value);
    delayMicroseconds(2);
    csHigh(ads);
 }

/**
 * Read device ID.
 * @return device ID
 */
uint8_t ADS129X::getDeviceId(uint8_t ads) {
    csLow(ads);
    SPI2.transfer(ADS129X_CMD_RREG); //RREG reg 0
    SPI2.transfer(0x00); //Asking for 1 byte
    uint8_t data = SPI2.transfer(0x00); // byte to read (hopefully 0b???11110)
    delayMicroseconds(2);
    csHigh(ads);
    return data;
}

#ifndef ADS129X_POLLING
/**
 * Interrupt that gets called when DRDY goes HIGH.
 * Transfers data and sets a flag.
 */
void ADS129X_dataReadyISR1() {
    digitalWrite(ADS129X_SSEL1_PIN, LOW);
    // status
    ((uint8_t *) ADS129X_data1)[0*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[0*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[0*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[0*4+0] = SPI2.transfer(0x00);
    // channel 1
    ((uint8_t *) ADS129X_data1)[1*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[1*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[1*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[1*4+0] = SPI2.transfer(0x00);
    // channel 2
    ((uint8_t *) ADS129X_data1)[2*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[2*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[2*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[2*4+0] = SPI2.transfer(0x00);
    // channel 3
    ((uint8_t *) ADS129X_data1)[3*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[3*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[3*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[3*4+0] = SPI2.transfer(0x00);
    // channel 4
    ((uint8_t *) ADS129X_data1)[4*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[4*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[4*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[4*4+0] = SPI2.transfer(0x00);
    // channel 5
    ((uint8_t *) ADS129X_data1)[5*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[5*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[5*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[5*4+0] = SPI2.transfer(0x00);
    // channel 6
    ((uint8_t *) ADS129X_data1)[6*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[6*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[6*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[6*4+0] = SPI2.transfer(0x00);
    // channel 7
    ((uint8_t *) ADS129X_data1)[7*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[7*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[7*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[7*4+0] = SPI2.transfer(0x00);
    // channel 8
    ((uint8_t *) ADS129X_data1)[8*4+3] = 0;
    ((uint8_t *) ADS129X_data1)[8*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[8*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data1)[8*4+0] = SPI2.transfer(0x00);
    digitalWrite(ADS129X_SSEL1_PIN, HIGH);
    ADS129X_newData1 = true;
}

void ADS129X_dataReadyISR2() {
    digitalWrite(ADS129X_SSEL2_PIN, LOW);
    // status
    ((uint8_t *) ADS129X_data2)[0*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[0*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[0*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[0*4+0] = SPI2.transfer(0x00);
    // channel 1
    ((uint8_t *) ADS129X_data2)[1*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[1*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[1*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[1*4+0] = SPI2.transfer(0x00);
    // channel 2
    ((uint8_t *) ADS129X_data2)[2*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[2*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[2*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[2*4+0] = SPI2.transfer(0x00);
    // channel 3
    ((uint8_t *) ADS129X_data2)[3*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[3*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[3*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[3*4+0] = SPI2.transfer(0x00);
    // channel 4
    ((uint8_t *) ADS129X_data2)[4*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[4*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[4*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[4*4+0] = SPI2.transfer(0x00);
    // channel 5
    ((uint8_t *) ADS129X_data2)[5*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[5*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[5*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[5*4+0] = SPI2.transfer(0x00);
    // channel 6
    ((uint8_t *) ADS129X_data2)[6*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[6*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[6*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[6*4+0] = SPI2.transfer(0x00);
    // channel 7
    ((uint8_t *) ADS129X_data2)[7*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[7*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[7*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[7*4+0] = SPI2.transfer(0x00);
    // channel 8
    ((uint8_t *) ADS129X_data2)[8*4+3] = 0;
    ((uint8_t *) ADS129X_data2)[8*4+2] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[8*4+1] = SPI2.transfer(0x00);
    ((uint8_t *) ADS129X_data2)[8*4+0] = SPI2.transfer(0x00);
    digitalWrite(ADS129X_SSEL2_PIN, HIGH);
    ADS129X_newData2 = true;
}
#endif

/**
 * Receive data when in continuous read mode.
 * @param buffer buffer for received data
 * @return true when received data
 */
bool ADS129X::getData(uint8_t ads, int32_t *buffer) {
#ifndef ADS129X_POLLING
    switch (ads)
    {
      case ADS_1:
        if (ADS129X_newData1) {
          ADS129X_newData1 = false;
          for (int i = 0; i < 9; i++) {
            buffer[i] = ADS129X_data1[i];
          }
          return true;
        }
          break;
      case ADS_2:
        if (ADS129X_newData2) {
          ADS129X_newData2 = false;
          for (int i = 0; i < 9; i++) {
            buffer[i] = ADS129X_data2[i];
          }
        return true;
        }
        break;
      case BOTH_ADS:
       if (ADS129X_newData1 && ADS129X_newData2) {
          ADS129X_newData1 = false;
          ADS129X_newData2 = false;
          for (int i = 0; i < 9; i++) {
            buffer[i] = ADS129X_data1[i];
          }
          for (int i = 0; i < 9; i++) {
            buffer[i + 10] = ADS129X_data2[i];
          }
        return true;
       }
        break;
    }
    return false;
#else
    switch (ads)
    {
      case ADS_1:

          if (digitalRead(ADS129X_DRDY1_PIN) == LOW) {
          csLow(ADS1);
          for(uint8_t i = 0; i<9; i++){
            int32_t dataPacket = 0;
            for(int j = 0; j<3; j++){
                uint8_t dataByte = SPI2.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte;
            }
            buffer[i] = dataPacket;
        }
        csHigh(ADS1);
        return true;

      case ADS_2:

          if (digitalRead(ADS129X_DRDY2_PIN) == LOW) {
           csLow(ADS2);
          for(uint8_t i = 0; i<9; i++){
            int32_t dataPacket = 0;
            for(int j = 0; j<3; j++){
                uint8_t dataByte = SPI2.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte;
            }
            buffer[i] = dataPacket;
          }
          csHigh(ADS2);
          return true;

      case BOTH:
          if (digitalRead(ADS129X_DRDY1_PIN && ADS129X_DRDY2_PIN) == LOW) {
          csLow(ADS1);
          for(uint8_t i = 0; i<9; i++){
            int32_t dataPacket = 0;
            for(int j = 0; j<3; j++){
                uint8_t dataByte = SPI2.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte;
            }
            buffer[i] = dataPacket;
          }
          csHigh(ADS1);
          csLow(ADS2);
          for(uint8_t i = 0; i<9; i++){
            int32_t dataPacket = 0;
            for(int j = 0; j<3; j++){
                uint8_t dataByte = SPI2.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte;
            }
            buffer[i + 10] = dataPacket;
          }
          csHigh(ADS2);
         return true;
          break;
      
    }
    
    return false;
#endif
}

/**
 * Configure channel _channel.
 * @param _channel   channel (1-8)
 * @param _powerDown power down (true, false)
 * @param _gain      gain setting
 * @param _mux       mux setting
 */
void ADS129X::configChannel(uint8_t ads, uint8_t _channel, bool _powerDown, uint8_t _gain, uint8_t _mux) {
    uint8_t value = ((_powerDown & 1)<<7) | ((_gain & 7)<<4) | (_mux & 7);
    WREG(ads, ADS129X_REG_CH1SET + (_channel-1), value);
}
