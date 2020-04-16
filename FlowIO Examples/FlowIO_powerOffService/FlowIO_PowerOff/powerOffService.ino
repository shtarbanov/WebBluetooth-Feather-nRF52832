//NOTE: Remember that you MUST execute 'resetOffTimer()' inside each event loop that you consider as non-idle event.

uint8_t inactiveTimeLimit[] = {5}; //minutes of inactivity until power off;
int offTimerStart = millis();
bool remaining1minute=false;
bool remaining2minute=false;
bool remaining3minute=false;
uint8_t remainingTimeUint;

void createPowerOffService(void) {
  const uint8_t powerServiceUUID[16]              = {0x01,0xaa,0x00,0x00,0x00,0x00,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-00000000aa01"
  const uint8_t chrPowerOffNowUUID[16]            = {0x01,0xaa,0x00,0x00,0x00,0xc1,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa01"
  const uint8_t chrPowerOffWhenInactiveForUUID[16]= {0x01,0xaa,0x00,0x00,0x00,0xc2,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c2000000aa01"
  const uint8_t chrPowerOffRemainingTimeUUID[16]  = {0x01,0xaa,0x00,0x00,0x00,0xc3,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c3000000aa01"

  powerOffService = BLEService(powerServiceUUID);
  powerOffService.begin();

  chrPowerOffNow = BLECharacteristic(chrPowerOffNowUUID);
  chrPowerOffNow.setProperties(CHR_PROPS_WRITE);
  chrPowerOffNow.setWriteCallback(onWrite_chrPowerOffNow);
  chrPowerOffNow.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrPowerOffNow.setFixedLen(1);
  chrPowerOffNow.begin();

  chrPowerOffWhenInactiveFor = BLECharacteristic(chrPowerOffWhenInactiveForUUID);
  chrPowerOffWhenInactiveFor.setProperties(CHR_PROPS_WRITE | CHR_PROPS_READ);
  chrPowerOffWhenInactiveFor.setWriteCallback(onWrite_chrPowerOffWhenInactiveFor);
  chrPowerOffWhenInactiveFor.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrPowerOffWhenInactiveFor.setFixedLen(1);
  chrPowerOffWhenInactiveFor.begin();
  chrPowerOffWhenInactiveFor.write(inactiveTimeLimit,1); //this MIUST go after .begin().

  chrPowerOffRemainingTime = BLECharacteristic(chrPowerOffRemainingTimeUUID);
  chrPowerOffRemainingTime.setProperties(CHR_PROPS_NOTIFY);
  chrPowerOffRemainingTime.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrPowerOffRemainingTime.setFixedLen(1);
  chrPowerOffRemainingTime.begin();
  chrPowerOffRemainingTime.write(inactiveTimeLimit,1); //this MIUST go after .begin().
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrPowerOffNow(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if(len==1){
    if(data[0]==0x01) flowio.powerOFF();
  }
}

void onWrite_chrPowerOffWhenInactiveFor(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  resetOffTimer(); //reset whenever something is written, even if invalid entry
  if(len==1){
    inactiveTimeLimit[0] = data[0];
  }
}

void resetOffTimer(){   //You must run this function in each event that you consider "active" event 
  offTimerStart = millis();
  Serial.println("RESET TIMER");
  remaining1minute=false;
  remaining2minute=false;
  remaining3minute=false;
}

void powerOffIfInactiveTimeLimitReached(){
  if(inactiveTimeLimit[0]==0) return;
  
  float remainingtime = inactiveTimeLimit[0] - (millis() - offTimerStart)/60000.0f;
  if(remainingtime <= 0){     //IF YOU GET THIS SIGN MIXED WITH >=, YOU WILL BRICK YOUR DEVICE!!!
    flowio.powerOFF();
    resetOffTimer(); //This is useful in case of a hardware failure and power is not cut off properly.
  }
  Serial.println(remainingtime);
  if(remainingtime<=1 && remaining1minute==false){
    Serial.println("1 minute till off");
    remaining1minute=true; //this is to ensure that we execute this only once.
    chrPowerOffRemainingTime.notify8(1);
  }
  else if(remainingtime<=2 && remaining2minute==false){
    Serial.println("2 minutes till off");
    remaining2minute=true;
    chrPowerOffRemainingTime.notify8(2);
  }
  else if(remainingtime<=3 && remaining3minute==false){
    Serial.println("3 minutes till off");
    remaining3minute=true;
    chrPowerOffRemainingTime.notify8(3);
  }
}
