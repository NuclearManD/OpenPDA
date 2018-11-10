#include <TouchScreen.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
#define GREY1    0xAD55
#define GREY2    0x52AA

// These are 'flexible' lines that can be changed
#define TFT_CS 21
#define TFT_DC 4
#define TFT_RST 14

#define YP 33  // must be an analog pin, use "An" notation!
#define XM 13  // must be an analog pin, use "An" notation!
#define YM 12   // can be a digital pin
#define XP 27   // can be a digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 300
#define TS_MAXX 920
#define TS_MAXY 950

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 262);

boolean redraw = false;
boolean screenDirty = false;
int state = -1;

void setup() {
  Serial.begin(115200);
  analogReadResolution(10);
  analogSetSamples(1024);
  analogSetAttenuation(ADC_11db);
  tft.begin(HX8357D);

  tft.fillScreen(0);
  tft.setRotation(0);

  redraw = true;
  state  = 0;
}

void loop() {
  if(redraw){
    drawHome();
    redraw = false;
  }
}

#define ICONSIZE 50

int numPgms = 2;
char* pgmNames[] = {"Storage","Network"};
int   pgmIcons[] = {0,1};
void drawHome(){
  if(screenDirty){
    tft.fillScreen(0);
    screenDirty=false;
  }
  // draw background
  //  (none yet)
  // draw icons
  for(int i=0;i<numPgms;i++){
    drawIcon(10, i*(ICONSIZE+10)+5, pgmIcons[i]);
    tft.setCursor(20+ICONSIZE, i*(ICONSIZE+10) + 10);
    tft.print(pgmNames[i]);
  }
}
void drawIcon(int x, int y, int id){
  if(id==0){ // hard disk icon
    tft.drawRect(x,y+5,50,40,CYAN);
    tft.fillCircle(x+19,y+24, 18, GREY1);
    tft.drawLine(x+23, y+29, x+48, y+42, GREY2);
  }else if(id==1){
    tft.drawLine(x+25, y+10, x+25, y+49, RED);
    tft.
  }
}
TSPoint getPoint(){
  TSPoint p = ts.getPoint();
  if(p.z<10 || p.z>1000)return *((TSPoint*)0);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
  return p;
}

