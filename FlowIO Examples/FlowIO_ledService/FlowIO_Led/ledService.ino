
void createLedService(void) {
  #define ledServiceUUIDString "ffff1010-0000-1111-9999-0000000002aa"
  #define chrLedStatesUUIDString "ffff1010-0000-1111-9999-c100000002aa"

  uint8_t ledServiceUUID[16];
  uint8_t chrLedStatesUUID[16];
  
  // Convert String UUID to raw UUID bytes
  uuidStringToByteArray(ledServiceUUIDString, ledServiceUUID);
  uuidStringToByteArray(chrLedStatesUUIDString, chrLedStatesUUID);

  ledService = BLEService(ledServiceUUID);
  ledService.begin();

  chrLedStates = BLECharacteristic(chrLedStatesUUID);
  chrLedStates.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE | CHR_PROPS_NOTIFY);
  chrLedStates.setWriteCallback(onWrite_chrLedStates);
  chrLedStates.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrLedStates.setFixedLen(2);
  chrLedStates.begin();
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrLedStates(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.println("Value Written");
  if(len==2){
    flowio.redLED(data[0]);
    flowio.blueLED(data[1]);
  }
}
