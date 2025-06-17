#include "esp_camera.h"
#include <WiFi.h>

// Define your Wi-Fi credentials
const char* ssid = "Coconutpie";        // Change this to your WiFi SSID
const char* password = "sleepypiratecalculator"; // Change this to your WiFi Password

// Camera model settings for AI Thinker ESP32-CAM
#define PWDN_GPIO_NUM    32
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      21
#define Y4_GPIO_NUM      19
#define Y3_GPIO_NUM      18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

// Initialize retry counters
int maxWifiRetries = 10;
int wifiRetries = 0;

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  // Retry Wi-Fi connection multiple times
  while (WiFi.status() != WL_CONNECTED && wifiRetries < maxWifiRetries) {
    delay(1000);
    Serial.print(".");
    wifiRetries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi. Rebooting...");
    ESP.restart();  // Restart if unable to connect
  }
}

// Function to initialize the camera
void initCamera() {
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Frame size: you can change this to FRAMESIZE_SVGA or other options
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 12; // Lower value = better quality
  config.fb_count = 1;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  } else {
    Serial.println("Camera initialized successfully");
  }
}

// Function to capture an image
void captureAndSendImage() {
  // Capture image
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Display the size of the captured image
  Serial.print("Image captured, size: ");
  Serial.println(fb->len);

  // You can add image sending code here, like sending it over UDP, FTP, or Firebase

  // Free memory used by frame buffer
  esp_camera_fb_return(fb);
}

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  delay(2000); // Give time for the serial connection to stabilize

  // Initialize the camera
  initCamera();

  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  // Capture and send images every minute (60000 milliseconds)
  captureAndSendImage();
  delay(60000); // Wait for 1 minute before capturing another image
}
