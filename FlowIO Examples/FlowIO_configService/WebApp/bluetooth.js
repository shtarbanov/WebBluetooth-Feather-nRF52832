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

const configServiceUUID = 'ffff1010-0000-1111-9999-0000000003aa';
const chrConfigUUID = 'ffff1010-0000-1111-9999-c100000003aa';

let bleDevice;
let bleServer;
let configService;
let chrConfig;

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#read').addEventListener('click', getConfiguration);
  document.querySelector('#select').addEventListener('change', setConfiguration);
};

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [configServiceUUID]
        });
    bleServer = await bleDevice.gatt.connect();
    configService = await bleServer.getPrimaryService(configServiceUUID);
    chrConfig = await configService.getCharacteristic(chrConfigUUID);
    
    log("Connected");
    getConfiguration();
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
async function getConfiguration(){
  let config = await chrConfig.readValue(); //this returns a DataView
  let configNumber = config.getUint8(0)
  if(configNumber==0) log('GENERAL');
  else if(configNumber==1) log('INFLATION_SERIES');
  else if(configNumber==2) log('INFLATION_PARALLEL');
  else if(configNumber==3) log('VACUUM_SERIES');
  else if(configNumber==4) log('VACUUM_PARALLEL');
}
async function setConfiguration(){
  //1. If the user selects the empty element, I want to preserve the configuration the same.
  //2. I want the console window to display the name of the configuration not the value of it.
  let val = document.getElementById("select").value

  if(val!=''){
    log('Changed to: ')
    let valArray = new Uint8Array([val]); 
    await chrConfig.writeValue(valArray);
  } 

  if(val==0) log('GENERAL');
  else if(val==1) log('INFLATION_SERIES');
  else if(val==2) log('INFLATION_PARALLEL');
  else if(val==3) log('VACUUM_SERIES');
  else if(val==4) log('VACUUM_PARALLEL');
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}