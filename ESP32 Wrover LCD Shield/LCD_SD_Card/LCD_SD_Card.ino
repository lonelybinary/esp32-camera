/*
  Lonely Binary ESP32 Wrover LCD Shield
  LCD and SD Card Demo Code

  In this demonstration, we will illustrate how to connect both an LCD display and an SD card to a single SPI bus on an ESP32 microcontroller. Using the SPI (Serial Peripheral Interface) bus allows multiple devices (in this case, an LCD and an SD card) to communicate with the ESP32 over the same set of wires, with each device being uniquely addressed.
  
  Both the LCD and the SD Card are interfaced with the ESP32 via the SPI bus, which consists of the following lines:
  SCLK (Serial Clock): This line provides the clock signal from the ESP32 to both the LCD and the SD card, synchronizing data transmission.
  MOSI (Master Out Slave In): This line carries data from the ESP32 to either the LCD or the SD card, depending on which device is selected.
  MISO (Master In Slave Out): This line is used to receive data from the SD card to the ESP32. (Note: LCDs typically do not send data back to the microcontroller over SPI, so MISO is primarily used for the SD card.)
  
  Each device (LCD and SD card) also has a CS (Chip Select) pin. The CS pin is crucial as it determines which device on the SPI bus is active at any given time.
  
  In our demo code, we will demonstrate how to selectively activate each device by manipulating the CS pins:
  Activating a Device: To communicate with a specific device, such as the LCD or the SD card, we set its CS pin to LOW. This action "selects" the device, indicating that the ESP32 intends to communicate with it. 
  Deactivating a Device: Conversely, to stop communicating with a device, we set its CS pin to HIGH. This effectively "deselects" the device, making it inactive on the SPI bus.
  
  LCD Pin Configuration:
  replace "User_Setup.h" in TFT_eSPI folder

  Required Library:
  TFT_eSPI by Bodmer

  Board:
  ESP32 Wrover Module
*/
#include <Arduino.h>
#include <TFT_eSPI.h>  // Graphics and font library for ST7735 driver chip
#include "FS.h"
#include "SD.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


void sdInfo() {

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}
void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(3000);

  Serial.println("Lonely Binary ESP32 Wrover LCD Shield");
  Serial.println("Using SD LCD Demo Code");

  pinMode(33, OUTPUT);
  digitalWrite(33, HIGH);  // SD card disable
  digitalWrite(15, HIGH);  // LCD disable

  tft.init();
  tft.setRotation(1);

  if (!SD.begin(33, tft.getSPIinstance())) {
    Serial.println("Card Mount Failed");
    return;
  }

  digitalWrite(33, HIGH);  // SD card disable
  digitalWrite(15, LOW);   // LCD active
  tft.fillScreen(TFT_RED);
  delay(3000);

  digitalWrite(33, LOW);   // SD card active
  digitalWrite(15, HIGH);  // LCD disable
  sdInfo();

  digitalWrite(33, HIGH);  // SD card disable
  digitalWrite(15, LOW);   // LCD active
  tft.fillScreen(TFT_GREEN);
  delay(3000);

  digitalWrite(33, LOW);   // SD card active
  digitalWrite(15, HIGH);  // LCD disable
  sdInfo();

  digitalWrite(33, HIGH);  // SD card disable
  digitalWrite(15, LOW);   // LCD active
  tft.fillScreen(TFT_BLUE);
  delay(3000);

  digitalWrite(33, LOW);   // SD card active
  digitalWrite(15, HIGH);  // LCD disable
  sdInfo();
                      
}

void loop() {
  // put your main code here, to run repeatedly:
}
