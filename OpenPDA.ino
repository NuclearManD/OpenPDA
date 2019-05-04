#include <MicroNMEA.h>

#include <TouchScreen.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#define LEDC_CHANNEL_0 0
#define LEDC_CHANNEL_1 1

#define LASER_PIN 22

#include "tv_b_gone.h"
#include "honker_control.h"

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
#define BRIGHTNESS_PIN 13

#define YP 33  // must be an analog pin, use "An" notation!
#define XM 32  // must be an analog pin, use "An" notation!
#define YM 12   // can be a digital pin
#define XP 27   // can be a digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX -2500//-160//
#define TS_MINY -2885//145//
#define TS_MAXX 255//1024//
#define TS_MAXY 750//1023//

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, MOSI, SCK, TFT_RST, MISO);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

long idleTime;
char nmeaBuffer[100];
MicroNMEA gps(nmeaBuffer, sizeof(nmeaBuffer));

String logfile = "";

void println(String s){
  Serial.println(s);
  logfile+=s+'\n';
}
void print(String s){
  Serial.print(s);
  logfile+=s;
}
/*uint64_t reg_a;
uint64_t reg_b;
uint64_t reg_c;*/
void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  println("Setting up...");
  
  tft.begin(HX8357D);

  tft.setRotation(2);
  drawIcon(0,0,5);
  
  println("TFT Configured...");

  pinMode(IRLED, OUTPUT);
  pinMode(LASER_PIN, OUTPUT);

  SPI.setFrequency(8000000);
  
  ledcSetup(1, 40000, 8);
  ledcAttachPin(BRIGHTNESS_PIN, 1);
  
  ledcSetup(5, 80, 13);
  ledcAttachPin(IRLED, 5);
  
  println("LEDC Configured...");
  
  setBrightness(30);
  
  analogReadResolution(12);
  analogSetSamples(65536);
  analogSetClockDiv(8);
  analogSetAttenuation(ADC_11db);

  delay(5);

  /*reg_a = READ_PERI_REG(SENS_SAR_START_FORCE_REG);
  reg_b = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG);
  reg_c = READ_PERI_REG(SENS_SAR_MEAS_START2_REG);*/
  
  println("ADC Configured...");

  idleTime=millis()+180*1000;
  println("Ready!");

  doLogin();
}
void doLogin(){
  tft.fillScreen(WHITE);
  drawIcon(0,0,5);
  // login
  while(true){
    if(idleTime<millis()){
      ledcWrite(1, 0);
      while(true){
        delay(20);
        if(getPoint().z!=0)break;
      }
      setBrightness(getBrightness());
      delay(100);
      idleTime=millis()+180*1000; // three minutes
    }
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    if(p.x<30&&p.y<30){
      break;
    }
    delay(5);
  }
}

boolean laser = false;

void loop() {
  // draw OS stuff
  tft.fillScreen(BLACK);
  drawIcon(25,25,4);
  tft.drawRect(0,0,100,100,WHITE);
  tft.setCursor(125,25);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.print("LASER");
  tft.drawRect(100,0,100,100,WHITE);
  tft.setCursor(125,125);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("OFF");
  tft.drawRect(100,100,100,100,WHITE);
  drawIcon(225,125,6);
  drawIcon(25,125,7);
  tft.drawRect(0,100,100,100,WHITE);
  tft.setCursor(2,203);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.print("DT UTILS");
  tft.drawRect(0,200,100,100,WHITE);
  if(laser){
    // fill red triangle for laser at (220, 10)
    tft.fillTriangle(220,10,228, 22, 212, 22, RED);
  }else{
    // draw a red triangle for no laser at (220, 10)
    tft.drawTriangle(220,10,228, 22, 212, 22, RED);
  }

  while(true){
    //sys_yield();
    
    //honker_daemon();
    if(idleTime<millis()){
      ledcWrite(1, 0);
      while(true){
        delay(20);
        if(getPoint().z!=0)break;
      }
      setBrightness(getBrightness());
      delay(100);
      idleTime=millis()+180*1000; // three minutes
    }
    while (Serial1.available()) {
      char c = Serial1.read();
      gps.process(c);
      Serial.write(c);
    }
    if(gps.getHour()!=99){
      tft.fillRect(225,50,64,20, BLACK);
      tft.setCursor(225,50);
      tft.setTextSize(1);
      tft.setTextColor(WHITE);
      tft.print(gps.getHour());
      tft.print(':');
      tft.print(gps.getMinute());
    }
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    if(p.x<100&&p.y<100){
      tft.fillRect(100,100,120,60, BLUE);
      tft.setCursor(110,110);
      tft.setTextSize(1);
      tft.setTextColor(WHITE);
      tft.print("Running...");
      println("Running TV-B-Gone...");
      long timer = millis();
      tv_b_gone();
      print("Finished in ");
      print(String(millis()-timer));
      println("ms");
      return;
    }else if(p.x>100&&p.x<200&&p.y<100){
      laser = !laser;
      digitalWrite(LASER_PIN, laser);

      // only redraw icon and stay in input loop (avoid visual disturbance of screen clear)
      if(laser){
        // fill red triangle for laser at (220, 10)
        tft.fillTriangle(220,10,228, 22, 212, 22, RED);
      }else{
        // drawa red triangle for no laser at (220, 10)
        tft.fillTriangle(220,10,228, 22, 212, 22, BLACK);
        tft.drawTriangle(220,10,228, 22, 212, 22, RED);
      }
      delay(150);
    }else if(p.x>100&&p.x<200&&p.y>100&&p.y<200){
      idleTime=0; // turn off the display
      ledcWrite(1, 0);
      delay(250);
    }else if(p.x<100&&p.y>100&&p.y<200){
      // open log file
      tft.fillScreen(BLACK);
      tft.setTextSize(1);
      tft.setTextColor(GREEN);
      tft.setCursor(0,0);
      tft.print(logfile);
      int len = logfile.length();
      delay(200);
      while(true){
        //sys_yield();
        if(logfile.length()>len){
          tft.fillScreen(BLACK);
          tft.setTextSize(1);
          tft.setTextColor(GREEN);
          tft.setCursor(0,0);
          tft.print(logfile);
          len = logfile.length();
        }
        if(idleTime<millis()){
          ledcWrite(1, 0);
          while(true){
            delay(20);
            if(getPoint().z!=0)break;
          }
          setBrightness(getBrightness());
          delay(100);
          idleTime=millis()+180*1000; // three minutes
        }
        TSPoint p;
        if((p=getPoint()).z==0)continue;
        idleTime=millis()+180*1000; // three minutes
        break;
      }
      return;
    }else if(p.x<100&&p.y>200&&p.y<300){
      // do a login
      doLogin();
      // open prank toolkit
      tft.fillScreen(BLACK);
      tft.drawRect(0,0,100,100,WHITE);
      tft.setCursor(0,2);
      tft.setTextSize(2);
      tft.setTextColor(BLUE);
      tft.print(" BOMB\n PRANK");
      tft.drawRect(100,0,100,100,WHITE);
      tft.setCursor(105,5);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.print("AUTO HONK");
      if(honker_running){
        tft.setCursor(105,30);
        tft.setTextSize(2);
        tft.setTextColor(WHITE);
        tft.print("STOP");
      }
      tft.drawRect(100,100,100,100,WHITE);
      tft.setCursor(125,125);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.print("BACK");
      while(true){
        //sys_yield();
        if(idleTime<millis()){
          ledcWrite(1, 0);
          while(true){
            delay(20);
            if(getPoint().z!=0)break;
          }
          setBrightness(getBrightness());
          delay(100);
          idleTime=millis()+180*1000; // three minutes
        }
        TSPoint p;
        if((p=getPoint()).z==0)continue;
        idleTime=millis()+180*1000; // three minutes
        if(p.x>100&&p.x<200&&p.y<100){
          if(!honker_running){
            startHonkd();
            tft.setCursor(105,30);
            tft.setTextSize(2);
            tft.setTextColor(WHITE);
            tft.print("STOP");
          }else{
            tft.fillRect(105,30,94,69,BLACK);
            stopHonkd();
          }
          delay(500);
        }else if(p.x<100&&p.y<100){
          tft.fillScreen(WHITE);
          int minutes = 2;
          int seconds = 13;
          long mil = 0;
          println("Started bomb prank.");
          while(true){
            if(mil<millis()){
              mil=millis()+1000;
              seconds-=1;
              if(seconds==-1){
                seconds=59;
                minutes--;
                if(minutes<0){
                  break;
                }
              }
              tft.fillRect(100,100,256,50,WHITE);
              tft.setCursor(100,100);
              tft.setTextSize(3);
              tft.setTextColor(RED);
              tft.print(minutes);
              tft.print(':');
              tft.println(seconds);
            }
          }
          tft.print("OOOOOOOOOOOOoooOOF");
          delay(3000);
          tft.fillScreen(BLACK);
          tft.setCursor(100,100);
          tft.setTextSize(3);
          tft.setTextColor(RED);
          tft.print("Get Pranxt.");
          delay(5000);
          tft.fillScreen(BLACK);            // rerender
          tft.drawRect(0,0,100,100,WHITE);
          tft.setCursor(25,25);
          tft.setTextSize(2);
          tft.setTextColor(BLUE);
          tft.print("BOMB PRANK");
          tft.drawRect(100,0,100,100,WHITE);
          tft.setCursor(125,25);
          tft.setTextSize(2);
          tft.setTextColor(WHITE);
          tft.print("AUTO HONK");
          tft.drawRect(100,100,100,100,WHITE);
          tft.setCursor(125,125);
          tft.setTextSize(2);
          tft.setTextColor(WHITE);
          tft.print("BACK");
          println("Done.");
          break;
        }else if(p.x>100&&p.y>100&&p.x<200&&p.y<200){
          return;
        }
      }
    }
  }
  

  // process a press (from main menu)
}
long reg_rw_timer = 0;
void sys_yield(){
  /*if(reg_rw_timer<millis()){
    WRITE_PERI_REG(SENS_SAR_START_FORCE_REG, reg_a);  // fix ADC registers
    WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, reg_b);
    WRITE_PERI_REG(SENS_SAR_MEAS_START2_REG, reg_c);
    reg_rw_timer = millis()+10;
  }*/
}

#define ICONSIZE 50
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
  }else if(id==3){
    tft.fillCircle(x+25,y+25,20,GREY1);
    tft.fillRect(x+20,y,10,50,GREY1);
    tft.fillRect(x,y+20,50,10,GREY1);
    tft.fillCircle(x+25,y+25,4,GREY2);
  }else if(id==4){  // tv-b-gone
    tft.fillRect(x+5,y+10,40,25,BLACK);
    tft.drawRect(x+5,y+10,40,25,GREY1);
    tft.drawLine(x+10,y+10,x+4,y+4,GREY2);
    tft.drawLine(x+40,y+10,x+46,y+4,GREY2);
    tft.drawLine(x+7,y+12,x+42,y+32,RED);
    tft.drawLine(x+42,y+12,x+7,y+32,RED);
    tft.drawRect(x+8,y+36,34,2,GREY2);
  }else if(id==5){  // scaa logo (POSITION INDEPENDEDNT!)
    int w = tft.width();
    int h = tft.height();
    tft.fillCircle(w/2,h/2,w/5, GREY1);
    tft.drawCircle(w/2,h/2,w/5, RED);
    tft.setTextSize(3);
    tft.setCursor(w/2 - 40,h/2 - 10);
    tft.setTextColor(BLUE);
    tft.print("SCAA");
  }else if(id==6){  // scaa logo (POSITION DEPENDEDNT!)
    int q = ICONSIZE/2;
    tft.fillCircle(x+q,y+q,q, GREY1);
    tft.drawCircle(x+q,y+q,q, RED);
    tft.setTextSize(1);
    tft.setCursor(x+q - 20,y+q - 5);
    tft.setTextColor(BLUE);
    tft.print("SCAA");
  }else if(id==7){  // logfile
    int q = ICONSIZE/2;
    tft.fillRect(x+5,y+5,40,40,BLACK);
    tft.drawRect(x+5,y+5,40,40,GREY2);
    tft.setCursor(x+7,y+7);
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.print("> LOG");
  }
}
TSPoint getPoint(){
  TSPoint p;
  for(int i=0;i<100;i++){
    p = ts.getPoint();
    /*Serial.print(p.x);
      Serial.print(" ");
      Serial.print(p.y);
      Serial.print(" ");
      Serial.println(p.z);
      delay(100); //*/
    if(p.x<TS_MINX||p.y<TS_MINY||p.x>TS_MAXX||p.y>TS_MAXY){
      continue;
    }
    int y=p.y;
    p.x = tft.width()-map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = tft.height()-map(y, TS_MINY, TS_MAXY, 0, tft.height());
    if(p.z==0)p.z=1;
    if(p.x<0||p.y<0||p.x>tft.width()||p.y>tft.height())p.z=0;
    else{
      /*
      Serial.print(p.x);
      Serial.print(" ");
      Serial.print(p.y);
      Serial.print(" ");
      Serial.println(p.z);
      delay(100);         //*/
    }
    return p;
  }
  p.z=0;
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
   1);                     /* xCoreID */
}

int brightness = 100;

int getBrightness(){
  return brightness-35;
}
void setBrightness(int b){
  brightness=b+35;
  ledcWrite(1, (brightness*256)/100);
}
