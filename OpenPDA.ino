#include <TouchScreen.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"


// These are 'flexible' lines that can be changed
#define TFT_CS 21
#define TFT_DC 4
#define TFT_RST 14

#define YP 33  // must be an analog pin, use "An" notation!
#define XM 13  // must be an analog pin, use "An" notation!
#define YM 12   // can be a digital pin
#define XP 27   // can be a digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 148
#define TS_MINY 297
#define TS_MAXX 930
#define TS_MAXY 960

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

// SoftSPI - note that on some processors this might be *faster* than hardware SPI!
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, 18, 5, TFT_RST, 19);



void setup() {
  Serial.begin(115200);
  tft.begin(HX8357D);

  tft.fillScreen(0);
  tft.setRotation(0);
  tft.println("Hello World!!");
}

void loop() {

}
