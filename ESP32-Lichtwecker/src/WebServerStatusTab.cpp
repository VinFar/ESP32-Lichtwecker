#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerStatusTab.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiStatusTabName= "Wecker Status";

uint16_t WebUiStatusTab;
uint16_t WebUiTemperatureLabelID;

void WebUiStatusTabInit()
{
  DEBUG_PRINT("Created Status Tab" CLI_NL);
  WebUiStatusTab = ESPUI.addControl(ControlType::Tab,WebUiStatusTabName,WebUiStatusTabName);

  WebUiTemperatureLabelID=ESPUI.addControl(ControlType::Label,"Temperature (°C)","0.0",ControlColor::Alizarin,WebUiStatusTab);
}

uint16_t WebUiGetTemperatureLabelId(){
  return WebUiTemperatureLabelID;
}

#undef DEBUG_MSG