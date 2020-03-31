/*
  This example shows how to control the state of each of the two LEDs on the nrf52832 Feather
  board. The microcontorller has been set up so that is has one custom service with a single
  characteristic that holds a 2-byte value. The two bytes in the characteristic correspond to
  the state of the two LEDs on the board. The characteristic has a read and write persmisions.
    In this JavaScript code, we are connecting to the device and reading the current value of
  the characteristic. Then we are toggling one of the two bytes depending on which button the
  user presses on the screen, which in turn causes the corresponding LED to toggle as well. 

  This example uses the Async / Await approach.
*/
'use strict'

const serviceUUID = 'a3941db0-a97c-4cf1-943f-a25ff9ba40cd';
const ledCharacteristicUUID = '5b8c0ab6-a058-4684-b2b6-4a0a692e2d45';

let bleDevice;
let bleServer;
let ledService;
let ledCharacteristic;
let valueArray;
let stateRed = true;
let stateBlue = true;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#red').addEventListener('click', toggleRed);
  document.querySelector('#blue').addEventListener('click', toggleBlue);
  document.querySelector('#read').addEventListener('click', readCharacteristicValue);
};

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [serviceUUID]
        });
    bleServer = await bleDevice.gatt.connect();
    ledService = await bleServer.getPrimaryService(serviceUUID);
    ledCharacteristic = await ledService.getCharacteristic(ledCharacteristicUUID);
    
    //##################
    //Now we want to read the current value of the characteristic and set our LEDstate
    //variables in JavaScript to match those from the characteristic.
    //##################

    let valueDataView = await ledCharacteristic.readValue(); //returns a DataView.
    //If you didn't know that object is of type "DataView" you could just do
    //console.log(valueDataView) and then you will see all infor about this variable in the console.

    //We now convert the DataView to TypedArray so we can use array notation to access the data.
    //https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/buffer
    valueArray = new Uint8Array(valueDataView.buffer);
    log("Connected using Async function!\n The value is: \nQ1=" + valueArray[1] + " Q0=" + valueArray[0]);
    stateBlue = valueArray[1];
    stateRed = valueArray[0];
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
  let val = await ledCharacteristic.readValue(); //this returns a DataView
  log("Q1=" + val.getUint8(1) + " Q0=" + val.getUint8(0));
}
async function toggleRed(){
    valueArray[0] = (stateRed) ? 0x00 : 0x01;
    stateRed = !stateRed;
    await ledCharacteristic.writeValue(valueArray);
}
async function toggleBlue(){
    valueArray[1] = (stateBlue) ? 0x00 : 0x01;
    stateBlue = !stateBlue;
    await ledCharacteristic.writeValue(valueArray);
}
function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}