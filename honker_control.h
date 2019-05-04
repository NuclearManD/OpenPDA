#include "soc/soc.h"
#include "soc/sens_reg.h"
#include "WiFi.h"
#include <HTTPClient.h>
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

void honker_daemon(void* args){
  while(honker_running){
    if(honkd_timer<millis()){
      if(WiFi.status() != WL_CONNECTED){
        println("honkd connecting...");
        if(!connect_honker()){
          honkd_timer = millis()+30000;
          println("honkd failed to connect!");
        }
      }else{
        println("honkd honking...");
        HTTPClient http;
        http.begin(honkurl+soundnames[random(0,n_soundnames)]);
        http.GET();
        honkd_timer = millis()+random(20000,60000);
      }
    }
    delay(10);
  }
  println("honkd exiting...");
  honkd_timer = 0;
  vTaskDelete(NULL);
}
TaskHandle_t honkd_taskvar;
void startHonkd(){
  //println("Starting honkd...");
  //connect_honker();
  println("Running honkd...");
  mkTask(honker_daemon, "honkd", &honkd_taskvar, 8192);
  honker_running=true;
}
void stopHonkd(){
  honker_running = false;
}
