#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerAlarmTab.h"
#include "AlarmTask.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiAlarmTabName = "Wecker Einstellungen";
static const char *WebUiAlarmOnOffButtonLabel = "Wecker Ein/Aus";
static const char *WebUiAlarmAddAlarm = "Wecker hinzufügen";

Alarm_t Alarms[7] = {{Monday,"Montag"},{Tuesday,"Dienstag"},{Wednesday,"Mittwoch"},{Thursday,"Donnerstag"},{Friday,"Freitag"},{Saturday,"Samstag"},{Sunday,"Sonntag"}};

static void AlarmOnOffSwitchCallback(Control *Button, int value);
static void AlarmAddButtonClickCallback(Control *Button, int type);
static void WebUiAddAlarm(Alarm_t *Alarm);

uint16_t WebUiAlarmTab;
int AlarmOnOffSwitch;

void WebUiAlarmTabInit()
{
  DEBUG_PRINT("Created Alarm Tab" CLI_NL);
  WebUiAlarmTab = ESPUI.addControl(ControlType::Tab, WebUiAlarmTabName, WebUiAlarmTabName);
  ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, "+", ControlColor::Turquoise, WebUiAlarmTab, &AlarmOnOffSwitchCallback);
  for(int i=0;i<ARRAY_LEN(Alarms);i++){
    WebUiAddAlarm(&Alarms[i]);
  }
}

static void AlarmOnOffSwitchCallback(Control *Button, int value)
{
  switch (value)
  {
  case S_ACTIVE:
    DEBUG_PRINT("Alarm on for");
    break;

  case S_INACTIVE:
    DEBUG_PRINT("Alarm off for");
    break;
  }
  for(int i=0;i<ARRAY_LEN(Alarms);i++){
    if(Button->id == Alarms[i].EspUiControl){
      DEBUG_PRINT("%s" CLI_NL,Alarms[i].WeekDayString);
    }
  }
}

static void WebUiAddAlarm(Alarm_t *Alarm){
  DEBUG_PRINT("Created Alarm Tab for %s" CLI_NL,Alarm->WeekDayString);
  uint16_t TabID = ESPUI.addControl(ControlType::Tab, Alarm->WeekDayString, Alarm->WeekDayString);
  Alarm->EspUiControl = ESPUI.addControl(ControlType::Switcher,WebUiAlarmOnOffButtonLabel,WebUiAlarmOnOffButtonLabel,ControlColor::Turquoise,TabID,&AlarmOnOffSwitchCallback);
  //ESPUI.updateSwitcher(Alarm->EspUiControl,true);
}

static void AlarmAddButtonClickCallback(Control *Button, int type)
{
  switch (type)
  {
  case B_DOWN:
    DEBUG_PRINT("Button Alarm add clicked" CLI_NL);
    break;
    default:
    break;
  }
}

#undef DEBUG_MSG