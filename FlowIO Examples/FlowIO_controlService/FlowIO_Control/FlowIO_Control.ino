//When I send the command A! is the ! element 0 of the array?

#include <bluefruit.h>
#include <FlowIO.h>

#define DEVICE_NAME "nrf52 Control"// Device Name: Maximum 30 bytes

FlowIO flowio;

BLEService controlService;
BLECharacteristic chrCommand;
BLECharacteristic chrHardwareStatus;

uint8_t cmd[2];

void setup() {
  Serial.begin(9600);
  flowio = FlowIO(GENERAL);
  //flowio.activateSensor();
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  createControlService();
  startAdvertising();
  
}

void startAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.addService(controlService); //advertise service uuic

  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
  
  Bluefruit.Advertising.start(0);
}

void loop() {
    //Serial.println(flowio.getCurrentHardwareState());
}

void connect_callback(uint16_t conn_handle){
  //stop everything on connect
  Serial.println("Connected");
  cmd[0] = '!';
  cmd[1] = 0b00011111;
  flowio.command(cmd[0],cmd[1]);
  chrCommand.write(cmd,2);
  chrHardwareStatus.notify16(flowio.getHardwareStatus());
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
  //stop everything on disconnect
  cmd[0] = '!';
  cmd[1] = 0b00011111;
  flowio.command(cmd[0],cmd[1]);
}
