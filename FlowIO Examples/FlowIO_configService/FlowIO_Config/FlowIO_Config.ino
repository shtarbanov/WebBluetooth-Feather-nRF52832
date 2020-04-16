//TODO: Make the configuration change persist after reboot! 

/*This example implements the configuration service.
A device connected to the flowIO can change the pneumatic
configuration by writing a value to the characteristic. 
The current configuration can also be read from the characteristic.
*/

#include <bluefruit.h>
#include <FlowIO.h>

#define DEVICE_NAME "nrf52 Config"// Device Name: Maximum 30 bytes

FlowIO flowio;

BLEService configService;
BLECharacteristic chrConfig;

void setup() {
  flowio = FlowIO(GENERAL);
  
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  createConfigService(); //This is defined in the file "powerService.ino"
  startAdvertising();
}

void startAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.addService(configService); //advertise service uuic

  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
  
  Bluefruit.Advertising.start(0);
}

void loop() {
  flowio.startInflation(0x00000001);
  delay(1000);
  flowio.stopAction(0x00000001);
  delay(1000);
}

void connect_callback(uint16_t conn_handle){
  chrConfig.notify8(flowio.getConfig());
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
  flowio.stopAction(0x00011111);
}
