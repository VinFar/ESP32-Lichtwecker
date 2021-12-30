#include "myWifi.h"
#include "IPAddress.h"
#include "Debug.h"
#include "myRTC.h"
#include "WebServerUI.h"

#define DEBUG_MSG DEBUG_MSG_WIFI

static void configModeCallback(AsyncWiFiManager *WifiMan);
static void WifiDisconnectedCallback();
static void WifiConnectedCallback();

wl_status_t WifiStatus;

AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager WifiManager(&server,&dns);

void TaskWifi(void *arg){
    
    DEBUG_PRINT("Created TaskWifi\n");

    WifiManager.setDebugOutput(true);
    WifiManager.autoConnect("Lichtwecker");
    DEBUG_PRINT("Trying to connect...\n");
    WifiManager.setAPCallback(configModeCallback);
    //WifiManager.autoConnect();

    while(1){
        if(WiFi.status()==WL_CONNECTED){
          if(WifiGetStatus()!=WL_CONNECTED){
            WifiConnectedCallback();
          }
        }else{
          if(WifiGetStatus()==WL_CONNECTED){
            WifiDisconnectedCallback();
          }
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

}

wl_status_t WifiGetStatus(){
  return WifiStatus;
}

static void WifiConnectedCallback(){
  WifiStatus=WL_CONNECTED;
  DEBUG_PRINT("WiFi Connected\n");
  DEBUG_PRINT("SSID: %s" CLI_NL,WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  DEBUG_PRINT("IP Address: %d.%d.%d.%d" CLI_NL,ip[0],ip[1],ip[2],ip[3]);
  vTaskDelay(pdMS_TO_TICKS(1000));
  xTaskCreatePinnedToCore(TaskWebUI,"TaskWebUI",4000,NULL,1,NULL,CONFIG_ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TaskSNTP,"TaskSNTP",4000,NULL,1,NULL,CONFIG_ARDUINO_RUNNING_CORE);
}

static void WifiDisconnectedCallback(){
  WifiStatus=WL_DISCONNECTED;
  DEBUG_PRINT("Wifi Disconnected\n");
  DEBUG_PRINT("Starting AutoConnect\n");
  vTaskDelete(TaskSNTPHandleGet());
  WifiManager.autoConnect();
}

static void configModeCallback(AsyncWiFiManager *WifiMan) {
  IPAddress ip = WiFi.softAPIP();
  DEBUG_PRINT("Entered config mode\n");
  DEBUG_PRINT("IP Address: %s\n",ip.toString());
  DEBUG_PRINT("%s\n",WifiMan->getConfigPortalSSID());
}

#undef DEBUG_MSG