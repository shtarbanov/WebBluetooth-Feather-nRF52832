/*This example allows you to read and receive a notifications from a BLE device
that reports a floating point value (4-byte). 

The default value reported has many digits after the decimal point, which are 
not true, thus we are trunkating the value to just 5 digits. 

*/
'use strict'

let bleDevice;
let bleServer;
let floatNumberService;
let floatValueCharacteristic;
let requestNewNumberCharacteristic;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#read').addEventListener('click', readCharacteristicValue);
  document.querySelector('#generate').addEventListener('click', requestNewNumber);
  document.querySelector('#continuouson').addEventListener('click', startNotifications);
  document.querySelector('#continuousoff').addEventListener('click', stopNotifications);
};

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [0xae6f]
    });
    bleServer = await bleDevice.gatt.connect();
    floatNumberService = await bleServer.getPrimaryService(0xae6f);
    floatValueCharacteristic = await floatNumberService.getCharacteristic(0x2947);
    requestNewNumberCharacteristic = await floatNumberService.getCharacteristic(0x2948);

    //This is reading the value of the characteristic and displaying it
    let valueDataView = await floatValueCharacteristic.readValue(); //returns a DataView.
    //We must convert to a float32Array to get the endianness correctly.
        //let floatArray = new Float32Array(valueDataView.buffer);
        //log("Connected!\n" + floatArray[0] + "%");
    //Or alternatively, we can set the second parameter in the .getFloat32() to 'true'
    log("Connected!\n" + valueDataView.getFloat32(0,true));


    //This is responding to notifications sent by the peripheral of changes to the value.
    await floatValueCharacteristic.startNotifications();
    floatValueCharacteristic.addEventListener('characteristicvaluechanged', event => { //an event is returned 
      let floatValue = event.target.value.getFloat32(0,true); 
      floatValue = floatValue.toFixed(4); //rounding to 4 decimal places. 
      log(floatValue);
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
  let val = await floatValueCharacteristic.readValue(); //this returns a DataView
  //It is unnecessary to log the value of val, because this triggers our event listener
  //in the 'connect' function. Hence we commend out the line below.
  //log(val.getUint8(0) + "%");
}

async function requestNewNumber(){
    let value = new Uint8Array([1]);
    await requestNewNumberCharacteristic.writeValue(value);
}

async function startNotifications(){
    let value = new Uint8Array([2]);
    await requestNewNumberCharacteristic.writeValue(value);  
}
async function stopNotifications(){
    let value = new Uint8Array([0]);
    await requestNewNumberCharacteristic.writeValue(value);
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}