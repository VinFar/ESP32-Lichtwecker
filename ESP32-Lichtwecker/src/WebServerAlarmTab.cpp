#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerAlarmTab.h"
#include "AlarmTask.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiAlarmTabName = "Wecker Einstellungen";
static const char *WebUiAlarmOnOffButtonLabel = "Wecker Ein/Aus";
static const char *WebUiAlarmAddAlarm = "Wecker hinzufügen";
static const char *WebUiAlarmHoursLabel = "Stunde";
static const char *WebUiAlarmMinuteLabel = "Minute";

static void AlarmOnOffSwitchCallback(Control *Button, int value);
static void AlarmAddButtonClickCallback(Control *Button, int type);
static void WebUiAddAlarm(Alarm_t *Alarm);
static void AlarmNumberInputCallback(Control *Select, int type);

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
    if (id == Alarms[i].EspUiId.EspUiControlHour || id == Alarms[i].EspUiId.EspUiControlMinute || id == Alarms[i].EspUiId.EspUiControlSwitcher)
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
  Alarm->EspUiId.EspUiControlSwitcher = ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, WebUiAlarmOnOffButtonLabel, ControlColor::Turquoise, TabID, &AlarmOnOffSwitchCallback);

  uint16_t numberId = ESPUI.addControl(ControlType::Number, "WebUiAlarmHoursLabel", "7", ControlColor::Turquoise, TabID, &AlarmNumberInputCallback);
  ESPUI.addControl(ControlType::Min, "WebUiAlarmHoursLabel", "0", ControlColor::None, numberId);
  ESPUI.addControl(ControlType::Max, "WebUiAlarmHoursLabel", "24", ControlColor::None, numberId);
  Alarm->EspUiId.EspUiControlHour = numberId;

  numberId = ESPUI.addControl(ControlType::Number, "Minute", "7", ControlColor::Turquoise, TabID, &AlarmNumberInputCallback);
  ESPUI.addControl(ControlType::Min, "Minute", "0", ControlColor::None, numberId);
  ESPUI.addControl(ControlType::Max, "Minute", "60", ControlColor::None, numberId);
  Alarm->EspUiId.EspUiControlMinute = numberId;
}

void WebUiAlarmSwitchSetState(const Alarm_t Alarm, const bool newState)
{
  DEBUG_PRINT("Updated Switcher %s to %d" CLI_NL, Alarm.WeekDayString, newState);
  ESPUI.updateSwitcher(Alarm.EspUiId.EspUiControlSwitcher, Alarm.AlarmOnOff);
}

void WebUiAlarmSwitchUpdateAll()
{
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    WebUiAlarmSwitchSetState(Alarms[i], Alarms[i].AlarmOnOff);
  }
}

static void AlarmNumberInputCallback(Control *Select, int type)
{
  uint8_t idx = WebUiAlarmGetIndexById(Select->id);
  if (idx > ARRAY_LEN(Alarms))
  {
    return;
  }
  if (strcmp(Select->label, WebUiAlarmHoursLabel) == 0)
  {
    Alarms[idx].Hour = type;
  }
  else if (strcmp(Select->label, WebUiAlarmMinuteLabel) == 0)
  {
    Alarms[idx].Minute = type;
  }
  else
  {
    return;
  }

  prefs.begin(AlarmPrefsNameSpace);
  prefs.putBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms));
  prefs.end();

  DEBUG_PRINT("Alarm %s for %s set to %d. IDX: %d" CLI_NL, Select->label, Alarms[idx].WeekDayString, Alarms[idx].Hour, idx);
}

#undef DEBUG_MSG