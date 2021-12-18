#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerAlarmTab.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiAlarmTabName = "Wecker Einstellungen";
static const char *WebUiAlarmOnOffButtonLabel = "Wecker Ein/Aus";
static const char *WebUiAlarmAddAlarm = "Wecker hinzufügen";

static void AlarmOnOffSwitchCallback(Control *Button, int value);
static void AlarmAddButtonClickCallback(Control *Button, int type);
static void WebUiAddAlarm();

uint16_t WebUiAlarmTab;
int AlarmOnOffSwitch;

void WebUiAlarmTabInit()
{
  DEBUG_PRINT("Created Alarm Tab" CLI_NL);
  WebUiAlarmTab = ESPUI.addControl(ControlType::Tab, WebUiAlarmTabName, WebUiAlarmTabName);
  ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, "+", ControlColor::Turquoise, WebUiAlarmTab, &AlarmOnOffSwitchCallback);
  ESPUI.button(WebUiAlarmAddAlarm, &AlarmAddButtonClickCallback, ControlColor::Alizarin);
  ESPUI.addControl(ControlType::Button, WebUiAlarmAddAlarm, "+", ControlColor::Turquoise, WebUiAlarmTab, &AlarmAddButtonClickCallback);
}

static void AlarmOnOffSwitchCallback(Control *Button, int value)
{
  switch (value)
  {
  case S_ACTIVE:
    DEBUG_PRINT("Alarm on" CLI_NL);
    break;

  case S_INACTIVE:
    DEBUG_PRINT("Alarm off" CLI_NL);
    break;
  }
}

static void WebUiAddAlarm(){
  DEBUG_PRINT("Created Second Alarm Tab" CLI_NL);
  WebUiAlarmTab = ESPUI.addControl(ControlType::Tab, "Test", "Test");
  ESPUI.jsonReload();

}

static void AlarmAddButtonClickCallback(Control *Button, int type)
{
  switch (type)
  {
  case B_DOWN:
    DEBUG_PRINT("Button Alarm add clicked" CLI_NL);
    WebUiAddAlarm();
    break;
    default:
    break;
  }
}

#undef DEBUG_MSG