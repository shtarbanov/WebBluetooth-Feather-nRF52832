
void createConfigService(void) {
  uint8_t configServiceUUID[16] = {0x03,0xaa,0x00,0x00,0x00,0x00,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-00000000aa03"
  uint8_t chrConfigUUID[16]     = {0x03,0xaa,0x00,0x00,0x00,0xc1,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa03"
  
  configService = BLEService(configServiceUUID);
  configService.begin();

  chrConfig = BLECharacteristic(chrConfigUUID);
  chrConfig.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  chrConfig.setWriteCallback(onWrite_chrConfig);
  chrConfig.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  chrConfig.setFixedLen(1);
  chrConfig.begin();
  chrConfig.write8(flowio.getConfig());
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrConfig(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if(len==1){
    Configuration cfg; 
    if(data[0] == 0x00) cfg = GENERAL;
    else if(data[0] == 0x01) cfg = INFLATION_SERIES;
    else if(data[0] == 0x02) cfg = INFLATION_PARALLEL;
    else if(data[0] == 0x03) cfg = VACUUM_SERIES;
    else if(data[0] == 0x04) cfg = VACUUM_PARALLEL;
    else{
      cfg = flowio.getConfig();
      chrConfig.write8(cfg);
    }
    flowio.stopAction(0x00011111); //stop esverything before changing the configuration.
    flowio.setConfig(cfg);
  }
}
