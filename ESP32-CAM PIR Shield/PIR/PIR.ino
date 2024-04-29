/*
  Lonely Binary ESP32-CAM PIR Shield
  
  When the Passive Infrared (PIR) sensor detects motion, it triggers the ESP32-CAM's built-in Flash.
*/

#include <Arduino.h>
#define PIR 3   // Active LOW
#define FLASH 4 // Active High

void setup()
{
    pinMode(PIR, INPUT);
    pinMode(FLASH, OUTPUT);
}

void loop()
{
    if (digitalRead(PIR) == HIGH)
    {
        digitalWrite(FLASH, LOW);
        delay(100);
    }
    else
    {
        digitalWrite(FLASH, HIGH);
        delay(100);
        digitalWrite(FLASH, LOW);
        delay(100);
    }

}