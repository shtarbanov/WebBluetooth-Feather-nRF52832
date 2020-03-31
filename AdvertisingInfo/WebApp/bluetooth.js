window.onload = function(){
  document.querySelector('#discover').addEventListener('click', discover);
};

async function discover() {  
  try{
    let bleDevice = await navigator.bluetooth.requestDevice({
          filters: [{namePrefix: 'nrf52'}]
    });
    console.log(bleDevice);
    log("id: " + bleDevice.id);
    log("name: " + bleDevice.name);
    log("gatt.connected: " + bleDevice.gatt.connected);
  }
  catch(error){
    log("Ouch! " + error);
  }
}

function log(text) {
    console.log(text);
    document.querySelector('#log').textContent += text + '\n';
}