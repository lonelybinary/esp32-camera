/*
  Lonely Binary ESP32 Wrover LCD Shield Pin Configuration
  Copy and Overwrite this file to "tft_espi" library folder
*/
#define USER_SETUP_INFO "User_Setup"
#define ST7735_DRIVER        
#define TFT_RGB_ORDER TFT_RGB 


#define TFT_MISO 12
#define TFT_MOSI 14
#define TFT_SCLK 13
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1  

#define LOAD_GLCD
#define LOAD_FONT2 
#define LOAD_FONT4 
#define LOAD_FONT6
#define LOAD_FONT7  
#define LOAD_FONT8  
#define LOAD_FONT8N 
#define LOAD_GFXFF 
#define SMOOTH_FONT


#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000

