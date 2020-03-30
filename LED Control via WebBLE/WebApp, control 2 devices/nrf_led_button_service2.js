/* 
  This example shows how to control the state of each of the two LEDs on the nrf52832 Feather
  board. The microcontorller has been set up so that is has one custom service with a single
  characteristic that holds a 2-byte value. The two bytes in the characteristic correspond to
  the state of the two LEDs on the board. The characteristic has a read and write persmisions.
    In this JavaScript code, we are connecting to the device and reading the current value of
  the characteristic. Then we are toggling one of the two bytes depending on which button the
  user presses on the screen, which in turn causes the corresponding LED to toggle as well. 
*/
'use strict'

const serviceUUID2 = 'a3941db0-a97c-4cf1-943f-a25ff9ba40cd';
const ledCharacteristicUUID2 = '5b8c0ab6-a058-4684-b2b6-4a0a692e2d45';

let bleDevice2;
let bleServer2;
let bleService2;
let ledCharacteristic2;
let valueArray2;
let stateRed2 = true;
let stateBlue2 = true;

async function connect2() {  
  try{
    bleDevice2 = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [serviceUUID]
        });
    bleServer2 = await bleDevice2.gatt.connect();
    bleService2 = await bleServer2.getPrimaryService(serviceUUID2);
    ledCharacteristic2 = await bleService2.getCharacteristic(ledCharacteristicUUID2);
    
    //##################
    //Now we want to read the current value of the characteristic and set our LEDstate
    //variables in JavaScript to match those from the characteristic.
    //##################

    let valueDataView2 = await ledCharacteristic2.readValue(); //returns a DataView.
    //If you didn't know that object is of type "DataView" you could just do
    //console.log(valueDataView) and then you will see all infor about this variable in the console.

    //We now convert the DataView to TypedArray so we can use array notation to access the data.
    //https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/buffer
    valueArray2 = new Uint8Array(valueDataView2.buffer);
    log2("Connected! The value is: \nQ1=" + valueArray2[1] + " Q0=" + valueArray2[0]);
    stateBlue2 = valueArray2[1];
    stateRed2 = valueArray2[0];
  }
  catch(error){
    log2("Ouch! " + error);
  }
}
function onDisconnectButtonClick2() {
  if (!bleDevice2) {
    log2('No device found');
  }
  else if (bleDevice2.gatt.connected) {
    log2('Disconnecting');
    bleDevice2.gatt.disconnect();
  } 
  else {
    log2('Device already disconnected');
  }
}
async function readCharacteristicValue2(){
  let val2 = await ledCharacteristic2.readValue(); //this returns a DataView
  log2("Q1=" + val2.getUint8(1) + " Q0=" + val2.getUint8(0));
}
async function toggleRed2(){
    valueArray2[0] = (stateRed2) ? 0x00 : 0x01;
    stateRed2 = !stateRed2;
    await ledCharacteristic2.writeValue(valueArray2);
}
async function toggleBlue2(){
    valueArray2[1] = (stateBlue2) ? 0x00 : 0x01;
    stateBlue2 = !stateBlue2;
    await ledCharacteristic2.writeValue(valueArray2);
}
function log2(text) {
    console.log(text);
    document.querySelector('#log2').textContent += text + '\n';
}