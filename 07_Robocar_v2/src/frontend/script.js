const localIP = '192.168.100.120'
const cameraStreamUrl = `http://${localIP}/stream`;

let websocket;
const wsUrl = `ws://${localIP}:81/`;
const wsStatuses = {
  succeed: 'succeed',
  connecting: 'connecting',
  failed: 'failed',
}

function init() {
  updateWebSocketStatus(wsStatuses.connecting);
  initWebSocket();
}

function initWebSocket() {
  websocket = new WebSocket(wsUrl);
  websocket.onopen = () => {
    initStreamAndControl();
    updateWebSocketStatus(wsStatuses.succeed);
  };
  websocket.onclose = () => {
    updateWebSocketStatus(wsStatuses.failed);
    // Stop motors if connection lost
    sendCommand('S');
  };
}

function sendCommand(command) {
  if (websocket && websocket.readyState === WebSocket.OPEN) {
    websocket.send(command);
    console.log(command)
  }
}

function initStreamAndControl() {
  removeLoading();
  addStreamAndControlLayout();
  updateWebSocketStatus();
}

function addStreamAndControlLayout() {
  /* 
  * This is commands to communicate with arduino nano and it hardware by i2c interface 
  * F - forward
  * B - backward
  * L - left
  * R - right
  * S1 - servo one
  * S2 - servo two
  */
  const controlAndStreamLayout = `
  <img class="camera" src="${cameraStreamUrl}" width="320" height="240">
  <br>
  <button onmousedown="sendCommand('F')" onmouseup="sendCommand('S')">Forward</button>
  <div>
    <button onmousedown="sendCommand('L')" onmouseup="sendCommand('S')">Left</button>
    <button onmousedown="sendCommand('R')" onmouseup="sendCommand('S')">Right</button>
  </div>
  <button onmousedown="sendCommand('B')" onmouseup="sendCommand('S')">Backward</button>
  <br>
  <h2>Servo Control</h2>
  <input type='range' min='0' max='180' id='servo1' oninput="sendCommand(getActualServoValue(1))">Servo 1<br>
  <input type='range' min='0' max='180' id='servo2' oninput="sendCommand(getActualServoValue(2))">Servo 2<br>
  `
  const controlContainer = document.createElement('div');
  controlContainer.innerHTML = controlAndStreamLayout;
  document.body.appendChild(controlContainer)
}

function getActualServoValue(servoNumber) {
  const servoValues = {
    1: document.querySelector('#servo1').value,
    2: document.querySelector('#servo2').value,
  }
  return `S${servoNumber}:${servoValues[servoNumber]}`
}

function removeLoading() {
  const loadingLayout = document.querySelector('.loading');
  loadingLayout.remove();
}

function updateWebSocketStatus(status) {
  const statusElement = document.querySelector('.status');
  statusElement.classList.remove(...Object.values(wsStatuses))
  statusElement.classList.add(status)
  statusElement.innerHTML = `Websocket: ${status}`;
}

init();