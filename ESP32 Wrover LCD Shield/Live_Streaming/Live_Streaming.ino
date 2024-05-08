/*
  Lonely Binary ESP32 Wrover Live Camera Web Hosting
  Demo Code
  
  Capture Live Photo : http://IP_ADDRESS/photo
  Stream Live Video : http://IP_ADDRESS/video

  Required Library:
  None

  Board:
  ESP32 Wrover Modeul
*/


#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Replace these with your WiFi network settings
const char *ssid = "SSID";           // WiFi network name
const char *password = "PASSWORD";  // WiFi password

#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 21
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 19
#define Y4_GPIO_NUM 18
#define Y3_GPIO_NUM 5
#define Y2_GPIO_NUM 4
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

WebServer server(80);
WiFiClient client;

const char HEADER[] = "HTTP/1.1 200 OK\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);

void handle_jpg_stream(void) {
  char buf[32];
  int s;

  WiFiClient client = server.client();

  client.write(HEADER, hdrLen);
  client.write(BOUNDARY, bdrLen);

  while (true) {
    if (!client.connected()) break;
    camera_fb_t *fb = esp_camera_fb_get();  // Get the frame buffer (replace with your capture method)
                                            // if (!fb) {
                                            //     Serial.println("Camera capture failed");
                                            //     break;
                                            // }
    s = fb->len;
    client.write(CTNTTYPE, cntLen);

    // what's this for?
    sprintf(buf, "%d\r\n\r\n", s);
    client.write(buf, strlen(buf));

    client.write(fb->buf, s);
    client.write(BOUNDARY, bdrLen);
    Serial.print("This Frame Length: ");
    Serial.println(fb->len);
    esp_camera_fb_return(fb);  // Return the frame buffer back to the camera driver
    delay(2);
  }
}

void handle_NotFound() {
  server.send(404, "text/plain", "Lonely Binary ESP32 Wrover LCD Shield Demo Code.");
}

void handleJpeg() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera Capture Failed");
  } else {

    Serial.print("This Frame Length: ");
    Serial.println(fb->len);
  }

  // Set the content type to image/jpeg
  server.sendHeader("Content-Type", "image/jpeg");
  server.sendHeader("Content-Length", String(fb->len));

  // Write the JPEG data to the client
  server.client().write((const char *)fb->buf, fb->len);

  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  


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
  config.xclk_freq_hz = 10000000;
  config.frame_size = FRAMESIZE_VGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // connect WIFI
  WiFi.begin(ssid, password);  // Connect to the network
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {  // Wait until the connection is established
    delay(500);
    Serial.print(".");
  }

  Serial.println('\n\n\n');
  Serial.println("Lonely Binary WebServer Photo/Video Demo Code");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  // Print the local IP address

  Serial.print("Photo : http://");
  Serial.print(WiFi.localIP()); 
  Serial.println("/photo");

  Serial.print("Video : http://");
  Serial.print(WiFi.localIP()); 
  Serial.println("/video");

  //Web Server Configuration
  server.on("/photo", handleJpeg);
  server.on("/video", handle_jpg_stream);
  server.onNotFound(handle_NotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}