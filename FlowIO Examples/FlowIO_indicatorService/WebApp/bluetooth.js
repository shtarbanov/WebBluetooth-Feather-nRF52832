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

const indicatorServiceUUID = '0b0b0b0b-0b0b-0b0b-0b0b-00000000aa02';
const chrLedStatesUUID     = '0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa02';
const chrErrorUUID         = '0b0b0b0b-0b0b-0b0b-0b0b-c2000000aa02';

let bleDevice;
let bleServer;
let indicatorService;
let chrLedStates;
let chrError;
let valueArray;
let stateRed = true;
let stateBlue = true;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#red').addEventListener('click', toggleRed);
  document.querySelector('#blue').addEventListener('click', toggleBlue);
  document.querySelector('#read').addEventListener('click', readCharacteristicValue);
  document.querySelector('#readError').addEventListener('click', readError);
  document.querySelector('#clearError').addEventListener('click', clearError);
};

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [indicatorServiceUUID]
        });
    bleServer = await bleDevice.gatt.connect();
    indicatorService = await bleServer.getPrimaryService(indicatorServiceUUID);
    chrLedStates = await indicatorService.getCharacteristic(chrLedStatesUUID);
    chrError = await indicatorService.getCharacteristic(chrErrorUUID);
    
    log("Connected");

    //Subscribe to receive notifications from the error characteristic
    await chrError.startNotifications();
    chrError.addEventListener('characteristicvaluechanged', event => {
      log("Error Code: " + event.target.value.getUint8(0));
    })
    await chrError.readValue(); //This triggers a notification to be sent.

    //Subscribe to receive notifications from chrLedStates.
    await chrLedStates.startNotifications(); //This line causes red LED to turn off for some reason!?
    chrLedStates.addEventListener('characteristicvaluechanged', event => {
      log("Notification: B=" + event.target.value.getUint8(1) + " R=" + event.target.value.getUint8(0));
    })

    //##################
    //Now we want to read the current value of the characteristic and set our LED state
    //variables in JavaScript to match those from the characteristic.
    //##################

    let valueDataView = await chrLedStates.readValue(); //returns a DataView. This triggers a notification.
    //If you didn't know that object is of type "DataView" you could just do
    //console.log(valueDataView) and then you will see all infor about this variable in the console.

    //We now convert the DataView to TypedArray so we can use array notation to access the data.
    //https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/buffer
    valueArray = new Uint8Array(valueDataView.buffer);
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
  let val = await chrLedStates.readValue(); //this returns a DataView
  //Whenever we request to read the value, this will trigger in JavaScript the
  //'characteristicvaluechanged' notification. (This will NOT cause the FlowIO
  //itself to emit a notification! Or maybe it will under the hood, but not
  //anywhere in my code at least!) Hence, we don't need to log the value of 'val'
  //here because we will get it in the event listener function above.
  //log("Q1=" + val.getUint8(1) + " Q0=" + val.getUint8(0));
}
async function toggleRed(){
    valueArray[0] = (stateRed) ? 0x00 : 0x01;
    stateRed = !stateRed;
    await chrLedStates.writeValue(valueArray);
}
async function toggleBlue(){
    valueArray[1] = (stateBlue) ? 0x00 : 0x01;
    stateBlue = !stateBlue;
    await chrLedStates.writeValue(valueArray);
}

async function readError(){
  await chrError.readValue(); //thi will trigger our notification listener.
}

async function clearError(){
  let zeroArray = new Uint8Array([0]);
  await chrError.writeValue(zeroArray);
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}