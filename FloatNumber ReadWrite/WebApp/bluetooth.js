/*This example allows you to read and receive a notifications from a BLE device
that reports a floating point value (4-byte).

The default value reported has many digits after the decimal point, which are 
not true, thus we are trunkating the value to just 5 digits. 

*/
'use strict'

let bleDevice;
let bleServer;
let floatService;
let chrFloatLimits;

let valDisplay;
let theSlider;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#set').addEventListener('click', setFloat);
  document.querySelector('#get').addEventListener('click', getFloat);
  theSlider = document.getElementById("slider");
  theSlider.addEventListener('input', getSliderValue);
  valDisplay = document.getElementById("value");

};

function getSliderValue() {
  valDisplay.innerHTML = this.value;
  return this.value;
}

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: ['ffff1010-0000-1111-9999-0000000005aa']
    });
    bleServer = await bleDevice.gatt.connect();
    floatService = await bleServer.getPrimaryService('ffff1010-0000-1111-9999-0000000005aa');
    chrFloatLimits = await floatService.getCharacteristic('ffff1010-0000-1111-9999-c100000005aa');
    log("Connected!");
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

async function setFloat(){
  let v = parseFloat(theSlider.value);
  log(v);
  let value = new Float32Array([v]);
  await chrFloatLimits.writeValue(value);
}
async function getFloat(){
  let val = await chrFloatLimits.readValue(); //returns a dataView
  let floatValue = val.getFloat32(0,true); 
  log(floatValue.toFixed(4));
}

async function requestNewNumber(){
    let value = new Uint8Array([1]);
    await chrPressureRequest.writeValue(value);
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}