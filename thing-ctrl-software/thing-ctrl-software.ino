/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include "SPI.h"
#include "src/Adafruit-GFX-Library/Adafruit_GFX.h"  //modified for RP2040
#include "src/Adafruit_ILI9341.h"                   //modified to point at local GFX library

#define TFT_CS 0
#define TFT_RST 1
#define TFT_DC 2
#define TFT_BL 3


Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);   //init with hardware SPI
GFXcanvas16 canvas(240, 320);         //Init canvas (framebuffer)

uint16_t dispVal = 100;
uint32_t startTime;

void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  //turn backlight on
  tft.begin();    //begin at 48MHzz
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  
  startTime = millis();
}


void loop(void) {
  testText();
  dispVal--;
  if(dispVal == 0){
    uint32_t stopTime = millis();
    Serial.print("Took " ); Serial.print(stopTime - startTime); Serial.println(" millis");
    dispVal = 100;
    startTime = millis();
  }
}


void testText() {
  canvas.fillScreen(ILI9341_BLACK);
  canvas.setCursor(0, 0);
  canvas.setTextColor(ILI9341_WHITE);
  canvas.setTextSize(1);
  canvas.println("Hello World!");
  canvas.setTextColor(ILI9341_YELLOW);
  canvas.setTextSize(2);
  canvas.println(dispVal);
  canvas.setTextColor(ILI9341_RED);
  canvas.setTextSize(3);
  canvas.println(0xDEADBEEF, HEX);
  canvas.println();
  canvas.setTextColor(ILI9341_GREEN);
  canvas.setTextSize(5);
  canvas.println("Groop");
  canvas.setTextSize(2);
  canvas.println("I implore thee,");
  canvas.setTextSize(1);
  canvas.println("my foonting turlingdromes.");
  canvas.println("And hooptiously drangle me");
  canvas.println("with crinkly bindlewurdles,");
  canvas.println("Or I will rend thee");
  canvas.println("in the gobberwarts");
  canvas.println("with my blurglecruncheon,");
  canvas.println("see if I don't!");
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());   //write framebuffer to device
}
