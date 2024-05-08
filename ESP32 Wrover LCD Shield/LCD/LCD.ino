/*
  Lonely Binary ESP32 Wrover LCD Shield
  LCD Demo Code

  The LCD and SD Card are connected to the same ESP32 SPI Bus.

  SPI is commonly used to connect microcontrollers to peripherals such as LCD, sensors, SD cards, and shift registers, among other devices.

  Four Wire Interface: Standard SPI communication uses four wires:
  SCLK (Serial Clock): Output from master, used to synchronize data transmission.
  MOSI (Master Out Slave In): Data line from master to slave.
  MISO (Master In Slave Out): Data line from slave to master.
  CS/SS (Chip Select/Slave Select): Output from master to enable or disable individual slave devices. It is active low, meaning a slave is selected when the signal is low.
   
  Pin Configuration:
  replace "User_Setup.h" in TFT_eSPI folder

  Required Library:
  TFT_eSPI by Bodmer

  Board:
  ESP32 Wrover Module
*/

#include <Arduino.h>
#include <TFT_eSPI.h>  // Graphics and font library for ST7735 driver chip

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_RED);
  delay(3000);
  tft.fillScreen(TFT_GREEN);
  delay(3000);
  tft.fillScreen(TFT_BLUE);
  delay(3000);
}

void loop() {
  tft.fillScreen(TFT_RED);
  delay(500);
  tft.fillScreen(TFT_GREEN);
  delay(500);
  tft.fillScreen(TFT_BLUE);
  delay(500);
}
