#include <esp_camera.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>

// WiFi credentials
const char* ssid = "HUAWEI-tDe4";
const char* password = "48575443C607C69C";

// Web server and WebSocket server
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// AI Thinker ESP32-CAM pin configuration
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Store the last command to manage stop when the connection is lost
String lastCommand = "";

// Global variables for stream control
bool streaming = false;
unsigned long lastFrameTime = 0;
const unsigned long frameInterval = 100; // Time between frames in milliseconds (adjust as needed)
WiFiClient client; // Client connection for the stream

// Function to send a single frame
void sendFrame() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Proper multipart frame format
  client.write("--frame\r\n");
  client.write("Content-Type: image/jpeg\r\n\r\n");
  client.write((const char*)fb->buf, fb->len);
  client.write("\r\n");

  esp_camera_fb_return(fb);
}

void startCameraServer() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // Set up the camera endpoint for single image capture
  server.on("/camera", HTTP_GET, []() {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      server.send(503, "text/plain", "Camera capture failed");
      return;
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  // Stream endpoint: set streaming flag when requested
  server.on("/stream", HTTP_GET, []() {
    streaming = true;
    client = server.client(); // Store the current client
    client.write("HTTP/1.1 200 OK\r\n");
    client.write("Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n");
  });

  // Start the server
  server.begin();
  Serial.println("Camera server started");
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String command = String((char*)payload);

    // Send command to Arduino Nano via Serial
    Serial.println(command);

    // Store the last command to handle connection checks
    lastCommand = command;
  }
}

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  // Initialize camera
  startCameraServer();  // Make sure the camera server is started

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  // Handle WebSocket and HTTP server
  webSocket.loop();
  server.handleClient();

  // Handle streaming if enabled
  if (streaming && client.connected() && millis() - lastFrameTime >= frameInterval) {
    sendFrame();
    lastFrameTime = millis();
  }

  // Connection check: if WebSocket connection is lost, send a stop command
  if (lastCommand != "" && !webSocket.connectedClients()) {
    Serial.println("S"); // Stop command
    lastCommand = "";
  }
}
