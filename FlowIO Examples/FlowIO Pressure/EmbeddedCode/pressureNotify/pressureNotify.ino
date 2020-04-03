/*This example just like the random number generator example, except that now  
 *the number reported is the pressure value from the sensor in FlowIO.
 *  numberGeneratorService
 *    pressureValueCharacteristic (4-byte) - READ, NOTIFY.
 *    pressureRequestCharacteristic (1-byte) - READ, WRITE.
 *
 *Every 5 seconds, a new pressure reading is made and a notification is sent with the value.
 *Additionally, the central device can also request at any time that a new pressure reading 
 *be made by writing a '1' in the request characteristic. The peripheral device will then 
 *send take a new reading, send the value as a notification, and write 0 to the request characteristic.
 */
 
#include <bluefruit.h>
#include <FlowIO.h>

FlowIO flowio;
int lastTime = millis();

BLEService pressureService;
BLECharacteristic pressureValueCharacteristic;
BLECharacteristic pressureRequestCharacteristic;

void setupServicesAndCharacteristics(){
  pressureService = BLEService(0xAE6F); 
  pressureService.begin();

  pressureValueCharacteristic = BLECharacteristic(0x2947); 
  pressureValueCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY); 
  pressureValueCharacteristic.setFixedLen(4);
  pressureValueCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  pressureValueCharacteristic.begin();

  pressureRequestCharacteristic = BLECharacteristic(0x2948);
  pressureRequestCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE); 
  pressureRequestCharacteristic.setFixedLen(1);
  pressureRequestCharacteristic.setPermission(SECMODE_ENC_NO_MITM, SECMODE_ENC_NO_MITM);
  pressureRequestCharacteristic.setWriteCallback(onWrite);
  pressureRequestCharacteristic.begin();
}

//this is how we are retrieve the value written.
void onWrite(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len){
  Serial.println("Invoked");
  if(len==1){
    Serial.println("Length is 1 byte. Data is:");
    Serial.println(data[0],HEX);
    if(data[0] == 0x01){
      Serial.println("Data is a 0x01");
      notifyNewPressure();
      pressureRequestCharacteristic.write8(0); //This line is not even necessary, because onWrite is executed regardless of the initial value.
    }
  }
}

void notifyNewPressure(){
  float num = flowio.getPressure(PSI);
  pressureValueCharacteristic.notify(&num,sizeof(num));
}

void startAdv(void){
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);   // Advertising packet
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();   // Secondary Scan Response packet (optional)
  //It is vital that we ADVERTISE the services on the device, if someone is searching
  //for that device based on the services it supports. If not advertised, there is no
  //way for a central to know what services are supported before connecting.
  Bluefruit.Advertising.addService(pressureService);   // Include battService uuid  
  Bluefruit.Advertising.restartOnDisconnect(true); //Restart advertising on disconnect.
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void setup(){ 
  //FlowIO must be initilized BEFORE the bluetooth.
  flowio = FlowIO(GENERAL);
  if(flowio.activateSensor()==false) flowio.redLED(HIGH);  //TODO: Have the value true/false be returned in a callback function.

  Serial.begin(9600);
  //Bluetooth must be initilized AFTER FlowIO.
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
    notifyNewPressure();
    lastTime=millis();
  } 
  waitForEvent();  // Request CPU to enter low-power mode until an event/interrupt occurs
}

void connect_callback(uint16_t conn_handle){ // callback invoked when central connects
  notifyNewPressure();
  pressureRequestCharacteristic.write8(0);
}
