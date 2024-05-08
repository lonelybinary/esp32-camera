/*
  Lonely Binary ESP32 Wrover LCD Shield
  Demo Code
*/
#include "esp_camera.h"
#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>
#include "FS.h"
#include "SD.h"

#define BTN 0
#define SD_CS 33
#define TFT_CS 15

TFT_eSPI tft = TFT_eSPI();

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

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  if (y >= tft.height())
    return 0;

  tft.pushImage(x, y, w, h, bitmap);

  return 1;
}


void sdOn() {
  digitalWrite(SD_CS, LOW);
  digitalWrite(TFT_CS, HIGH);
}

void lcdOn() {
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TFT_CS, LOW);
}

void allOff() {
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TFT_CS, HIGH);
}
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Lonely Binary ESP32 Wrover LCD Shield Demo Code");

  pinMode(BTN, INPUT_PULLUP);
  pinMode(SD_CS, OUTPUT);
  pinMode(TFT_CS, OUTPUT);

  allOff();

  tft.init();
  tft.setRotation(0);

  if (!SD.begin(SD_CS, tft.getSPIinstance())) {
    Serial.println("Card Mount Failed");
    return;
  }

  lcdOn();
  tft.fillScreen(TFT_BLACK);
  tft.println("     LONELY BINARY");
  tft.println("Digital Camera Demo");
  tft.println("");
  tft.println("Press Button at the back to take a photo.");

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(8);
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
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
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

  sensor_t *s = esp_camera_sensor_get();
  s->set_hmirror(s, 1);
  s->set_vflip(s, 0);
}


void take_picture() {
  static int counter = 0;
  static String imgFile = "";
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if (fb) {
    Serial.println(fb->len);
    Serial.println(fb->width);
    Serial.println(fb->height);
    Serial.println(fb->format);
    Serial.println(fb->timestamp.tv_sec);
    Serial.println(fb->timestamp.tv_usec);

    // write image to SD card
    sdOn();
    uint32_t start = millis();
    uint32_t end = start;
    counter++;
    imgFile = "/image" + String(counter) + ".jpg";
    File file = SD.open(imgFile, FILE_WRITE);
    file.write(fb->buf, fb->len);
    file.close();
    end = millis() - start;
    Serial.printf("%s file name\n", imgFile.c_str());
    Serial.printf("%u bytes written for %lu ms\n", fb->len, end);

    lcdOn();
    start = millis();
    TJpgDec.drawJpg(-60, 8, fb->buf, fb->len);
    end = millis() - start;
    Serial.printf("flash %lu ms\n\n", end);
  }

  esp_camera_fb_return(fb);
  fb = NULL;
}


void loop() {
  static bool is_take_photo = false;

  if (digitalRead(BTN) == LOW) {
    is_take_photo = true;
    take_picture();
    delay(200);
  }

  if ((digitalRead(BTN) == LOW) && (is_take_photo == true)) {
    is_take_photo = false;
    delay(200);
  }

}