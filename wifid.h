#include <WiFi.h>

#include "noturcam.h"

#define MODE_SCAN 0
#define MODE_STA 1
#define MODE_AP 2
#define MODE_OFF 3

void println(String s);
void print(String s);
void mkTask(void (*taskFunc)(void*),char* name, TaskHandle_t* taskVar, int stack);

int wifid_mode = MODE_OFF;
bool enable_thanos = false;
bool is_ap_set_up = false;
bool wifid_running = false;

WiFiServer thanos_server(80);

TaskHandle_t wifid_taskvar;

int netscan_len = 0;

void thanos_serve();
void wifid(void* params);


bool enable_wifi(){
  if(wifid_mode!=MODE_OFF)return true;
  /*if(honker_running){
    println("Error: cannot enable wifi while honker is running.");
    return false;
  }*/
  wifid_mode = MODE_SCAN;
  wifid_running = true;
  mkTask(wifid, "wifid", &wifid_taskvar, 8192);
  return true;
}

void disable_wifi(){
  wifid_mode = MODE_OFF;
}

bool start_thanos(){
  if(wifid_mode!=MODE_OFF){
    disable_wifi();
    while(wifid_running);
  }
  if(!enable_wifi())return false;
  wifid_mode = MODE_AP;

  delay(100);

  println("Thanos server starting...");
  enable_thanos=true;
  thanos_server.begin();
  return true;
}

void stop_thanos(){
  if(!enable_thanos)return;
  enable_thanos = false;
  delay(100);
  thanos_server.end();
}

char* wifid_ssid;
char* wifid_password;

bool wifi_connect(char* ssid, char* password){
  if(!enable_wifi())return false;
  wifid_ssid = ssid;
  wifid_password = password;
  wifid_mode = MODE_STA;
  return true;
}

void wifid(void* params){
  println("wifid starting...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  long timer = 0;
  
  while(wifid_mode!=MODE_OFF){
    if(wifid_mode==MODE_SCAN){
      if(timer<millis()){
        netscan_len = WiFi.scanNetworks();
        timer = millis()+20000;
      }
      delay(20);
    }else{
      if(wifid_mode==MODE_AP){
        if(!is_ap_set_up){
          println("Creating WiFi AP...");
          if(enable_thanos)
            WiFi.softAP("M5Cam-test");
          else
            WiFi.softAP("OpenPDA","opda");
          print("Our IP is ");
          println(String(WiFi.localIP()));
          is_ap_set_up = true;
        }
      }else if(wifid_mode==MODE_STA){
        if((timer<millis())&&(WiFi.status() != WL_CONNECTED)){
          print("wifid: Connecting to ");
          println(wifid_ssid);
          WiFi.begin(wifid_ssid, wifid_password);
          timer = millis()+20000;
        }
      }
      if(enable_thanos){
        thanos_serve();
      }
    }
    delay(20);
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  is_ap_set_up = false;
  println("wifid is stopping...");
  wifid_running = false;
  vTaskDelete(NULL);
}

void thanos_serve()
{
  WiFiClient client = thanos_server.available();
  if (client) 
  {
    print(String("Thanos: ")+millis()+" New Client : ");
    String currentLine = "";
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        //Serial.write(c);
        if (c == '\n') 
        {
          if (currentLine.length() == 0) 
          {
            println("/");
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<style>body{margin: 0}\nimg{height: 100%; width: auto}</style><img src='/camera'>");
            client.println();
            break;
          } 
          else {
            currentLine = "";
          }
        } 
        else if (c != '\r') {
          currentLine += c;
        }
        
        if(currentLine.endsWith("GET /camera")){
          println("/camera");
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:image/jpeg");
          client.println();
          client.write(thanos, thanos_size);
          break;
        }
      }
    }
    // close the connection:
    client.stop();
  }  
}
