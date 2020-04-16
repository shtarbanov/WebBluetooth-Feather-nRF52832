/* This example powers off the flowio device
*/
'use strict'

const powerOffServiceUUIDString           = '0b0b0b0b-0b0b-0b0b-0b0b-00000000aa01';
const chrPowerOffNowUUIDString            = '0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa01';
const chrPowerOffWhenInactiveForUUIDString= '0b0b0b0b-0b0b-0b0b-0b0b-c2000000aa01';
const chrPowerOffRemainingTimeUUIDString  = '0b0b0b0b-0b0b-0b0b-0b0b-c3000000aa01';

let bleDevice;
let bleServer;

let powerOffService;
  let chrPowerOffNow;
  let chrPowerOffWhenInactiveFor;
  let chrPowerOffRemainingTime;

let inactiveSlider;
let inactiveValue;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#off').addEventListener('click', powerOff);
  document.getElementById("inactiveSlider").addEventListener('input', setPowerOffTimer);

  inactiveValue = document.getElementById("inactiveValue");
};

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [powerOffServiceUUIDString]
        });
    bleServer = await bleDevice.gatt.connect();
    powerOffService = await bleServer.getPrimaryService(powerOffServiceUUIDString);
    chrPowerOffNow = await powerOffService.getCharacteristic(chrPowerOffNowUUIDString);
    chrPowerOffWhenInactiveFor = await powerOffService.getCharacteristic(chrPowerOffWhenInactiveForUUIDString);
    //Change the embedded code so that this always hold the correct value in minutes.
    chrPowerOffRemainingTime = await powerOffService.getCharacteristic(chrPowerOffRemainingTimeUUIDString);

    log("Connected");

    //Subscribe to receive the notifications for remaining time till off:
    await chrPowerOffRemainingTime.startNotifications();
    chrPowerOffRemainingTime.addEventListener('characteristicvaluechanged', event => {
      log('Power Off In: ' + event.target.value.getUint8(0));
    })
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
async function powerOff(){
  log('Power off');
  let poweroff = new Uint8Array([1]);
  await chrPowerOffNow.writeValue(poweroff);
}

//this is an event function and it receives automatically
//an 'event'. The following are equivalent 'this' <-> 'event.target'
async function setPowerOffTimer(){
  if(event.target.value==0){
    inactiveValue.innerHTML = "Disabled.";
  } 
  else{
    inactiveValue.innerHTML = event.target.value + " minutes.";
  }
  let powerofftime = new Uint8Array([event.target.value]);
  await chrPowerOffWhenInactiveFor.writeValue(powerofftime);
};

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}