
void createControlService(void) {
  uint8_t controlServiceUUID[16]   = {0x04,0xaa,0x00,0x00,0x00,0x00,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-00000000aa04"
  uint8_t chrCommandUUID[16]       = {0x04,0xaa,0x00,0x00,0x00,0xc1,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa04"
  uint8_t chrHardwareStatusUUID[16]= {0x04,0xaa,0x00,0x00,0x00,0xc2,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c2000000aa04"

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
