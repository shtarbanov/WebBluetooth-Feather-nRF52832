/* The ascii character hex equivalencies are:
  ! = 0x21
  + = 0x2b
  - = 0x2d
  ^ = 0x5e
  ? = 0x3f

  TODO: Create an graphical representation based on the table.
*/
'use strict'

const controlServiceUUID = 'ffff1010-0000-1111-9999-0000000004aa';
const chrCommandUUID = 'ffff1010-0000-1111-9999-c100000004aa';
const chrHardwareStatusUUID = 'ffff1010-0000-1111-9999-c200000004aa';

let bleDevice;
let bleServer;
let controlService;
let chrCommand;
let chrHardwareStatus;
let valueArray = new Uint8Array(2);

window.onload = function(){
  document.querySelector('#connect').addEventListener('click', connect);
  document.querySelector('#disconnect').addEventListener('click', onDisconnectButtonClick);
  document.querySelector('#inflate').addEventListener('click', startInflation);
  document.querySelector('#vacuum').addEventListener('click', startVacuum);
  document.querySelector('#release').addEventListener('click', startRelease);
  document.querySelector('#stopAll').addEventListener('click', stopAllActions);
  document.querySelector('#stop').addEventListener('click', stopAction);
  document.querySelector('#read').addEventListener('click', getHardwareStatus);
};

async function connect() {  
  try{
    bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}],
          optionalServices: [controlServiceUUID]
        });
    bleServer = await bleDevice.gatt.connect();
    controlService = await bleServer.getPrimaryService(controlServiceUUID);
    chrCommand = await controlService.getCharacteristic(chrCommandUUID);  
    chrHardwareStatus = await controlService.getCharacteristic(chrHardwareStatusUUID);
    log("Connected");

    //subscribe to receive characteristic notification events
    await chrHardwareStatus.startNotifications();
    chrHardwareStatus.addEventListener('characteristicvaluechanged', event => { //an event is returned      
      let byte0 = event.target.value.getUint8(0);
      document.querySelector('#ports').innerHTML = (byte0 & 0x1f).toBinaryString(5);
      document.querySelector('#inlet').innerHTML = (byte0>>5 & 0x01);
      document.querySelector('#outlet').innerHTML = (byte0>>6 & 0x01);
      document.querySelector('#pump1').innerHTML = (byte0>>7 & 0x01);
      let byte1 = event.target.value.getUint8(1);
      document.querySelector('#pump2').innerHTML = (byte1 & 0x01);
      document.querySelector('#ledr').innerHTML = (byte1>>1 & 0x01);
      document.querySelector('#ledb').innerHTML = (byte1>>2 & 0x01);
      document.querySelector('#sensor').innerHTML = (byte1>>3 & 0x01);

    })

    //read the value of the chrHardwareStatus. This will trigger a notification to be sent.
    //This is critical, because it initilizes the values in our html table upon connect.
    let valueDataView = await chrHardwareStatus.readValue(); //returns a DataView.
  }

  catch(error){
    log("Ouch! " + error);
  }
}

//#############---CALLBACKS---###################
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
async function getHardwareStatus(){
  let val = await chrHardwareStatus.readValue(); //this returns a DataView
  log(val.getUint8(1).toBinaryString(8) + val.getUint8(0).toBinaryString(8));
}
async function startInflation(){
  valueArray[0] = 0x2b; //'+'
  valueArray[1] = getSelectedPorts();
  await chrCommand.writeValue(valueArray);
}
async function startVacuum(){
  valueArray[0] = 0x2d; //'-'
  valueArray[1] = getSelectedPorts();
  await chrCommand.writeValue(valueArray);
}
async function startRelease(){
  valueArray[0] = 0x5e; //'^'
  valueArray[1] = getSelectedPorts();
  await chrCommand.writeValue(valueArray);
}
async function stopAllActions(){
  valueArray[0]=0x21; //'!'
  valueArray[1]=0xff;
  await chrCommand.writeValue(valueArray);
}
async function stopAction(){
  valueArray[0]=0x21; //'!'
  valueArray[1]=getSelectedPorts();
  await chrCommand.writeValue(valueArray);
}
function getSelectedPorts(){
  let portsByte = 0x00;
  if(document.querySelector('#port1').checked) portsByte ^= 0x01; //0 0001
  if(document.querySelector('#port2').checked) portsByte ^= 0x02; //0 0010
  if(document.querySelector('#port3').checked) portsByte ^= 0x04; //0 0100
  if(document.querySelector('#port4').checked) portsByte ^= 0x08; //0 1000
  if(document.querySelector('#port5').checked) portsByte ^= 0x10; //1 0000
  return portsByte;
}
function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}