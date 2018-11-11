#include <TouchScreen.h>
#include <SPI.h>
#include "WiFi.h"
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
#define GREY3    0xDF5D

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
#define TS_MINY 275
#define TS_MAXX 920
#define TS_MAXY 950

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 262);

TaskHandle_t guiPgm;

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
  tft.setRotation(1);

  mkTask(guiProcessFunc, "GUI Receiver", &guiPgm, 8192);

  redraw = true;
  state  = -1;
}
#define STATE_LOCKED -1
#define STATE_HOME 0
#define STATE_STORAGE 1
#define STATE_NETWORK 2

int numStorageDevices = 0;

void loop() {
  if(redraw){
    if(state==STATE_LOCKED){
      state=STATE_HOME;
    }
    if(state==STATE_HOME){
      drawHome();
    }else if(state==STATE_STORAGE){
      tft.setTextColor(WHITE);
      tft.fillRoundRect(15,15,tft.width()-45,180,5,BLUE);
      tft.setCursor(35,35);
      tft.setTextSize(2);
      if(numStorageDevices==0){
        tft.print("No storage devices.");
      }else{
        
      }
      tft.fillRect(30,150,60,20, WHITE);
      tft.setTextColor(BLUE);
      tft.setCursor(32,155);
      tft.print("HOME");
    }else if(state==STATE_NETWORK){
      tft.setTextColor(WHITE);
      tft.fillRoundRect(15,15,tft.width()-45,180,5,RED);
      tft.setCursor(35,35);
      tft.setTextSize(1);
      
      tft.print("[networking disabled]");
      
      tft.setCursor(250, 35);

      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(200);
      int numNetworks = WiFi.scanNetworks();
      
      if(numNetworks==0){
        tft.print("No APs!");
      }else{
        for(int i=0;i<numNetworks;i++){
          tft.setCursor(250, 35+i*20);
          tft.print(WiFi.SSID(i));
          tft.print(" : ");
          tft.print(WiFi.RSSI(i));
        }
      }
      WiFi.mode(WIFI_OFF);
      //btStop();
      
      tft.fillRect(30,150,60,20, WHITE);
      tft.setTextColor(RED);
      tft.setCursor(32,155);
      tft.print("HOME");
    }
    redraw = false;
  }else{
    delay(20);
  }
}

void guiProcessFunc(void*){
  while(true){
    delay(20);
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    Serial.print(p.x);
    Serial.print(" ");
    Serial.println(p.y);
    if(state==STATE_HOME){
      if(p.x<170){
        redraw=true;
        if(p.y<60){ // storage selected
          state=STATE_STORAGE;
        }else if(p.y<110){ // network selected
          state=STATE_NETWORK;
        }
      }
    }else if(state==STATE_STORAGE){
      if((p.x>30)&&(p.y>150)&&(p.x<90)&&(p.y<170)){
        state=STATE_HOME;
        redraw=true;
        screenDirty=true;
      }
    }else if(state==STATE_NETWORK){
      if((p.x>30)&&(p.y>150)&&(p.x<90)&&(p.y<170)){
        state=STATE_HOME;
        redraw=true;
        screenDirty=true;
      }
    }
    delay(80);
  }
}

#define ICONSIZE 50

int numPgms = 2;
char* pgmNames[] = {"Storage","Network"};
int   pgmIcons[] = {2,1};
void drawHome(){
  if(screenDirty){
    tft.fillScreen(0);
    screenDirty=false;
  }
  tft.setTextColor(WHITE);
  // draw background
  //  (none yet)
  // draw icons
  tft.setTextSize(3);
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
    tft.fillCircle(x+19,y+24, 3, GREY2);
    tft.fillTriangle(x+23, y+29, x+48, y+42, x+48, y+38, GREY2);
  }else if(id==1){ // wireless/network icon 1
    for(int i=3;i>0;i--){
      tft.drawCircle(x+25, y+10, i*2+2, RED);
    }
    tft.fillRect(x+10, y+10, 40, 25, 0);
    tft.fillCircle(x+25, y+10, 2, RED);
    
    tft.drawLine(x+25, y+10, x+25, y+50, RED);
    tft.drawLine(x, y+50, x+50, y+50, RED);
  }else if(id==2){ // floppy disk icon
    tft.fillRect(x,y+10,45,40, GREY2);
    tft.fillRect(x+10,y,35,50, GREY2);
    tft.fillTriangle(x, y+10, x+10, y, x+10, y+10, GREY2);
    tft.fillRect(x+15,y,10,10,GREY1);
    tft.drawRect(x+25,y,7,10,GREY1);
    tft.fillRect(x+32,y,3,10,GREY1);
    tft.fillRect(x+12,y+25,26,25, GREY3);
  }
}
TSPoint getPoint(){
  TSPoint p = ts.getPoint();
  if(p.z<10 || p.z>1000){
    p.z=0;
    return p;
  }
  int y=p.y;
  p.y = tft.height()-map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
  p.x = map(y, TS_MINY, TS_MAXY, 0, tft.width());
  return p;
}

void mkTask(void (*taskFunc)(void*),char* name, TaskHandle_t* taskVar, int stack){
  xTaskCreatePinnedToCore(
   taskFunc,                  /* pvTaskCode */
   name,            /* pcName */
   stack,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   taskVar,                 /* pxCreatedTask */
   0);                     /* xCoreID */
}

