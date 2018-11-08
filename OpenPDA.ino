#include <TouchScreen.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"


// These are 'flexible' lines that can be changed
#define TFT_CS 21
#define TFT_DC 4
#define TFT_RST 14
#define YPLUS A3
#define XPLUS A2
#define YMINS A1
#define XMINS A0

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

// SoftSPI - note that on some processors this might be *faster* than hardware SPI!
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, 18, 5, TFT_RST, 19);



void setup() {
  Serial.begin(115200);
  Serial.println("HX8357D Test!"); 

  //SPI.setClockDivider(SPI_CLOCK_DIV);
  tft.begin(HX8357D);

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(HX8357_RDPOWMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDCOLMOD);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDIM);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDSDR);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  
  Serial.println(F("Benchmark                Time (microseconds)"));

  tft.setRotation(1);
  tft.println("Hello World!!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
