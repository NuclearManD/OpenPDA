#include "soc/soc.h"
#include "soc/sens_reg.h"
#include "WiFi.h"
#include "esp_wifi.h"


void println(String s);
void print(String s);
void mkTask(void (*taskFunc)(void*),char* name, TaskHandle_t* taskVar, int stack);

boolean connect_honker(){
  println("connect_honker:Attempting connection...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  if(n==0){
    delay(1000);
    n = WiFi.scanNetworks();
  }
  if(n==0){
    println("connect_honker:No WiFi networks; Abort");
    return false;
  }
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    String name = String(WiFi.SSID(i));
    if(name.startsWith("Honker")){
      println("connect_honker:Connecting to "+name);
      WiFi.begin(name.c_str());
      long timeout = millis()+100000;
      while (WiFi.status() != WL_CONNECTED) {
        delay(50);
        if(timeout<millis()){
          println("connect_honker:Connect failed; Abort");
          return false;
        }
      }
      println("connect_honker:Connected.");
      return true;
    }
    //delay(10);
  }
  println("connect_honker:No valid networks; Abort");
  return false;
}

boolean honker_running = false;

String honkurl = "http://honk.com/";

char* soundnames[] = {"honk", "wiz", "revz", "pkfire", "drinkweed"};
int n_soundnames = 5;

long honkd_timer = 0;

void honker_daemon(){
  /*while(honker_running&&honkd_timer<millis()){
    if(WiFi.status() != WL_CONNECTED){
      if(!connect_honker());//{
        honkd_timer = millis()+30000;
      //}
    }else{
      HTTPClient http;
      http.begin(honkurl+soundnames[random(0,n_soundnames)]);
      http.GET();
      honkd_timer = millis()+random(20000,60000);
    }
  }*/
}

void startHonkd(){
  println("Starting honkd...");
  println("Running honkd...");
  connect_honker();
  //WiFi.disconnect();
  //WiFi.mode(WIFI_OFF);
  
  /*HTTPClient http;
  http.begin(honkurl+soundnames[random(0,n_soundnames)]);
  http.GET();
  //WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  println("Honking done.");*/
  
  //honker_running=true;
}
void stopHonkd(){
  println("honkd terminated.");
  honker_running = false;
}
