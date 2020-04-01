/*This example shows how to implement a service with 2 characteristics.  
 *  numberGeneratorService
 *    floatNumberCharacteristic (4-byte) - READ, NOTIFY.
 *    requestNewNumberCharacteristic (1-byte) - READ, WRITE.
 *
 *Every 5 seconds, a new random number is generated and a notification is sent with the number.
 *Additionally, the central device can also request at any time that a new random number be generated
 *by writing a '1' in the request characteristic. The peripheral device will then generate a new 
 *random number, send it as a notification, and set the value in the request characteristic back to 0.
 */
#include <bluefruit.h>

int lastTime = millis();

//All included services are in: C:\Users\Ali\AppData\Local\Arduino15\packages\adafruit\hardware\nrf52\0.14.0\libraries\Bluefruit52Lib\src\services
BLEService randomGeneratorService;
BLECharacteristic floatNumberCharacteristic;
BLECharacteristic requestNewNumberCharacteristic;

void setupServicesAndCharacteristics(){
  randomGeneratorService = BLEService(0xAE6F); 
  randomGeneratorService.begin();

  floatNumberCharacteristic = BLECharacteristic(0x2947); 
  floatNumberCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY); 
  floatNumberCharacteristic.setFixedLen(4);
  floatNumberCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  floatNumberCharacteristic.begin();

  requestNewNumberCharacteristic = BLECharacteristic(0x2948);
  requestNewNumberCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE); 
  requestNewNumberCharacteristic.setFixedLen(1);
  requestNewNumberCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  requestNewNumberCharacteristic.setWriteCallback(onWrite);
  requestNewNumberCharacteristic.begin();
}

//this is how re retrieve the value written.
void onWrite(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len){
  Serial.println("Invoked");
  if(len==1){
    Serial.println("Length is 1 byte. Data is:");
    Serial.println(data[0],HEX);
    if(data[0] == 0x01){
      Serial.println("Data is a 0x01");
      newRandomNumberNotification();
      requestNewNumberCharacteristic.write8(0); //This line is not even necessary, because onWrite is executed regardless of the initial value.
    }
  }
}



void setup(){ 
  Serial.begin(9600);
  Bluefruit.autoConnLed(true);   // Setup the BLE LED to be enabled on CONNECT
  //All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); // Config the peripheral connection with maximum bandwidth 
  Bluefruit.begin();
  Bluefruit.setTxPower(0); // Set max power. Accepted values: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setName("nrf52 random");
  Bluefruit.Periph.setConnectCallback(connect_callback);
  setupServicesAndCharacteristics(); 
  startAdv();   // Set up and start advertising
}

void loop(){ 
  if(millis() - lastTime > 5000){ //we wiil check the battery only once per 5 seconds. This reduces power consumption greatly.
    newRandomNumberNotification();
  } 
  waitForEvent();  // Request CPU to enter low-power mode until an event/interrupt occurs

  //I want to now read the value of the requestNewNumberCharacteristic and if is 1 then I execute the
  //sendRandomNumberNotification. But is there a way to do this without polling. Is there a callback
  //I can just set up on the characteristic itself?
}

void newRandomNumberNotification(){
  float num = random(100,999) + random(0,99)/100.0f;
  lastTime = millis();
  floatNumberCharacteristic.notify(&num,sizeof(num));
}

void startAdv(void){
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);   // Advertising packet
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();   // Secondary Scan Response packet (optional)
  //It is vital that we ADVERTISE the services on the device, if someone is searching
  //for that device based on the services it supports. If not advertised, there is no
  //way for a central to know what services are supported before connecting.
  Bluefruit.Advertising.addService(randomGeneratorService);   // Include battService uuid  
  Bluefruit.Advertising.restartOnDisconnect(true); //Restart advertising on disconnect.
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void connect_callback(uint16_t conn_handle){ // callback invoked when central connects
  newRandomNumberNotification();
  requestNewNumberCharacteristic.write8(0);
}
