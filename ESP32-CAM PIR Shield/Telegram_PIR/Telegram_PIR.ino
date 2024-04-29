/*
  Lonely Binary ESP32-CAM PIR Shield
  
  
  When the Passive Infrared (PIR) sensor detects motion, it triggers the ESP32-CAM module. Upon activation, the ESP32-CAM immediately captures a photo and sends a notification along with the image to a predefined Telegram bot. This setup leverages the ESP32-CAM's ability to function as both a surveillance and communication device.

  Note that in this setup, the PIR sensor shares a pin with the serial communication function on the ESP32-CAM. Consequently, serial outputs are disabled to prevent interference with the sensor's operation. This is important for maintaining the reliability of the motion detection feature.

  To remotely monitor the device or for troubleshooting purposes, you can locate the IP address of your ESP32-CAM via your router's administration interface. This IP address can be useful for accessing the camera's live feed or adjusting settings through a web interface. Consider implementing additional features such as timestamping the photos for better tracking or integrating more complex notifications based on specific criteria to enhance the functionality of your home security system.
  
  (Ctrl+Z or opt+Z to triggle word wrap in Arduino IDE)
  
  Required Library:
  UniversalTelegramBot by Brian Lough
  ArduinoJson by Benoit 
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "esp_camera.h"
#include <ArduinoJson.h>

// replace with your SSID and PASSWORD
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// replace with your bot_token (from Botfather) and chat_id (from IDBot @myidbot)
#define BOT_TOKEN "BOT_TOKEN"
#define CHAT_ID "CHAT_ID"

// Camera Configuration
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define FLASH_LED_PIN 4  // Active High
#define PIR 3            // Active LOW

bool setupCamera() {
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
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    return false;
  }

  return true;
}


const unsigned long BOT_MTBS = 1000;  // mean time between scan messages
const unsigned long PHOTO_MTBS = 5000;

unsigned long bot_lasttime;    // last time messages' scan has been done
unsigned long photo_lasttime;  // last time messages' scan has been done
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

bool flashState = LOW;

camera_fb_t *fb = NULL;

bool isMoreDataAvailable();
byte *getNextBuffer();
int getNextBufferLen();

bool dataAvailable = false;

void send_photo(String chat_id) {
  fb = NULL;
  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb) {
    //Serial.println("Camera capture failed");
    bot.sendMessage(chat_id, "Camera capture failed", "");
    return;
  }
  dataAvailable = true;
  //Serial.println("Sending");
  bot.sendPhotoByBinary(chat_id, "image/jpeg", fb->len,
                        isMoreDataAvailable, nullptr,
                        getNextBuffer, getNextBufferLen);

  //Serial.println("done!");

  esp_camera_fb_return(fb);
}

void handleNewMessages(int numNewMessages) {
  // Serial.println("handleNewMessages");
  // Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
    }

    if (text == "/photo") {
      send_photo(chat_id);
    }

    if (text == "/start") {
      String welcome = "Welcome to the ESP32Cam Telegram bot.\n\n";
      welcome += "/photo : will take a photo\n";
      welcome += "/flash : toggle flash LED (VERY BRIGHT!)\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

bool isMoreDataAvailable() {
  if (dataAvailable) {
    dataAvailable = false;
    return true;
  } else {
    return false;
  }
}

byte *getNextBuffer() {
  if (fb) {
    return fb->buf;
  } else {
    return nullptr;
  }
}

int getNextBufferLen() {
  if (fb) {
    return fb->len;
  } else {
    return 0;
  }
}

void setup() {
  // *DON'T USE SERIAL OUTPUT
  pinMode(PIR, INPUT);

  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);  //defaults to low

  if (!setupCamera()) {
    while (true) {
      delay(100);
    }
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  configTime(0, 0, "pool.ntp.org");  // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    delay(100);
    now = time(nullptr);
  }

  // Make the bot wait for a new message for up to 60seconds
  bot.longPoll = 60;

  digitalWrite(FLASH_LED_PIN, HIGH);
  delay(1000);
  digitalWrite(FLASH_LED_PIN, LOW);
  send_photo(CHAT_ID);
  pinMode(PIR, INPUT);
}

void loop() {

  if (digitalRead(PIR) == LOW) {
    digitalWrite(FLASH_LED_PIN, HIGH);
    send_photo(CHAT_ID);
    bot.sendMessage(CHAT_ID, "Intruder Detected", "");
    delay(1000);
  } else {
    digitalWrite(FLASH_LED_PIN, LOW);
  }
}