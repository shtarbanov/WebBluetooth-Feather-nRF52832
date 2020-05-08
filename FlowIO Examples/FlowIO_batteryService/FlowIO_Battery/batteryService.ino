//battery percentage (https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/nrf52-adc)
#define VBAT_PIN          A7            //A7 for nrf52832, A6 for nrf52840
#define VBAT_RESOLUTION   0.73242188F   //For 12bit @ 3V reference, resolution is 3000mV/4096steps = 0.732421875mV/step.
#define VBAT_DIVIDER_COMP 1.403F        // this is 4.2/3
#define REAL_VBAT_RESOLUTION (VBAT_RESOLUTION * VBAT_DIVIDER_COMP) //this is in units of millivolts/step.


uint8_t batteryPercentage = 0; 
uint8_t batteryPercentageOld = 0; 
int lastTime = millis();

float readVBAT() {
  int raw=0;
  float vbatMv; //battery voltage in millivolt
  raw = analogRead(VBAT_PIN);   // Get the raw 12-bit, 0..3000mV ADC value
  vbatMv = raw * REAL_VBAT_RESOLUTION; //we multiply the read value times the millivols per step.
  return vbatMv; 
}

uint8_t getBatteryPercentage(){ //this computation is based on LiPo chemistry. 
  float mv = readVBAT();
  if(mv<3300)    return 0; 
  if(mv<3600) {
    mv -= 3300;
    return mv/30;
  } 
  mv -= 3600;
  return 10 + (mv * 0.15F );  // thats mv/6.66666666
}

void updateBatteryLevelEvery(int interval){
  if(Bluefruit.connected()){ //This is mandatory here becasue we cannot execute .notify8() unless connected.
    if(millis() - lastTime > interval){ //we wiil check the battery only once per 5 seconds. This reduces power consumption greatly.
      Serial.println("Read Battery Value");
      batteryPercentage = getBatteryPercentage();
      Serial.println(batteryPercentage);
      lastTime = millis();
      if(batteryPercentage < batteryPercentageOld-1 || batteryPercentage > batteryPercentageOld+1){
        if(batteryPercentage < 10)  chrBattPercentage.notify8(batteryPercentage);
        else                        chrBattPercentage.write8(batteryPercentage);
        batteryPercentageOld = batteryPercentage;
      }
    }
  }
}

void createBatteryService(){
  batteryService = BLEService(0x180F);
  batteryService.begin();

  chrBattPercentage = BLECharacteristic(0x2A19);
  chrBattPercentage.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY); 
  chrBattPercentage.setFixedLen(1);
  chrBattPercentage.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    //For the permission, the first parameter is the READ, second the WRITE permission
  chrBattPercentage.begin();
}
