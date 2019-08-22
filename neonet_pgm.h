
extern Adafruit_HX8357 tft;
extern TouchScreen ts;

uint64_t dial_neonet_adr();

uint64_t neonet_registry_adrs[] = {
  0x115200,
  0xf00f1337,
  0x12190002,
  0xFB920666
};

char* neonet_registry_lbls[] = {
  "Apex",
  "SketchyWalker",
  "LeonardoGPS/KBD",
  "MegaHonker0"
};

int neonet_registry_len = 4;

void render_ask_neonet_adr(boolean full_repaint=true, int sel=-1){
  if(full_repaint){
    tft.fillScreen(BLACK);
    tft.drawRect(0,0, 160, 75, WHITE);
    tft.setCursor(10,5);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("DIAL");
  }

  if(sel>=0){
    tft.drawRect(160,0, 160, 75, WHITE);
    tft.setCursor(170,5);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("SELECT >");
  }else if(!full_repaint){
    tft.fillRect(160, 0, 160, 75, BLACK);
  }

  int y=75;

  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  
  for(int i=0;i<neonet_registry_len;i++){
    tft.drawRect(0,y, 320, 75, i==sel ? ORANGE : WHITE);
    tft.setTextColor(i==sel ? ORANGE : WHITE);

    uint64_t address = neonet_registry_adrs[i];
    
    tft.setCursor(10, y+5);
    tft.print("0x"+String((uint32_t)(address>>32), HEX));
    String tmp = String((uint32_t)(address), HEX);
    for(int i=0;i<8-tmp.length();i++)tft.print('0');
    tft.print(tmp+" | "+neonet_registry_lbls[i]);
    y+=75;
  }
}

uint64_t ask_neonet_adr(){
  render_ask_neonet_adr();
  int selected = -1;
  while(true){
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes

    if((p.x>160)&&(p.y<75)&&(selected!=-1)){
      return neonet_registry_adrs[selected];
    }else if((p.x<160)&&(p.y<75)){
      return dial_neonet_adr();
    }else if(p.y>75){
      selected = (p.y-75)/75;
      render_ask_neonet_adr(false, selected);
    }
  }
}

int val_map[] = { 1, 2, 3, 10, 4, 5, 6, 11, 7, 8, 9, 12, 13, 0, 14, 15};

void render_dial_neonet_adr(boolean full_repaint=true){
  if(full_repaint){
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    for(int i=0;i<16;i++){
      tft.drawRect((i%4)*80, (i/4)*80 + 160, 80, 80, WHITE);
      tft.setCursor((i%4)*80 + 5, (i/4)*80 + 165);
      String tmp = String(val_map[i], HEX);
      tmp.toUpperCase();
      tft.print(tmp);
    }
    tft.drawRect(220, 80, 100, 80, WHITE);
    tft.setCursor(225, 85);
    tft.print("Dial");
    tft.drawRect(120, 80, 100, 80, WHITE);
    tft.setCursor(125, 85);
    tft.print("<--");
  }
  tft.setCursor(10, 10);
}

uint64_t dial_neonet_adr(){
  render_dial_neonet_adr();
  tft.print("0x");
  int len = 0;
  uint64_t value = 0;
  while(true){
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes

    if((p.x>220)&&(p.y<160)&&(p.y>80)){
      return value;
    }else if((p.x>120)&&(p.x<220)&&(p.y<160)&&(p.y>80)){
      value = value>>4;
      len--;
      
      
      tft.fillRect(0,0,320, 80, BLACK);
      tft.setCursor(10, 10);
      String tmp = String((uint32_t)(value>>32), HEX);
      if(tmp!="0"){
        tft.print("0x"+tmp);
        tmp = String((uint32_t)(value), HEX);
        for(int i=0;i<8-tmp.length();i++)tft.print('0');
      }else{
        tft.print("0x");
        tmp = String((uint32_t)(value), HEX);
      }
      if(tmp!="0")
        tft.print(tmp);
    }else if(p.y>160){

      while(getPoint().z!=0);
      delay(50);
      
      uint64_t x = val_map[p.x/80 + 4*((p.y-160)/80)];
      value = (value<<4) | x;
      len++;
      String tmp = String((int)x, HEX);
      tmp.toUpperCase();
      tft.print(tmp);
    }else{
    }
  }
}

void render_neonet_ctrl_pgm_home(boolean gps, boolean log, int periphs, boolean redraw = true){
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  if(redraw){
    tft.fillScreen(BLACK);
    tft.drawRect(100,100,100,100, WHITE);
    tft.setCursor(105,105);
    tft.print("<- EXIT");
  }
  if(gps){
    tft.drawRect(0,0,100,100, WHITE);
    tft.setCursor(5,5);
    tft.print("GPS");
  }
  if(log){
    tft.drawRect(100,0,100,100, WHITE);
    tft.setCursor(105,5);
    tft.print("LOGS");
  }
  if(periphs&PERIPH_HONKER){
    tft.drawRect(0, 100, 100, 100, WHITE);
    tft.setCursor(5, 105);
    tft.print("HONK");
  }
}

void neonet_ctrl_pgm(String gpsdat, String logdat, uint64_t endpoint, int periphs){
  boolean gps = gpsdat.length()>0;
  boolean log = logdat.length()>0;
  if(!(gps||log||(periphs!=0))){
    popup("Error: no protocols!");
    return;
  }
  render_neonet_ctrl_pgm_home(gps, log, periphs, true);
  
  while(true){

    boolean do_redraw = false;
    
    screenIdleHandler();
    TSPoint p;
    if((p=getPoint()).z==0)continue;
    idleTime=millis()+180*1000; // three minutes
    if(p.x<100&&p.y<100){
      display("Reloading GPS coords...");
      sendNrlPacket(endpoint, NULL, 0, PORT_GPS);
      long timeout = millis()+8000;
      int len = -1;
      while(timeout>millis()){
        len = getNrlPacket(timeout-millis());
        if((nrl_sender==endpoint)&&(nrl_port==PORT_GPS)&&(nrl_target==nrl_adr))break;
        else{
          Serial.println((uint32_t)nrl_sender, HEX);
          Serial.println((uint32_t)nrl_target, HEX);
          Serial.println(nrl_port, HEX);
        }
      }
      if(len==-1){
        popup("Connection Error.");
      }else{
        popup((char*)nrl_data);
      }
      do_redraw = true;
    }else if(p.x>100&&p.y<100&&p.x<200){
      // logs subprogram
      tft.fillScreen(BLACK);
      tft.setTextColor(GREEN);
      tft.setTextSize(1);
      tft.setCursor(0,0);
      tft.print(logdat);
      while(getPoint().z==0){
        screenIdleHandler();
      }
      idleTime=millis()+180*1000; // three minutes
      do_redraw = true;
    }else if(p.y>100&&p.x<100&&p.y<200){
      // honk subprogram
    }else if(p.y>100&&p.x>100&&p.x<200&&p.y<200){
      // exit
      return;
    }
    render_neonet_ctrl_pgm_home(gps, log, periphs, do_redraw);
  }
}
