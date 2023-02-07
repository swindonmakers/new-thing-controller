#include "SPI.h"
#include "src/Adafruit-GFX-Library/Adafruit_GFX.h"  //modified for RP2040
#include "src/Adafruit_ILI9341.h"                   //modified to point at local GFX library
#include <TaskScheduler.h>

#define TFT_CS 0
#define TFT_RST 1
#define TFT_DC 2
#define TFT_BL 3

Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);   //init with hardware SPI
GFXcanvas16 canvas(240, 320);         //Init canvas (framebuffer)

void updateTFTCB();
Task updateTFT(100, TASK_FOREVER, &updateTFTCB);

Scheduler ts;

uint16_t dispVal = 100;

void setup() {
  Serial.begin(115200);
  
  setupTFT();
  setupTasks();
}

void loop(void) {
  ts.execute();
}
