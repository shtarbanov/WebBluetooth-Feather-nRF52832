/* 
  This example builds on the basic LED control example, and shows how to connect to 
  2 bluetooth peripheral devices simultaneously. In the .js code, we have simply
  made the variables into arrays, and made the functions take an argument i. In this
  way, we can use this same code for any number of devices we wish to connect to,
  simply by modifying the definition of the .onload function.
*/
'use strict'

const serviceUUID = 'a3941db0-a97c-4cf1-943f-a25ff9ba40cd';
const ledCharacteristicUUID = '5b8c0ab6-a058-4684-b2b6-4a0a692e2d45';

let bleDevice = [];
let bleServer = [];
let bleService = [];
let ledCharacteristic = [];
let valueArray =[]; //this will be an array holding an array.
let stateRed = [];
let stateBlue = [];

window.onload = function(){
  //since we can't pass both a function and argument in the second parameter of the 
  //event listener, we are creating an anonymous functions which will then execute
  //the code connect(#).
  document.querySelector('#connect').addEventListener('click', function(){connect(1)});
  document.querySelector('#disconnect').addEventListener('click', function(){onDisconnectButtonClick(1)});
  document.querySelector('#red').addEventListener('click', function(){toggleRed(1)});
  document.querySelector('#blue').addEventListener('click', function(){toggleBlue(1)});
  document.querySelector('#read').addEventListener('click', function(){readCharacteristicValue(1)});

  document.querySelector('#connect2').addEventListener('click', function(){connect(2)});
  document.querySelector('#disconnect2').addEventListener('click', function(){onDisconnectButtonClick(2)});
  document.querySelector('#red2').addEventListener('click', function(){toggleRed(2)});
  document.querySelector('#blue2').addEventListener('click', function(){toggleBlue(2)});
  document.querySelector('#read2').addEventListener('click', function(){readCharacteristicValue(2)});

};

async function connect(i) { //i is the device Number 
  try{
    bleDevice[i] = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [serviceUUID]
        });
    bleServer[i] = await bleDevice[i].gatt.connect();
    bleService[i] = await bleServer[i].getPrimaryService(serviceUUID);
    ledCharacteristic[i] = await bleService[i].getCharacteristic(ledCharacteristicUUID);
    
    //##################
    //Now we want to read the current value of the characteristic and set our LEDstate
    //variables in JavaScript to match those from the characteristic.
    //##################

    let valueDataView = await ledCharacteristic[i].readValue(); //returns a DataView.
    //If you didn't know that object is of type "DataView" you could just do
    //console.log(valueDataView) and then you will see all infor about this variable in the console.

    //We now convert the DataView to TypedArray so we can use array notation to access the data.
    //https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/buffer
    valueArray[i] = new Uint8Array(valueDataView.buffer);
    log(i, "Connected! The value is: \nQ1=" + valueArray[i][1] + " Q0=" + valueArray[i][0]);
    stateBlue[i] = valueArray[i][1];
    stateRed[i] = valueArray[i][0];
  }
  catch(error){
    log(i, "Ouch! " + error);
  }
}
function onDisconnectButtonClick(i) {
  if (!bleDevice[i]) {
    log(i, 'No device found');
  }
  else if (bleDevice[i].gatt.connected) {
    log(i, 'Disconnecting');
    bleDevice[i].gatt.disconnect();
  } 
  else {
    log(i, 'Device already disconnected');
  }
}
async function readCharacteristicValue(i){
  let val = await ledCharacteristic[i].readValue(); //this returns a DataView
  log(i, "Q1=" + val.getUint8(1) + " Q0=" + val.getUint8(0));
}
async function toggleRed(i){
    valueArray[i][0] = (stateRed[i]) ? 0x00 : 0x01;
    stateRed[i] = !stateRed[i];
    await ledCharacteristic[i].writeValue(valueArray[i]);
}
async function toggleBlue(i){
    valueArray[i][1] = (stateBlue[i]) ? 0x00 : 0x01;
    stateBlue[i] = !stateBlue[i];
    await ledCharacteristic[i].writeValue(valueArray[i]);
}
function log(i, text) {
    console.log(text);
    if(i==1) document.querySelector('#log').textContent += text + '\n';
    if(i==2) document.querySelector('#log2').textContent += text + '\n';
}