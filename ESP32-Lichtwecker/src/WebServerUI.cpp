#include "main.h"
#include "Debug.h"
#include "WebServerUI.h"
#include <DNSServer.h>
#include <ESPUI.h>
#include "IPAddress.h"
#include "myLED.h"
#include "WebServerAlarmTab.h"
#include "WebServerStatusTab.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 0, 81);
DNSServer dnsServer;

void WebUiInitTabs();

void TaskWebUI(void *arg)
{
  DEBUG_PRINT("Created TaskWebUI" CLI_NL);
  // ESPUI.setVerbosity(Verbosity::VerboseJSON);
  dnsServer.start(DNS_PORT, "*", apIP);

  WebUiInitTabs();

  ESPUI.sliderContinuous = true;
  ESPUI.begin("ESPUI Control");
  DEBUG_PRINT("Started WebUI" CLI_NL);

  WebUiAlarmSwitchUpdateAll();

  while (1)
  {
    dnsServer.processNextRequest();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void WebUiInitTabs(){
  WebUiAlarmTabInit();
  WebUiStatusTabInit();
}
#undef DEBUG_MSG
