/*  This example shows what we can have on a BLE device looks like when no services
 *   or characteristics are implemented. Surprisingly, we can still have a very large
 *   amount of imformation.
 */
#include <bluefruit.h>

void setup(){ 
  Bluefruit.autoConnLed(true);   // Setup the BLE LED to be enabled on CONNECT
  //All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); // Config the peripheral connection with maximum bandwidth 
  Bluefruit.begin();
  Bluefruit.setTxPower(4); // Set max power. Accepted values: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setName("nrf52 null");
  Bluefruit.Periph.setConnectCallback(connect_callback);
  startAdv();   // Set up and start advertising
}

void loop(){ 
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
}
