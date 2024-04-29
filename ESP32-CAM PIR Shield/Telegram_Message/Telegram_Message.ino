/*
  Lonely Binary ESP32-CAM PIR Shield

  Basic telgram test. 
  The telegram bot sends a message after ESP32-CAM boots up

  Required Library:
  UniversalTelegramBot by Brian Lough
  ArduinoJson by Benoit 
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// replace with your SSID and PASSWORD
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// replace with your bot_token (from Botfather) and chat_id (from IDBot @myidbot)
#define BOT_TOKEN "BOT_TOKEN"
#define CHAT_ID "CHAT_ID"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }

  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Lonely Binary ESP32-CAM PIR Shield", "");
}

void loop() {

}