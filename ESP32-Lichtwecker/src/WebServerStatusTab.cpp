#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerStatusTab.h"
#include "myLED.h"
#include "WebServerUI.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiStatusTabName = "Wecker Status";

uint16_t WebUiStatusTab;
uint16_t WebUiTemperatureLabelID;
uint16_t WebUiDutyCycleLedLabelId;

void WebUiStatusTabInit()
{
  DEBUG_PRINT("Created Status Tab" CLI_NL);
  WebUiStatusTab = ESPUI.addControl(ControlType::Tab, WebUiStatusTabName, WebUiStatusTabName);

  WebUiTemperatureLabelID = ESPUI.addControl(ControlType::Label, "Temperature (°C)", "0.0", ControlColor::Alizarin, WebUiStatusTab);
  String LedPwm = String(LedPwmGet(), 2);
  WebUiDutyCycleLedLabelId = ESPUI.addControl(ControlType::Label, "LED Power (%)", LedPwm, ControlColor::Alizarin, WebUiStatusTab);
}

uint16_t WebUiGetTemperatureLabelId()
{
  return WebUiTemperatureLabelID;
}

void WebUiLedPwmUpdateLabel(float DutyCycle)
{
  if (WebUiIsStarted())
  {
    DutyCycle = (DutyCycle - 10) / 0.90f;
    if (DutyCycle > 100.0f)
      DutyCycle = 100.0f;
    
    if (DutyCycle < 0.0f)
      DutyCycle = 0.0f;
    
    String DcString = String(DutyCycle, 2);
    ESPUI.print(WebUiDutyCycleLedLabelId, DcString);
  }
}

#undef DEBUG_MSG