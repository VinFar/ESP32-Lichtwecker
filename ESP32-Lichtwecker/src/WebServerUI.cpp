#include "main.h"
#include "Debug.h"
#include "WebServerUI.h"
#include <DNSServer.h>
#include <ESPUI.h>
#include "IPAddress.h"
#include "myLED.h"
#include "WebServerAlarmTab.h"
#include "WebServerStatusTab.h"
#include "NeoPixel.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 0, 81);
DNSServer dnsServer;

void WebUiInitTabs();
bool EspUiStarted = false;

void TaskWebUI(void *arg)
{
  DEBUG_PRINT("Created TaskWebUI" CLI_NL);
  // ESPUI.setVerbosity(Verbosity::VerboseJSON);
  DEBUG_PRINT("IP: %d.%d.%d.%d" CLI_NL,apIP[0],apIP[1],apIP[2],apIP[3]);
  bool Status = dnsServer.start(DNS_PORT, "*", apIP);
  DEBUG_PRINT("DNS Status %s" CLI_NL, Status ? "OK" : "Fail");
  WebUiInitTabs();

  ESPUI.begin("Lichtwäcker v1.1.0.0");
  // ESPUI.beginLITTLEFS("Lichtwecker");
  // ESPUI.beginSPIFFS("Lichtwecker");
  DEBUG_PRINT("Started WebUI" CLI_NL);
  EspUiStarted = true;

  WebUiAlarmSwitchUpdateAll();

  vTaskDelete(NULL);

}

void DnsServerProcessNextRequest()
{
  if (WebUiIsStarted())
  {
    dnsServer.processNextRequest();
  }
}

bool WebUiIsStarted()
{
  return EspUiStarted;
}

void WebUiInitTabs()
{
  WebUiAlarmTabCreate();
  WebUiRGBCreate();
  WebUiStatusTabCreate();
  WebUiAlarmTabInit();
  WebUiRGBInit();
  WebUiStatusTabInit();
}
#undef DEBUG_MSG
