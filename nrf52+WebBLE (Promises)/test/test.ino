//TODO: In the onConnect method, set the initial value of the characteristic, and then set the LED states
//to the bytes of the characteristic.

#include <bluefruit.h>
#define DEVICE_NAME "Nordic_Blinky"// Device Name: Maximum 30 bytes
// User service UUID: Change this to your generated service UUID
#define USER_SERVICE_UUID "a3941db0-a97c-4cf1-943f-a25ff9ba40cd"
#define WRITE_CHARACTERISTIC_UUID "5b8c0ab6-a058-4684-b2b6-4a0a692e2d45"

#define redLed 17 //red led
#define blueLed 19

uint8_t userServiceUUID[16];
uint8_t writeCharacteristicUUID[16];

BLEService userService;
BLECharacteristic writeCharacteristic;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  setupServices();
  startAdvertising();
  Serial.begin(9600);

}

void loop() {
}

void setupServices(void) {
  // Convert String UUID to raw UUID bytes
  strUUID2Bytes(USER_SERVICE_UUID, userServiceUUID);
  strUUID2Bytes(WRITE_CHARACTERISTIC_UUID, writeCharacteristicUUID);

  // Setup User Service
  userService = BLEService(userServiceUUID);
  userService.begin();

  writeCharacteristic = BLECharacteristic(writeCharacteristicUUID);
  writeCharacteristic.setProperties(CHR_PROPS_WRITE | CHR_PROPS_READ);
  writeCharacteristic.setWriteCallback(writeLEDCallback);
  writeCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  writeCharacteristic.setFixedLen(2);
  writeCharacteristic.begin();
}

void startAdvertising(void) {
  // Start Advertising
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(userService);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);
}

//###########################---Callbacks---##################################

//This is executed when a central device writes to the characteristic.This is how we read the characteristic.
void writeLEDCallback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  if(len==1){
    int firstByte = data[0];
    digitalWrite(redLed, firstByte);
  } 
  else  if(len==2){
    Serial.print(data[0], BIN); //upper byte
    Serial.print(" ");
    Serial.println(data[1],BIN); //lower byte
    int firstByte = data[0];
    char secondByte = data[1];
    digitalWrite(redLed, firstByte);
    digitalWrite(blueLed, secondByte);
  } 
  else {
    Serial.println("Not a valid input");
  }
}

void connect_callback(uint16_t conn_handle){
  //Set the initial value of the characteristic and the initial states of the LEDs to that value.
  Serial.println("Connected");
  uint8_t chrVals[] = {0x01,0x01};
  digitalWrite(redLed,chrVals[0]);
  digitalWrite(blueLed,chrVals[1]);
  writeCharacteristic.write(chrVals,2);
  
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
  Serial.println("Disconnected");
  digitalWrite(redLed,LOW);
  digitalWrite(blueLed,LOW);
}
//###########################---UUID Converter---#################################
void strUUID2Bytes(String strUUID, uint8_t binUUID[]) {
  String hexString = String(strUUID);
  hexString.replace("-", "");

  for (int i = 16; i != 0 ; i--) {
    binUUID[i - 1] = hex2c(hexString[(16 - i) * 2], hexString[((16 - i) * 2) + 1]);
  }
}

char hex2c(char c1, char c2) {
  return (nibble2c(c1) << 4) + nibble2c(c2);
}

char nibble2c(char c) {
  if ((c >= '0') && (c <= '9'))
    return c - '0';
  if ((c >= 'A') && (c <= 'F'))
    return c + 10 - 'A';
  if ((c >= 'a') && (c <= 'f'))
    return c + 10 - 'a';
  return 0;
}
