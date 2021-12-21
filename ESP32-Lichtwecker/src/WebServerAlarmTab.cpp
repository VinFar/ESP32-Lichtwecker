#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerAlarmTab.h"
#include "AlarmTask.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiAlarmTabName = "Wecker Einstellungen";
static const char *WebUiAlarmOnOffButtonLabel = "Wecker Ein/Aus";
static const char *WebUiAlarmAddAlarm = "Wecker hinzufügen";

static void AlarmOnOffSwitchCallback(Control *Button, int value);
static void AlarmAddButtonClickCallback(Control *Button, int type);
static void WebUiAddAlarm(Alarm_t *Alarm);
static void AlarmSelectedHours(Control *Select, int type);

uint16_t WebUiAlarmTab;
int AlarmOnOffSwitch;

void WebUiAlarmTabInit()
{
  DEBUG_PRINT("Created Alarm Tab" CLI_NL);
  WebUiAlarmTab = ESPUI.addControl(ControlType::Tab, WebUiAlarmTabName, WebUiAlarmTabName);
  ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, "+", ControlColor::Turquoise, WebUiAlarmTab, &AlarmOnOffSwitchCallback);
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    WebUiAddAlarm(&Alarms[i]);
  }
}

static void AlarmOnOffSwitchCallback(Control *Button, int value)
{
  uint8_t idx = WebUiAlarmGetIndexById(Button->id);
  if (!prefs.begin(AlarmPrefsNameSpace, false))
  {
    DEBUG_PRINT("Could not open NameSpace" CLI_NL);
  }
  switch (value)
  {
  case S_ACTIVE:
    Alarms[idx].AlarmOnOff = 1;
    DEBUG_PRINT("Alarm %s for %s" CLI_NL, Alarms[idx].AlarmOnOff ? "on" : "off", Alarms[idx].WeekDayString);
    break;

  case S_INACTIVE:
    Alarms[idx].AlarmOnOff = 0;
    DEBUG_PRINT("Alarm %s for %s" CLI_NL, Alarms[idx].AlarmOnOff ? "on" : "off", Alarms[idx].WeekDayString);
    break;
  }
  if (!prefs.putBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms)))
  {
    DEBUG_PRINT("Could not write Namespace %s" CLI_NL, AlarmPrefsNameSpace);
  }
  prefs.end();
}

uint8_t WebUiAlarmGetIndexById(uint16_t id)
{
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    if (id == Alarms[i].EspUiControl)
    {
      return i;
    }
  }
  return 0;
}

static void WebUiAddAlarm(Alarm_t *Alarm)
{
  DEBUG_PRINT("Created Alarm Tab for %s" CLI_NL, Alarm->WeekDayString);
  uint16_t TabID = ESPUI.addControl(ControlType::Tab, Alarm->WeekDayString, Alarm->WeekDayString);
  Alarm->EspUiControl = ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, WebUiAlarmOnOffButtonLabel, ControlColor::Turquoise, TabID, &AlarmOnOffSwitchCallback);

  uint16_t HoursSelect = ESPUI.addControl(ControlType::Select, "Stunde", "", ControlColor::Turquoise, TabID, &AlarmSelectedHours);
  char HourString[3];
  for (int i = 1; i <= 12; i++)
  {
    itoa(i, HourString, 10);
    ESPUI.addControl(ControlType::Option, "12", "12", ControlColor::Alizarin, HoursSelect);
  }
}

void WebUiAlarmSwitchSetState(const Alarm_t Alarm, const bool newState)
{
  DEBUG_PRINT("Updated Switcher %s to %d" CLI_NL, Alarm.WeekDayString, newState);
  ESPUI.updateSwitcher(Alarm.EspUiControl, Alarm.AlarmOnOff);
}

void WebUiAlarmSwitchUpdateAll()
{
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    WebUiAlarmSwitchSetState(Alarms[i], Alarms[i].AlarmOnOff);
  }
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

static void AlarmSelectedHours(Control *Select, int type)
{

  uint8_t idx = WebUiAlarmGetIndexById(Select->id);

  Alarms[idx].Hour = type;

  DEBUG_PRINT("Alarm hour for %s set to %d" CLI_NL, Alarms[idx].WeekDayString, Alarms[idx].Hour);

  prefs.begin(AlarmPrefsNameSpace);
  prefs.putBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms));
  prefs.end();
}

#undef DEBUG_MSG