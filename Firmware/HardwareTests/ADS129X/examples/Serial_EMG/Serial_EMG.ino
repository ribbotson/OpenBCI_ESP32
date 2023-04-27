#include "ADS129X.h"

ADS129X ADS;




void setup() {
  
  Serial.begin(115200); // always at 12Mbit/s
  Serial.println("Firmware v0.0.1");

  ADS.begin();
  Serial.print("ADS1298 ID = ");
   Serial.println(ADS.getDeviceId(ADS_1),HEX);
  for(int reg = 0; reg < 0x19; reg++){
    Serial.print("reg = ");
    Serial.print(reg,HEX);
    Serial.print(" value = ");
    Serial.println(ADS.RREG(ADS_1, reg),HEX);
  }

  ADS.WREG(ADS_1, ADS129X_REG_CONFIG1, ADS129X_SAMPLERATE_1024); // enable 8kHz sample-rate
  ADS.WREG(ADS_1, ADS129X_REG_CONFIG3, (1<<ADS129X_BIT_PD_REFBUF) | (1<<6)); // enable internal reference
  ADS.WREG(ADS_1, ADS129X_REG_CONFIG2, (1<<ADS129X_BIT_INT_TEST) | ADS129X_TEST_FREQ_2HZ); // Test Signal

  // setup channels
  ADS.configChannel(ADS_1, 1, false, ADS129X_GAIN_12X, ADS129X_MUX_NORMAL);
  ADS.configChannel(ADS_1, 2, false, ADS129X_GAIN_12X, ADS129X_MUX_SHORT);
  ADS.configChannel(ADS_1, 3, false, ADS129X_GAIN_12X, ADS129X_MUX_SHORT);
  ADS.configChannel(ADS_1, 4, false, ADS129X_GAIN_12X, ADS129X_MUX_SHORT);
  ADS.configChannel(ADS_1, 5, false, ADS129X_GAIN_12X, ADS129X_MUX_SHORT);
  ADS.configChannel(ADS_1, 6, false, ADS129X_GAIN_12X, ADS129X_MUX_SHORT);
  ADS.configChannel(ADS_1, 7, false, ADS129X_GAIN_12X, ADS129X_MUX_SHORT);
  ADS.configChannel(ADS_1, 8, false, ADS129X_GAIN_12X, ADS129X_MUX_SHORT);
  

  for(int reg = 0; reg < 0x19; reg++){
    Serial.print("reg = ");
    Serial.print(reg,HEX);
    Serial.print(" value = ");
    Serial.println(ADS.RREG(ADS_1, reg),HEX);
  }


  delay(1);
  ADS.RDATAC(ADS_1);
  ADS.START(ADS_1);

  
}

void loop() {
 int32_t buffer[9];
  static uint32_t tLast;
  if (millis()-tLast > 500) {
     tLast = millis();
  }
  if (ADS.getData(ADS_1, buffer)) {
    uint32_t tStart = micros();
    for (uint8_t channel = 1; channel < 9; channel++) {
      /*if (channel == 8) {
        buffer[channel] = micros()-tStart;
      }*/
      int32_t value = buffer[channel];
      if (value & 1 <<23) value |= 0xff000000; // sign extend
      Serial.print(value);
      Serial.print(",");
      
    }
    Serial.println();
  }
}

