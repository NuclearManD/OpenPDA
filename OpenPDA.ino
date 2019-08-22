#include <TouchScreen.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"

#define LEDC_CHANNEL_0 0
#define LEDC_CHANNEL_1 1

#define PERIPH_PIR 1
#define PERIPH_HONKER 2

#define LASER_PIN 22
#define BUTTON_PIN 34
void println(String s);
void print(String s);

#include "lcd_control.h"
#include "tv_b_gone.h"
#include "neonet.h"
#include "neonet_pgm.h"
#include "musicd.h"
#include "wifid.h"
#include "jpeg.h"

String logfile = "";

void println(String s){
  Serial.println(s);
  logfile+=s+'\n';
}
void print(String s){
  Serial.print(s);
  logfile+=s;
}

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
  
  pinMode(BUTTON_PIN, INPUT);

  SPI.setFrequency(8000000);

  lcd_setup();
  
  ledcSetup(5, 80, 13);
  ledcAttachPin(IRLED, 5);
    
  println("LEDC Configured...");
  
  setBrightness(30);
  
  analogReadResolution(12);
  analogSetSamples(65536);
  analogSetClockDiv(8);
  analogSetAttenuation(ADC_11db);
  
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
    screenIdleHandler();
    TSPoint p=getPoint();
    if(p.z==0)continue;
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
  if(tvbgd_running){
    drawIcon(25,25,104);
  }else{
    drawIcon(25,25,4);
  }
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
  
  drawIcon(25,125,7); //log icon
  tft.drawRect(0,100,100,100,WHITE);
  
  tft.setCursor(2,203);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.print("DT UTILS");
  tft.drawRect(0,200,100,100,WHITE);
  
  drawIcon(125,225,3); // settings icon
  tft.drawRect(100,200,100,100,WHITE);

  drawIcon(225,125,6); // SCAA logo
  
  if(laser){
    // fill red triangle for laser at (220, 10)
    tft.fillTriangle(220,10,228, 22, 212, 22, RED);
  }else{
    // draw a red triangle for no laser at (220, 10)
    tft.drawTriangle(220,10,228, 22, 212, 22, RED);
  }

  // music icon
  drawIcon(25,325,8);
  tft.drawRect(0,300,100,100,WHITE);

  // wifi icon
  drawIcon(125,325,1);
  tft.drawRect(100,300,100,100,WHITE);

  while(true){
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    if(p.x<100&&p.y<100){
      if(tvbgd_running){
        drawIcon(25,25,4);
        stop_tv_b_gone();
      }else{
        drawIcon(25,25,104);
        start_tv_b_gone();
      }
      delay(100);
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
        screenIdleHandler();
        TSPoint p;
        if((p=getPoint()).z==0)continue;
        idleTime=millis()+180*1000; // three minutes
        break;
      }
      return;
    }else if(p.x<100&&p.y>200&&p.y<300){
      dtUtilProgram();
      return;
    }else if(p.x>100&&p.x<200&&p.y>200&&p.y<300){
      settingsProgram();
      return;
    }else if(p.x<100&&p.y>300&&p.y<400){
      musicProgram();
      return;
    }else if(p.x>100&&p.x<200&&p.y>300&&p.y<400){
      wifiProgram();
      return;
    }
  }
  

  // process a press (from main menu)
}
void dtUtilBaseRender(){
  // open prank toolkit
  tft.fillScreen(BLACK);
  tft.drawRect(0,0,100,100,WHITE);
  tft.setCursor(0,2);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.print(" BOMB\n PRANK");
  /*tft.drawRect(100,0,100,100,WHITE);
  tft.setCursor(105,5);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("AUTO HONK");
  if(honker_running){
    tft.setCursor(105,30);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.print("STOP");
  }*/
  tft.drawRect(100,100,100,100,WHITE);
  tft.setCursor(125,125);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("BACK");
}
void dtUtilProgram(){
  // do a login
  doLogin();
  dtUtilBaseRender();
  while(true){
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    /*if(p.x>100&&p.x<200&&p.y<100){
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
    }else */if(p.x<100&&p.y<100){
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
      dtUtilBaseRender();
      println("Done.");
      break;
    }else if(p.x>100&&p.y>100&&p.x<200&&p.y<200){
      return;
    }
  }
}


void musicBaseRender(){
  // open prank toolkit

  uint16_t color = MAGENTA;
  if(musicd_running)color = GREY1;

  tft.fillScreen(BLACK);
  tft.drawRect(0,0,100,100,WHITE);
  tft.setCursor(0,2);
  tft.setTextSize(2);
  tft.setTextColor(color);
  tft.print(" Mario");
  
  tft.drawRect(100,0,100,100,WHITE);
  tft.setCursor(105,5);
  tft.setTextSize(2);
  tft.setTextColor(color);
  tft.print("Mario (simple)");
  
  tft.drawRect(100,100,100,100,WHITE);
  tft.setCursor(125,125);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("BACK");
}
void musicProgram(){
  musicBaseRender();
  while(true){
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    if(p.x>100&&p.x<200&&p.y<100){
      if(!musicd_running){
        start_music_daemon(1);
        continue;
      }
    }else if(p.x<100&&p.y<100){
      if(!musicd_running){
        start_music_daemon(0);
        continue;
      }
    }else if(p.x>100&&p.y>100&&p.x<200&&p.y<200){
      return;
    }
    musicBaseRender();
  }
}
void wifiBaseRender(){
  // open wifi control panel

  tft.fillScreen(BLACK);
  tft.drawRect(0,0,100,100,WHITE);
  tft.setCursor(0,2);
  tft.setTextSize(2);
  tft.setTextColor(((wifid_mode==MODE_OFF)||(wifid_mode==MODE_SCAN)) ? BLUE : GREY1);
  tft.print(" Scan");
  
  tft.drawRect(100,0,100,100,WHITE);
  tft.setCursor(105,5);
  tft.setTextSize(2);
  tft.setTextColor(enable_thanos ? RED : GREY1);
  tft.print("Thanos");
  if(enable_thanos){
    tft.setCursor(105,35);
    tft.print("[runnning]");
  }
  
  tft.drawRect(100,100,100,100,WHITE);
  tft.setCursor(125,125);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("BACK");

  if(wifid_mode!=MODE_OFF){
    tft.drawRect(0,100,100,100,WHITE);
    tft.setCursor(5,105);
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.print("Disable");
  }else{
    tft.drawRect(0,100,100,100,WHITE);
    tft.setCursor(5,105);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.print("NeoNet");
  }
  
  tft.drawRect(0,200,100,100,WHITE);
  drawIcon(25,225,9);

  tft.setCursor(10,300);
  tft.print("WiFi Status: ");
  if(wifid_mode==MODE_OFF)tft.print("OFF");
  else if(wifid_mode==MODE_STA)tft.print("Connected");
  else if(wifid_mode==MODE_AP){
    tft.print("Access Point");
    if(enable_thanos)tft.print(" (thanos)");
  }else if(wifid_mode==MODE_SCAN){
    tft.print("Scanning");
  }
}
void wifiProgram(){
  wifiBaseRender();
  while(true){
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    if(p.x>100&&p.x<200&&p.y<100){
      if(enable_thanos)stop_thanos();
      else start_thanos();
    }else if(p.x<100&&p.y<100){
      if(wifid_mode==MODE_OFF)enable_wifi();
      if(wifid_mode==MODE_SCAN){
        long timer = millis()+10000;
        tft.fillScreen(BLACK);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.setCursor(0,0);
        tft.println("Scanning...");
        while((timer>millis())&&(netscan_len==0));
        tft.println(String("Found ")+netscan_len+" networks:");
        Serial.println(netscan_len);
        for(int i=0;i<netscan_len;i++){
          tft.print(WiFi.SSID(i));
          if(WiFi.encryptionType(i) != WIFI_AUTH_OPEN)
            tft.print(" *");
          tft.println();
        }
        while(true){
          screenIdleHandler();
          if(getPoint().z==0)continue;
          idleTime=millis()+180*1000; // three minutes
          break;
        }
      }
    }else if(p.x>100&&p.y>100&&p.x<200&&p.y<200){
      return;
    }else if(p.y>100&&p.x<100&&p.y<200){
      if(wifid_mode!=MODE_OFF)disable_wifi();
      else{
        if(wifi_connect("nti", "littleguys")){
          display("Connecting to WiFi...");
          // Wait for connect
          long timer = millis()+10000;
          while((WiFi.status() != WL_CONNECTED)&&(timer>millis())){
            delay(20);
          }
          if(WiFi.status() != WL_CONNECTED){
            disable_wifi();
            println("Connection failure.");
            popup("Connection error");
          }else{
            display("Connecting to NeoNet...");
            // now connect to NeoNet
            if(NeoNetSetup()){
              println("Connected to NeoNet.");
              ping(); // keep connection, this also responds to any ping from the remote.
              //popup("NeoNet connection will close when this program is closed.");
              uint64_t adr_targ = ask_neonet_adr();
              display("Discovering protocols...");

              /* Enumerate protocols
               *
               * Process is parallelized so that packets are sent on all protocols, then we listen for responses.
               */
              // Test Plasma
              sendNrlPacket(adr_targ, (byte*)"ls", 2, PORT_PLASMA_SICO);

              // Test Logread
              byte cmdrd[1] = {CMD_READ_LOG};
              sendNrlPacket(adr_targ, (byte*)cmdrd, 1, PORT_LOGREAD);

              // Test GPS
              sendNrlPacket(adr_targ, (byte*)"", 0, PORT_GPS);

              // Timeout setting at 8 seconds
              long timer = millis() + 8000;

              String gps_data = "";
              String log_data = "";
              int periphs = 0;
              String periph_txts = "";
              int loglen = -1;
              while(timer>millis()){
                int len = getNrlPacket(timer-millis());
                if(len>=0){
                  Serial.println(len);
                  Serial.println(nrl_port);
                  Serial.println((char*)nrl_data);
  
                  if(nrl_target==nrl_adr){
                    if(nrl_port == PORT_PLASMA_SOCI){
                      int i=0;
                      String text = "";
                      boolean cmd = false;
                      for(;i<len;i++){
                        if(nrl_data[i]==0){
                          if(cmd){
                            if(text=="honker")
                              periphs|=PERIPH_HONKER;
                            else if(text=="PIR")
                              periphs|=PERIPH_PIR;
                            periph_txts+=text;
                            if(i+1<len){
                              periph_txts+=", ";
                            }
                          }else{
                            if(text!="ok")break;
                            cmd = true;
                          }
                          text = "";
                        }else
                          text+=(char)nrl_data[i];
                      }
                    }else if(nrl_port == PORT_LOGREAD){
                      if(loglen==-1)loglen=0;
                      else log_data+=(char*)nrl_data;
                    }else if(nrl_port == PORT_GPS){
                      gps_data=(char*)nrl_data;
                    }
                  }
                }
              }
              neonet_ctrl_pgm(gps_data, log_data, adr_targ, periphs);

              // turn NeoNet off when done.
              neonet_socket.stop();
              delay(10);
            }else{
              popup("Could not connect to NeoNet.");
            }
          }
        }else{
          popup("Error with wifid: check logs.");
        }
      }
    }else if(p.y>200&&p.x<100&&p.y<300){
      if(wifi_connect("OpenPDA Camera","")){
        long timer = millis()+10000;
        display("Connecting to camera...");
        while((WiFi.status() != WL_CONNECTED)&&(timer>millis())){
          delay(20);
        }
        if(WiFi.status() != WL_CONNECTED){
          disable_wifi();
          println("Connection failure.");
          popup("Connection error");
        }else{
          println("Connected.");
          while(true){
            take_pic_subprogram();
          }
        }
      }else{
        popup("Error with wifid: check logs.");
      }
    }
    wifiBaseRender();
  }
}

void settingsBaseRender(){
  tft.fillScreen(BLACK);
  
  tft.drawRect(0,0,100,100,WHITE);      // icon for 5s tvbg period
  tft.setCursor(0,2);
  tft.setTextSize(1);
  if(tvbgd_command==COMMAND_ALL_OFF_5S)
    tft.setTextColor(BLUE);
  else
    tft.setTextColor(WHITE);
  tft.print("5s tvbg period");
  
  tft.drawRect(100,0,100,100,WHITE);      // icon for 10s tvbg period
  tft.setCursor(105,5);
  if(tvbgd_command==COMMAND_ALL_OFF_10S)
    tft.setTextColor(BLUE);
  else
    tft.setTextColor(WHITE);
  tft.print("10s tvbg period");
  
  tft.drawRect(0,100,100,100,WHITE);      // icon for tvbg lg only, 3s
  tft.setCursor(5,105);
  if(tvbgd_command==COMMAND_LG_OFF_W3S)
    tft.setTextColor(BLUE);
  else
    tft.setTextColor(WHITE);
  tft.print("3s LG period");
  
  tft.drawRect(100,100,100,100,WHITE);      // icon for tvbg lg only, 1s
  tft.setCursor(105,105);
  if(tvbgd_command==COMMAND_LG_OFF_W1S)
    tft.setTextColor(BLUE);
  else
    tft.setTextColor(WHITE);
  tft.print("1s LG period");
  
  tft.drawRect(100,300,100,100,WHITE);
  tft.setCursor(125,325);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("BACK");
}

void settingsProgram(){
  // open settings
  settingsBaseRender();
  while(true){
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    if(p.x>100&&p.x<200&&p.y<100){
      tvbgd_command=COMMAND_ALL_OFF_10S;
    }else if(p.x<100&&p.y<100){
      tvbgd_command=COMMAND_ALL_OFF_5S;
    }else if(p.x<100&&p.y>100&&p.y<200){
      tvbgd_command=COMMAND_LG_OFF_W3S;
    }else if(p.x>100&&p.x<200&&p.y>100&&p.y<200){
      tvbgd_command=COMMAND_LG_OFF_W1S;
    }else if(p.x>100&&p.y>300&&p.x<200&&p.y<400){
      return;
    }else{
      Serial.print(p.x);
      Serial.print(", ");
      Serial.println(p.y);
      continue;
    }
    settingsBaseRender();
  }
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

void mkTask(void (*taskFunc)(void*),char* name, TaskHandle_t* taskVar, int stack){
  xTaskCreatePinnedToCore(
   taskFunc,                  /* pvTaskCode */
   name,            /* pcName */
   stack,                   /* usStackDepth */
   NULL,                   /* pvParameters */
   1,                      /* uxPriority */
   taskVar,                 /* pxCreatedTask */
   1-xPortGetCoreID());                     /* xCoreID */
}
