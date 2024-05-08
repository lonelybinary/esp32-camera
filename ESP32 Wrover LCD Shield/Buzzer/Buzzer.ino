/*
  Lonely Binary ESP32 Wrover LCD Shield
  Buzzer Demo Code

  What is LEDC?
  The ESP32 LED Control module, commonly referred to as LEDC, is a highly flexible peripheral on the ESP32 chip designed to control the brightness of LEDs. It's used for applications that require Pulse Width Modulation (PWM) to achieve tasks like LED dimming, motor speed control, and audio signals generation.

  Why Use LEDC for Simulating PWM?
  Hardware-driven PWM: Unlike software PWM where the timing is controlled by the processor, LEDC utilizes the ESP32's hardware capabilities. This means the PWM generation doesn't load the CPU, leading to more efficient multitasking and smoother PWM output.
  Accuracy and Stability: Hardware PWM, such as that provided by the LEDC, is more accurate and stable compared to software PWM. This is because it's less susceptible to timing disturbances caused by other processes running on the CPU.
  High Resolution and Flexibility: With up to 13 bits of resolution, the LEDC module allows for very precise control over the PWM duty cycle, which is essential when precise dimming of LEDs or exact speed control of motors is needed.
  Multiple Channels: The ability to use multiple PWM channels simultaneously with independent control over each one's frequency and duty cycle is particularly useful for complex projects that need to manage multiple outputs.
  
  Which ESP32 Pin can I use?
  The ESP32 can output PWM using any GPIO via IO Matrix. Theoretically, the PWM can be distributed to any I/O except for those that only have input functions (e.g., GPIO34 ~ GPIO39).


  Required Library:
  ezBuzzer by ArduinoGetStarted.com

  Board:
  ESP32 Wrover Module
*/


#include <Arduino.h>
#include <ezBuzzer.h> 
// use LED Control (LEDC)
const int BUZZER_PIN = 32;

ezBuzzer buzzer(BUZZER_PIN); 

// notes in the melody:
int melody[] = {
    NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
    NOTE_E5,
    NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
    NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
    NOTE_D5, NOTE_G5};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations[] = {
    8, 8, 4,
    8, 8, 4,
    8, 8, 8, 8,
    2,
    8, 8, 8, 8,
    8, 8, 8, 16, 16,
    8, 8, 8, 8,
    4, 4};

void setup()
{
    Serial.begin(115200);
    Serial.println("Lonely Binary Buzzer Demo Code");
}

void loop()
{
    buzzer.loop(); 
    Serial.println(millis());
    if (buzzer.getState() == BUZZER_IDLE)
    {
        int length = sizeof(noteDurations) / sizeof(int);
        buzzer.playMelody(melody, noteDurations, length); // playing
    }
}