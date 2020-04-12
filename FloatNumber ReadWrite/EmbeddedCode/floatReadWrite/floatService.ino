

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
  chrFloatValue.begin();
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrFloatValue(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.println("Value: ");
  if(len==4){
    uint32_t rawData;
    rawData = data[0];
    rawData << 8;
    rawData = rawData | data[1];
    rawData << 8;
    rawData = rawData | data[2];
    rawData << 8;
    rawData = rawData | data[3];
    Serial.println(rawData);
    Serial.println((float) rawData);
  }
}

void onWrite_chrMinPressureLimits(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {

}
