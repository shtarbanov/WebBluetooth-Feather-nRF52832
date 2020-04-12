/*
 * This example demonstrates how to create a service with a characteristic 
 * that stores a floating point number (4-byte), to which you can read and write.
 * The embedded code is distributed across three files.
 */

#include <bluefruit.h>

#define DEVICE_NAME "nrf52 Float"// Device Name: Maximum 30 bytes

BLEService floatService;
BLECharacteristic chrFloatValue;

void setup() {
  Serial.begin(9600);
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.setTxPower(4);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  createFloatService();
  startAdvertising(); 
}

void startAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.addService(floatService); //advertise service uuic

  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
  
  Bluefruit.Advertising.start(0);
}

void loop() {
}

void connect_callback(uint16_t conn_handle){
  float num = 1.234;
  chrFloatValue.write(&num,sizeof(num));
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
}
