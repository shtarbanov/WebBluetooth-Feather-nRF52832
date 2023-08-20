/*This code sets up the nrf52832 Feather as a peripheral with one custom service and one characteristic 
The characteristic has read and write permissions, and contains a 2-byte value, which we map to the state of the LEDs.
Whenver the value of the characteristic is changed by another device (a central), the LED states also change accoridngly.

Whenever the value of the characteristic changes, a callback function is executed that reads the
new value and then sets the state of the LEDs accordingly. Notice that thee main loop is completely empty.
We are not doing any polling, but rather everything is event driven and based purely on callback functions.

That is very nice with this approach where we are working directly with the characteristic, is that we don't
have to worry about matching baud rates, we don't have to constantly be polling and checking for data which 
saves energy, and we have full flexibility in what we want to do. This approach is way better than BLE UART.
*/

/*  TODO: Look more indepth into the advertising settings, flags, permissions.
 *  TODO: How are my communication parameters, tx power, etc.
 *  TODO: Can you add encription?
 */
#include <bluefruit.h>
#define DEVICE_NAME "nrf52 LED Control"// Device Name: Maximum 30 bytes
#define LED_SERVICE_UUID "a3941db0-a97c-4cf1-943f-a25ff9ba40cd"
#define LED_CHARACTERISTIC_UUID "5b8c0ab6-a058-4684-b2b6-4a0a692e2d45"

#define redLed 17 //red led
#define blueLed 19

uint8_t ledServiceUUID[16];
uint8_t ledCharacteristicUUID[16];

BLEService ledService;
BLECharacteristic ledCharacteristic;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  setupServices();
  startAdvertising();
}

void loop() {
}

void setupServices(void) {
  ledService = BLEService(LED_SERVICE_UUID);
  ledService.begin();

  ledCharacteristic = BLECharacteristic(LED_CHARACTERISTIC_UUID);
  ledCharacteristic.setProperties(CHR_PROPS_WRITE | CHR_PROPS_READ);
  ledCharacteristic.setWriteCallback(getCharacteristicValueAndSetLEDs);
  ledCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
    //For the permission, the first parameter is the READ, second the WRITE permission  
  ledCharacteristic.setFixedLen(2);
  ledCharacteristic.begin();
}

void startAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(ledService);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);
}

//###########################---Callbacks---##################################

//This is executed when a central device writes to the characteristic.
void getCharacteristicValueAndSetLEDs(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if(len==2){
    int firstByte = data[0];
    char secondByte = data[1];
    digitalWrite(redLed, firstByte);
    digitalWrite(blueLed, secondByte);
  }
}
void connect_callback(uint16_t conn_handle){
  //Set the initial value of the characteristic and the initial states of the LEDs to that value.
  uint8_t chrVals[] = {0x01,0x01};
  digitalWrite(redLed,chrVals[0]);
  digitalWrite(blueLed,chrVals[1]);
  ledCharacteristic.write(chrVals,2);
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
  digitalWrite(redLed,LOW);
  digitalWrite(blueLed,LOW);
}

