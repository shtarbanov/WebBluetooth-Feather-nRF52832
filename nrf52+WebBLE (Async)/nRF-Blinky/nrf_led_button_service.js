/* 
  This example shows how to control the state of each of the two LEDs on the nrf52832 Feather
  board. The microcontorller has been set up so that is has one custom service with a single
  characteristic that holds a 2-byte balue. The two bytes in the characteristic correspond to
  the state of the two LEDs on the board. The characteristic has a read and write persmisions.
    In this JavaScript code, we are connecting to the device and reading the current value of
  the characteristic. Then we are toggling one of the two bytes depending on which button the
  user presses on the screen, which in turn causes the corresponding LED to toggle as well. 
*/
'use strict'

const serviceUUID = 'a3941db0-a97c-4cf1-943f-a25ff9ba40cd';
const ledCharacteristicUUID = '5b8c0ab6-a058-4684-b2b6-4a0a692e2d45';



let bleDevice;
let bleServer;
let bleService;
let button1char;
let ledCharacteristic;
let button1count = 0;
let stateRed = true;
let stateBlue = true;
let theValue = new Uint8Array(2);

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
          filters: [{namePrefix: 'Nordic'}],
          optionalServices: [serviceUUID]
        });
    bleServer = await bleDevice.gatt.connect();
    bleService = await bleServer.getPrimaryService(serviceUUID);
    ledCharacteristic = await bleService.getCharacteristic(ledCharacteristicUUID);
    let value = await ledCharacteristic.readValue();

    log("Connected! The value is: \nQ1=" + value.getUint8(1) + " Q0=" + value.getUint8(0));
    stateBlue = value.getUint8(1);
    stateRed = value.getUint8(0);
    theValue[1] = value.getUint8(1);
    theValue[0] = value.getUint8(0);
  }
  catch(error){
    log("Ouch! " + error);
  }
}

function onDisconnectButtonClick() {
  if (!bleDevice) {
    log('No device found');
    return;
  }
  if (bleDevice.gatt.connected) {
    log('Disconnecting');
    bleDevice.gatt.disconnect();
  } else {
    log('Device already disconnected');
  }
}

async function readCharacteristicValue(){
  let val = await ledCharacteristic.readValue();
  log("Q1=" + val.getUint8(1) + " Q0=" + val.getUint8(0));
}

async function toggleRed(){
    theValue[0] = (stateRed) ? 0x00 : 0x01;
    stateRed = !stateRed;
    await ledCharacteristic.writeValue(theValue);
}

async function toggleBlue(){
    theValue[1] = (stateBlue) ? 0x00 : 0x01;
    stateBlue = !stateBlue;
    await ledCharacteristic.writeValue(theValue);
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}