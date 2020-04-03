/* 
  This example reports battery level changes. It uses an event listener that triggers whenver 
  the BLE device sends a notification from the battery level characteristic. It also shows the
  time when the event has occurred. There are many other googies you can find in the 'event' object,
  which is returned when the event fires.
    The user also has the ability to read the current in the characteristic at any time.
*/
'use strict'

let bleDevice;
let bleServer;
let batteryService;
let batteryLevelCharacteristic;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#read').addEventListener('click', readCharacteristicValue);
};

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: ['battery_service']
    });
    bleServer = await bleDevice.gatt.connect();
    batteryService = await bleServer.getPrimaryService('battery_service'); //uuid is 0x180F
    batteryLevelCharacteristic = await batteryService.getCharacteristic('battery_level'); //uuid is 0x2A19
    

    //This is reading the value of the characteristic and displaying it
    let valueDataView = await batteryLevelCharacteristic.readValue(); //returns a DataView.
    log("Connected!\n" + valueDataView.getUint8(0) + "%");


    //Respond to notifications. Notifications will be sent when level < 10%
    await batteryLevelCharacteristic.startNotifications();
    batteryLevelCharacteristic.addEventListener('characteristicvaluechanged', event => { //an event is returned 
      log(event.target.value.getUint8(0)+'%');
      console.log(event); //we can use this in the console to see all the goodies in the event object.
    })
      //NOTE: The characteristic value is automatically passed to the callback function
  }
  catch(error){
    log("Ouch! " + error);
  }
}
function onDisconnectButtonClick() {
  if (!bleDevice) {
    log('No device found');
  }
  else if (bleDevice.gatt.connected) {
    log('Disconnecting');
    bleDevice.gatt.disconnect();
  } 
  else {
    log('Device already disconnected');
  }
}
async function readCharacteristicValue(){
  let val = await batteryLevelCharacteristic.readValue(); //this returns a DataView, and also triggers the
  //'characteristicvaluechanged' notification. Thus it is unnecessary to log this value here.
}
function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}