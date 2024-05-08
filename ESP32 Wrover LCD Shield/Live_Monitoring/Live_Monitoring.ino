/*
  Lonely Binary ESP32 Wrover LCD Shield
  Camera LCD Demo Code

  In this demonstration, we will show you how to capture live video footage from a camera, process the video frames in JPEG format, and subsequently display these frames on an LCD screen. This process involves several key steps, which we'll outline in the demo code:

  Key Components:
  Camera Module: This device captures the live video. For the purpose of this demo, it outputs frames in JPEG format, a common compressed image format that helps to manage the large amount of data involved in video capture.
  JPEG Decoding: Since the camera outputs frames in JPEG format, we'll include a step to decode these JPEG frames back into raw image data that can be used for display.
  LCD Display: This is the output device where the decoded video frames are displayed, allowing you to see the live video feed.

  Required Library:
  TFT_eSPI by Bodmer
  TJpg_Decoder by Bodmer

  Board:
  ESP32 Wrover Module
*/

#include "esp_camera.h"
#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>

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

TFT_eSPI tft = TFT_eSPI();

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);

  return 1;
}

void setup() {
  Serial.begin(115200);

  Serial.println("Lonely Binary ESP32 Wrover LCD Shield Demo Code");

  tft.init();
  tft.setRotation(0);
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tft_output);

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
  config.frame_size = FRAMESIZE_HQVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_DRAM;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_hmirror(s, 1);
  s->set_vflip(s, 0);
}

void loop() {


  camera_fb_t *fb = NULL;

  /*
  esp_camera_fb_get tells it to capture a image and store it in memory, 
  the pointer "fb" then points to where this data is
  */
  fb = esp_camera_fb_get();
  if (fb) {
    Serial.println(fb->len);
    Serial.println(fb->width);
    Serial.println(fb->height);
    Serial.println(fb->format);
    Serial.println(fb->timestamp.tv_sec);
    Serial.println(fb->timestamp.tv_usec);

    // Get the width and height in pixels of the jpeg if you wish
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, fb->buf, fb->len);
    Serial.print("Width = ");
    Serial.print(w);
    Serial.print(", height = ");
    Serial.println(h);

    // Draw the image, top left at 0,0
    TJpgDec.drawJpg(-60, -8, fb->buf, fb->len);
  }

  esp_camera_fb_return(fb);
  fb = NULL;
}