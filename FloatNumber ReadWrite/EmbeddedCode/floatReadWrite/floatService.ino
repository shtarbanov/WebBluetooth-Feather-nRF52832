

void createFloatService(void) {
  #define floatServiceUUIDString "ffff1010-0000-1111-9999-0000000005aa"
  #define chrFloatValueUUIDString "ffff1010-0000-1111-9999-c100000005aa"

  uint8_t floatServiceUUID[16];
  uint8_t chrFloatValueUUID[16];
  
  // Convert String UUID to raw UUID bytes
  uuidStringToByteArray(floatServiceUUIDString, floatServiceUUID);
  uuidStringToByteArray(chrFloatValueUUIDString, chrFloatValueUUID);

  floatService = BLEService(floatServiceUUID);
  floatService.begin();

  chrFloatValue = BLECharacteristic(chrFloatValueUUID);
  chrFloatValue.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  chrFloatValue.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrFloatValue.setFixedLen(4);
  chrFloatValue.setWriteCallback(onWrite_chrFloatValue);
  chrFloatValue.begin();
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrFloatValue(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.println("Value: ");
  if(len==4){
    byte byteArray[4] = {data[0],data[1],data[2],data[3]};
    float x = *(float *)&byteArray;
    Serial.println(x);
  }
}
