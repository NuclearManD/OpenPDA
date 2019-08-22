
#define BRIGHTNESS_PIN 13

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define ORANGE   0xFE40
#define WHITE    0xFFFF
#define GREY1    0xAD55
#define GREY2    0x52AA
#define GREY3    0xDF5D

// These are 'flexible' lines that can be changed
#define TFT_CS 21
#define TFT_DC 4
#define TFT_RST 14

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

void lcd_setup(){
  ledcSetup(1, 40000, 8);
  ledcAttachPin(BRIGHTNESS_PIN, 1);
}

int brightness = 100;

int getBrightness(){
  return brightness-35;
}
void setBrightness(int b){
  brightness=b+35;
  ledcWrite(1, (brightness*256)/100);
}

void lcdTone(int frequency){
  ledcSetup(1, frequency, 8);
  ledcWrite(1, (brightness*256)/100);
}

void lcdNoTone(){
  ledcSetup(1, 40000, 8);
  ledcWrite(1, (brightness*256)/100);
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
  }else if(id==3){  // gear/settings icon
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
  }else if(id==104){  // tv icon
    tft.fillRect(x+5,y+10,40,25,BLACK);
    tft.drawRect(x+5,y+10,40,25,GREY1);
    tft.drawLine(x+10,y+10,x+4,y+4,GREY2);
    tft.drawLine(x+40,y+10,x+46,y+4,GREY2);
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
  }else if(id==8){  // music note
    tft.fillCircle(x+10,y+35,5,MAGENTA);
    tft.fillCircle(x+40,y+45,5,MAGENTA);
    tft.drawLine(x+15,y+5,x+45,y+15, MAGENTA);
    tft.drawLine(x+15,y+5,x+15,y+35, MAGENTA);
    tft.drawLine(x+45,y+15,x+45,y+45, MAGENTA);
  }else if(id==9){  // camera
    tft.fillRect(x+5,y+15,40,30,GREY1);
    tft.fillCircle(x+15,y+30,7,BLACK);
    tft.drawLine(x+35,y+14,x+40,y+14,GREY2);
    tft.fillRect(x+32, y+20, 10, 5, WHITE);
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


void screenIdleHandler(){
  if(digitalRead(BUTTON_PIN)){
    idleTime = 0;
    delay(5);
    ledcWrite(1, 0);
    while(digitalRead(BUTTON_PIN));
    delay(5);
  }
  if(idleTime<millis()){
    ledcWrite(1, 0);
    while(true){
      delay(20);
      if(digitalRead(BUTTON_PIN))break;
    }
    delay(5);
    setBrightness(getBrightness());
    while(digitalRead(BUTTON_PIN));
    delay(5);
    idleTime=millis()+180*1000; // three minutes
  }
}

void display(char* message){
  tft.fillRect(20, 200, 280, 80, BLUE);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(21, 201);
  tft.print(message);
}
void display(char* message, uint16_t color){
  tft.fillRect(20, 200, 280, 80, color);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(21, 201);
  tft.print(message);
}

void popup(char* message){
  display(message, RED);
  tft.fillRect(140, 250, 40, 20, WHITE);
  tft.setCursor(141,251);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.print("OK");
  while(true){
    screenIdleHandler();
    TSPoint p=getPoint();
    if(p.z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    return;
    delay(5);
  }
}
