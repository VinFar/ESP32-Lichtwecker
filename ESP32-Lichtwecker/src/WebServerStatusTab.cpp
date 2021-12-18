#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerStatusTab.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiStatusTabName= "Wecker Status";

uint16_t WebUiStatusTab;

void WebUiStatusTabInit()
{
  DEBUG_PRINT("Created Status Tab" CLI_NL);
  WebUiStatusTab = ESPUI.addControl(ControlType::Tab,WebUiStatusTabName,WebUiStatusTabName);
}

#undef DEBUG_MSG