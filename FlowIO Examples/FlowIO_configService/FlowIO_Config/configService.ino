
void createConfigService(void) {
  #define configServiceUUIDString "ffff1010-0000-1111-9999-0000000003aa"
  #define chrPneumaticConfigUUIDString "ffff1010-0000-1111-9999-c100000003aa"

  uint8_t configServiceUUID[16];
  uint8_t chrPneumaticConfigUUID[16];
  
  // Convert String UUID to raw UUID bytes
  uuidStringToByteArray(configServiceUUIDString, configServiceUUID);
  uuidStringToByteArray(chrPneumaticConfigUUIDString, chrPneumaticConfigUUID);

  configService = BLEService(configServiceUUID);
  configService.begin();

  chrPneumaticConfig = BLECharacteristic(chrPneumaticConfigUUID);
  chrPneumaticConfig.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  chrPneumaticConfig.setWriteCallback(onWrite_chrPneumaticConfig);
  chrPneumaticConfig.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrPneumaticConfig.setFixedLen(1);
  chrPneumaticConfig.begin();
  chrPneumaticConfig.write8(flowio.getConfig());
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrPneumaticConfig(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if(len==1){
    Configuration cfg; 
    if(data[0] == 0x00) cfg = GENERAL;
    else if(data[0] == 0x01) cfg = INFLATION_SERIES;
    else if(data[0] == 0x02) cfg = INFLATION_PARALLEL;
    else if(data[0] == 0x03) cfg = VACUUM_SERIES;
    else if(data[0] == 0x04) cfg = VACUUM_PARALLEL;
    else{
      cfg = flowio.getConfig();
      chrPneumaticConfig.write8(cfg);
    }
    flowio.stopAction(0x00011111); //stop esverything before changing the configuration.
    flowio.setConfig(cfg);
  }
}
