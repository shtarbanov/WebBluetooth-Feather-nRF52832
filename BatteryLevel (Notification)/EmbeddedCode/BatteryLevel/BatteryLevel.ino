/*   >> Read the battery percentage of LiPo battery connected to the Feather nrf52832 once every 5 seconds.
 *   >> Implement the GATT battery service and characteristic.
 *   >> Send a notification to the connected central with the battery level only when it changes.
 *        (During any time, the central still has the ability to request the battery level)
 *        (See this video for explanation: https://www.youtube.com/watch?v=oCMOYS71NIU)
 *  
 */

#include <bluefruit.h>

//battery percentage (https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/nrf52-adc)
#define VBAT_PIN          A7            //A7 for nrf52832, A6 for nrf52840
#define VBAT_RESOLUTION   0.73242188F   //For 12bit @ 3V reference, resolution is 3000mV/4096steps = 0.732421875mV/step.
#define VBAT_DIVIDER_COMP 1.403F        // this is 4.2/3
#define REAL_VBAT_RESOLUTION (VBAT_RESOLUTION * VBAT_DIVIDER_COMP) //this is in units of millivolts/step.


uint8_t batteryPercentage = 0; 
uint8_t batteryPercentageOld = 0; 
int lastTime = millis();

float readVBAT() {
  int raw=0;
  float vbatMv; //battery voltage in millivolt
  raw = analogRead(VBAT_PIN);   // Get the raw 12-bit, 0..3000mV ADC value
  vbatMv = raw * REAL_VBAT_RESOLUTION; //we multiply the read value times the millivols per step.
  return vbatMv; 
}

uint8_t getBatteryPercentage(){ //this computation is based on LiPo chemistry. 
  float mv = readVBAT();
  if(mv<3300)    return 0; 
  if(mv<3600) {
    mv -= 3300;
    return mv/30;
  } 
  mv -= 3600;
  return 10 + (mv * 0.15F );  // thats mv/6.66666666
}

//All included services are in: C:\Users\Ali\AppData\Local\Arduino15\packages\adafruit\hardware\nrf52\0.14.0\libraries\Bluefruit52Lib\src\services
BLEBas  batteryService;

void setup(){ 
  analogReference(AR_INTERNAL_3_0);   // Set the analog reference to 3.0V (default = 3.6V) b/c battery voltage may drop to <3.6V and then default will become inaccurate.
  analogReadResolution(12);   // Set the resolution to 12-bit (0..4095). Can be 8, 10, 12 or 14
  Bluefruit.autoConnLed(true);   // Setup the BLE LED to be enabled on CONNECT
  //All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); // Config the peripheral connection with maximum bandwidth 
  Bluefruit.begin();
  Bluefruit.setTxPower(0); // Set max power. Accepted values: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setName("nrf52 BatteryLevel");
  Bluefruit.Periph.setConnectCallback(connect_callback);
  batteryService.begin(); // Start Battery Service
  startAdv();   // Set up and start advertising
}

void loop(){ 
  if(millis() - lastTime > 5000){ //we wiil check the battery only once per 5 seconds. This reduces power consumption greatly.
    batteryPercentage = getBatteryPercentage();
    lastTime = millis();
    if(batteryPercentage < batteryPercentageOld-1 || batteryPercentage > batteryPercentageOld+1){
      batteryService.notify(batteryPercentage);
      batteryPercentageOld = batteryPercentage;
    }
  } 
  waitForEvent();  // Request CPU to enter low-power mode until an event/interrupt occurs
}

void startAdv(void){
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);   // Advertising packet
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();   // Secondary Scan Response packet (optional)
  Bluefruit.Advertising.restartOnDisconnect(true); //Restart advertising on disconnect.
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void connect_callback(uint16_t conn_handle){ // callback invoked when central connects
  batteryPercentageOld = getBatteryPercentage();
  batteryService.notify(batteryPercentageOld);
}
