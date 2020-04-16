
void createIndicatorService(void) {
  uint8_t indicatorServiceUUID[16] = {0x02,0xaa,0x00,0x00,0x00,0x00,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-00000000aa02"
  uint8_t chrLedStatesUUID[16]     = {0x02,0xaa,0x00,0x00,0x00,0xc1,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa02"
  uint8_t chrErrorUUID[16]         = {0x02,0xaa,0x00,0x00,0x00,0xc2,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c2000000aa02"
  
  indicatorService = BLEService(indicatorServiceUUID);
  indicatorService.begin();

  chrLedStates = BLECharacteristic(chrLedStatesUUID);
  chrLedStates.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
  chrLedStates.setWriteCallback(onWrite_chrLedStates);
  chrLedStates.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrLedStates.setFixedLen(2);
  chrLedStates.begin();

  chrError = BLECharacteristic(chrErrorUUID);
  chrError.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  chrError.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrError.setFixedLen(1);
  chrError.begin();
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrLedStates(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.println("Value Written");
  if(len==2){
    flowio.redLED(data[0]);
    flowio.blueLED(data[1]);
  }
}
