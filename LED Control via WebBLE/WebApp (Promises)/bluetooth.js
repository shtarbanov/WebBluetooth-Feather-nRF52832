/*
  This example shows how to control the state of each of the two LEDs on the nrf52832 Feather
  board. The microcontorller has been set up so that is has one custom service with a single
  characteristic that holds a 2-byte value. The two bytes in the characteristic correspond to
  the state of the two LEDs on the board. The characteristic has a read and write persmisions.
    In this JavaScript code, we are connecting to the device and reading the current value of
  the characteristic. Then we are toggling one of the two bytes depending on which button the
  user presses on the screen, which in turn causes the corresponding LED to toggle as well. 

  This example uses the Promises approach.
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

function connect() {
  navigator.bluetooth.requestDevice({
    filters: [{namePrefix: 'nrf52'}],
    optionalServices: [serviceUUID]
  })
  .then(device => {
    bleDevice = device;
    return device.gatt.connect();
  })
  .then(server => {
    bleServer = server;
    log('Got bleServer');
    return server.getPrimaryService(serviceUUID);
  })
  .then(service => {
    log('Got ledService');
    ledService = service;
  })
  .then(() => {
    return ledService.getCharacteristic(ledCharacteristicUUID);
  })
  .then( characteristic => {
    ledCharacteristic = characteristic;
    return characteristic.readValue(); //this returns a DataView and puts it inside the .then() method.
  })
  .then(valueDataView => {
    valueArray = new Uint8Array(valueDataView.buffer);
    log("Connected using Promises!\nThe value is: \nQ1=" + valueArray[1] + " Q0=" + valueArray[0]);
    stateBlue = valueArray[1];
    stateRed = valueArray[0];
  })
  .catch(error => {
    log("Ouch! " + error);
  });
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

function readCharacteristicValue(){
  ledCharacteristic.readValue().then(val =>log("Q1=" + val.getUint8(1) + " Q0=" + val.getUint8(0)));
  //This is a promise, which returns a value of type DataView, which gets placed inside the .then() method.
  //Hence, our variavble "val" is of type "DataView". The syntax inside of .then() means: 
  //Take the val returned by the promise, and then => log that val as indicated in the log() function.
}

function toggleRed(){
    valueArray[0] = (stateRed) ? 0x00 : 0x01;
    stateRed = !stateRed;
    return ledCharacteristic.writeValue(valueArray);
}

function toggleBlue(){
    valueArray[1] = (stateBlue) ? 0x00 : 0x01;
    stateBlue = !stateBlue;
    return ledCharacteristic.writeValue(valueArray);
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}