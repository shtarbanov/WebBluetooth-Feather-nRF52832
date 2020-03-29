//TODO: Convert the promise to an asynchronous function
//TODO: Have the device report the battery value, then read and display the value here.
  //can you make the value be updated only if the battery level changes.
  //On the embedded device, implement the battery service manually. Read more about 
  //BLE to understand how to handle notify option, and more about permissions.


//TODO: Design a diagram showing the switchihng happening internally in the FlowIO. The diagram
  //should also show the power consumption, which would increase when more valves are opened. You
  //want to be able to just click on different parts of the diagram itself to change the behavior.
  //First you can try doing it in powerpoint, then in p5.js.
//Disconnect does not work on my Linux machine.
'use strict'

const serviceUUID = 'a3941db0-a97c-4cf1-943f-a25ff9ba40cd';
const ledCharacteristicUUID = '5b8c0ab6-a058-4684-b2b6-4a0a692e2d45';

let bluetoothDevice;
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

function connect() {
  log('Requesting Bluetooth Device...');
  navigator.bluetooth.requestDevice({
    filters: [{namePrefix: 'Nordic'}],
    optionalServices: [serviceUUID]
  })
  .then(device => {
    bluetoothDevice = device;
    return device.gatt.connect();
  })
  .then(server => {
    bleServer = server;
    log('Got bleServer');
    return server.getPrimaryService(serviceUUID);
  })
  .then(service => {
    log('Got bleService');
    bleService = service;
  })
  .then(() => {
    return bleService.getCharacteristic(ledCharacteristicUUID);
  })
  .then( characteristic => {
    ledCharacteristic = characteristic;
    log('Got the LED characteristic');
    return characteristic.readValue();
  })
  .then(value => {
    log("The value is: Q1=" + value.getUint8(1) + " Q0=" + value.getUint8(0));
    stateBlue = value.getUint8(1);
    stateRed = value.getUint8(0);
    theValue[1] = value.getUint8(1);
    theValue[0] = value.getUint8(0);
  })
  .catch(error => {
    log('> connect ' + error);
  });
}

function onDisconnectButtonClick() {
  if (!bluetoothDevice) {
    return;
  }
  log('Disconnecting from Bluetooth Device...');
  if (bluetoothDevice.gatt.connected) {
    bluetoothDevice.gatt.disconnect();
  } else {
    log('> Bluetooth Device is already disconnected');
  }
}

//In these characteristics, I want to first read the value, so that when 
//I write to the array, I can keep one of the bytes unchanged.

function readCharacteristicValue(){
  ledCharacteristic.readValue().then(val =>log("Q1=" + val.getUint8(1) + " Q0=" + val.getUint8(0)));
  //This is a promise. It returns a resolve value which is placed in the .then() method, and which
  //I am calling "val". The syntax inside of .then() means: "Take the value returned by the promise,
  //which we name "resolveVal", and then => do the following thing with that revolveVal.
}

function toggleRed(){
    theValue[0] = (stateRed) ? 0x00 : 0x01;
    stateRed = !stateRed;
    return ledCharacteristic.writeValue(theValue);
}

function toggleBlue(){
    theValue[1] = (stateBlue) ? 0x00 : 0x01;
    stateBlue = !stateBlue;
    return ledCharacteristic.writeValue(theValue);
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}