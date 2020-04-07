
void createControlService(void) {
  #define controlServiceUUIDString "ffff1010-0000-1111-9999-0000000004aa"
  #define chrCommandUUIDString "ffff1010-0000-1111-9999-c100000004aa"
  #define chrHardwareStatusUUIDString "ffff1010-0000-1111-9999-c200000004aa"

  uint8_t controlServiceUUID[16];
  uint8_t chrCommandUUID[16];
  uint8_t chrHardwareStatusUUID[16];
  
  // Convert String UUID to raw UUID bytes
  uuidStringToByteArray(controlServiceUUIDString, controlServiceUUID);
  uuidStringToByteArray(chrCommandUUIDString, chrCommandUUID);
  uuidStringToByteArray(chrHardwareStatusUUIDString, chrHardwareStatusUUID);

  controlService = BLEService(controlServiceUUID);
  controlService.begin();

  chrCommand = BLECharacteristic(chrCommandUUID);
  chrCommand.setProperties(CHR_PROPS_WRITE);
  chrCommand.setWriteCallback(onWrite_chrCommand);
  chrCommand.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrCommand.setFixedLen(2);
  chrCommand.begin();

  chrHardwareStatus = BLECharacteristic(chrHardwareStatusUUID);
  chrHardwareStatus.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  chrHardwareStatus.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrHardwareStatus.setFixedLen(2);
  chrHardwareStatus.begin();
  chrHardwareStatus.write16(flowio.getHardwareStatus());
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrCommand(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.println("Value Written");
  //I don't know if the order of these is correct.
  
  if(len==2){
    Serial.print(data[0],BIN);
    Serial.println(data[1],BIN);
    flowio.command(data[0],data[1]);
  }
  chrHardwareStatus.notify16(flowio.getHardwareStatus());
}
