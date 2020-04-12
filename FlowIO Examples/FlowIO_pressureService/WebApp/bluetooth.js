/*This example allows you to read and receive a notifications from a BLE device
that reports a floating point value (4-byte).

The default value reported has many digits after the decimal point, which are 
not true, thus we are trunkating the value to just 5 digits. 

*/
'use strict'

let bleDevice;
let bleServer;
let pressureService;
let chrPressureValue;
let chrPressureRequest;
let chrMaxPressureLimits;
let chrMinPressureLimits;

let maxDisplay;
let maxSlider;
let minDisplay;
let minSlider;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#read').addEventListener('click', readCharacteristicValue);
  document.querySelector('#generate').addEventListener('click', requestNewNumber);
  
  document.querySelector('#getmax').addEventListener('click', getMaxLimit);  
  document.querySelector('#getmin').addEventListener('click', getMinLimit);
  document.querySelector('#setminmax').addEventListener('click', setMinMax);
};

function getMaxValue() {
  maxDisplay.innerHTML = this.value;
  return this.value;
}
function getMinValue() {
  minDisplay.innerHTML = this.value;
  return this.value;
}

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: ['ffff1010-0000-1111-9999-0000000005aa']
    });
    bleServer = await bleDevice.gatt.connect();
    pressureService = await bleServer.getPrimaryService('ffff1010-0000-1111-9999-0000000005aa');
    chrPressureValue = await pressureService.getCharacteristic('ffff1010-0000-1111-9999-c100000005aa');
    chrPressureRequest = await pressureService.getCharacteristic('ffff1010-0000-1111-9999-c200000005aa');
    chrMaxPressureLimits = await pressureService.getCharacteristic('ffff1010-0000-1111-9999-c300000005aa');
    chrMinPressureLimits = await pressureService.getCharacteristic('ffff1010-0000-1111-9999-c400000005aa');

    //This is reading the value of the characteristic and displaying it
    let valueDataView = await chrPressureValue.readValue(); //returns a DataView.
    //We must convert to a float32Array to get the endianness correctly.
        //let floatArray = new Float32Array(valueDataView.buffer);
        //log("Connected!\n" + floatArray[0] + "%");
    //Or alternatively, we can set the second parameter in the .getFloat32() to 'true'
    log("Connected!\nP = " + valueDataView.getFloat32(0,true).toFixed(3));


    //This is responding to notifications sent by the peripheral of changes to the value.
    await chrPressureValue.startNotifications();
    chrPressureValue.addEventListener('characteristicvaluechanged', event => { //an event is returned 
      let floatValue = event.target.value.getFloat32(0,true); 
      floatValue = floatValue.toFixed(3); //rounding to 4 decimal places. 
      log("P = " + floatValue);
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
  let val = await chrPressureValue.readValue(); //this returns a DataView
  //It is unnecessary to log the value of val, because this triggers our event listener
  //in the 'connect' function. Hence we commend out the line below.
  //log(val.getUint8(0) + "%");
}


async function getMaxLimit(){
  let val = await chrMaxPressureLimits.readValue(); //returns a dataView
  let floatValue = val.getFloat32(0,true); 
  log(floatValue.toFixed(2));
}

async function getMinLimit(){
  let val = await chrMinPressureLimits.readValue(); //returns a dataView
  let floatValue = val.getFloat32(0,true); 
  log(floatValue.toFixed(2));
}

async function setMinMax(){
  let minimum = $("#slider-range").slider("values",0);
  let maximum = $("#slider-range").slider("values",1);
  let min = new Float32Array([minimum]);
  let max = new Float32Array([maximum]);
  await chrMinPressureLimits.writeValue(min);
  await chrMaxPressureLimits.writeValue(max);
  log(min[0].toFixed(2) + ' - ' + max[0].toFixed(2));
}

async function requestNewNumber(){
    let value = new Uint8Array([1]);
    await chrPressureRequest.writeValue(value);
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}

$(function updateSlider(){
    $("#slider-range").slider({
      range: true, min: 0, max: 500, values: [75, 300], step: 0.01, orientation: "horizontal",
      slide: (event, ui) => $("#label").val(ui.values[0] + " - " + ui.values[1])
    });
    $("#label").val($("#slider-range").slider("values", 0) + " - " + $("#slider-range").slider("values", 1));
  });