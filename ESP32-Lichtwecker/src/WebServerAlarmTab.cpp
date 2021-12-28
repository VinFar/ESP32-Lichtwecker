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
static const char *WebUiAlarmSaveTime = "Weckzeit speichern";

static void AlarmOnOffSwitchCallback(Control *Button, int value);
static void AlarmAddButtonClickCallback(Control *Button, int type);
static void WebUiAddAlarm(Alarm_t *Alarm);
static void AlarmNumberInputCallback(Control *Select, int type);
static void AlarmStatusSwitchCallback(Control *Button, int value);

uint16_t WebUiAlarmTab;
uint16_t WebUiAlarmStatusSwitcherId;
int AlarmOnOffSwitch;

void WebUiAlarmTabInit()
{
  DEBUG_PRINT("Created Alarm Tab" CLI_NL);

  WebUiAlarmTab = ESPUI.addControl(ControlType::Tab, WebUiAlarmTabName, WebUiAlarmTabName);
  WebUiAlarmStatusSwitcherId=ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, "", ControlColor::Turquoise, WebUiAlarmTab, &AlarmStatusSwitchCallback);
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    WebUiAddAlarm(&Alarms[i]);
  }
}

static void AlarmOnOffSwitchCallback(Control *Button, int value)
{
  int8_t idx = WebUiAlarmGetIndexById(Button->id);
  if (idx < 0)
  {
    return;
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
  default:

    break;
  }
  if (!prefs.begin(AlarmPrefsNameSpace, false))
  {
    DEBUG_PRINT("Could not open NameSpace" CLI_NL);
  }
  if (!prefs.putBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms)))
  {
    DEBUG_PRINT("Could not write Namespace %s" CLI_NL, AlarmPrefsNameSpace);
  }
  prefs.end();
}

static void AlarmStatusSwitchCallback(Control *Button, int value)
{

  switch (value)
  {
  case S_ACTIVE:
    AlarmStatusSet(true);
    DEBUG_PRINT("Alarm Status on. Resuming TaskAlarm..." CLI_NL);
    vTaskResume(TaskAlarmGetTaskHandle());
    break;

  case S_INACTIVE:
    AlarmStatusSet(false);
    DEBUG_PRINT("Alarm Status off" CLI_NL);
    break;
  default:

    break;
  }
  AlarmStatusSaveToNvs();
}

int8_t WebUiAlarmGetIndexById(uint16_t id)
{
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    if (id == Alarms[i].EspUiId.EspUiControlHour || id == Alarms[i].EspUiId.EspUiControlMinute || id == Alarms[i].EspUiId.EspUiControlSwitcher || id == Alarms[i].EspUiId.EspUiControlSaveButton)
    {
      return i;
    }
  }
  return -1;
}

static void WebUiAddAlarm(Alarm_t *Alarm)
{
  DEBUG_PRINT("Created Alarm Tab for %s" CLI_NL, Alarm->WeekDayString);
  DEBUG_PRINT("Alarm for %s set to %02d:%02dh. Alarm %s" CLI_NL, Alarm->WeekDayString, Alarm->Hour, Alarm->Minute, Alarm->AlarmOnOff ? "on" : "off");
  uint16_t TabID = ESPUI.addControl(ControlType::Tab, Alarm->WeekDayString, Alarm->WeekDayString);
  Alarm->EspUiId.EspUiControlSwitcher = ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, WebUiAlarmOnOffButtonLabel, ControlColor::Turquoise, TabID, &AlarmOnOffSwitchCallback);

  char TimeString[3];
  itoa(Alarm->Hour, TimeString, 10);
  uint16_t numberId = ESPUI.addControl(ControlType::Number, WebUiAlarmHoursLabel, TimeString, ControlColor::Turquoise, TabID, &AlarmNumberInputCallback);
  ESPUI.addControl(ControlType::Min, WebUiAlarmHoursLabel, "0", ControlColor::None, numberId);
  ESPUI.addControl(ControlType::Max, WebUiAlarmHoursLabel, "23", ControlColor::None, numberId);
  Alarm->EspUiId.EspUiControlHour = numberId;

  itoa(Alarm->Minute, TimeString, 10);
  numberId = ESPUI.addControl(ControlType::Number, "Minute", TimeString, ControlColor::Turquoise, TabID, &AlarmNumberInputCallback);
  ESPUI.addControl(ControlType::Min, "Minute", "0", ControlColor::None, numberId);
  ESPUI.addControl(ControlType::Max, "Minute", "59", ControlColor::None, numberId);
  Alarm->EspUiId.EspUiControlMinute = numberId;

  Alarm->EspUiId.EspUiControlSaveButton = ESPUI.addControl(ControlType::Button, WebUiAlarmSaveTime, "Speichern", ControlColor::Turquoise, TabID, &AlarmNumberInputCallback);
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
  ESPUI.updateSwitcher(WebUiAlarmStatusSwitcherId, AlarmStateGet());
}

static void AlarmNumberInputCallback(Control *Select, int type)
{
  uint8_t idx = WebUiAlarmGetIndexById(Select->id);
  bool SavePrefs = false;
  if (idx > ARRAY_LEN(Alarms))
  {
    return;
  }
  if (strcmp(Select->label, WebUiAlarmHoursLabel) == 0)
  {
    Alarms[idx].Hour = Select->value.toInt();
  }
  else if (strcmp(Select->label, WebUiAlarmMinuteLabel) == 0)
  {
    Alarms[idx].Minute = Select->value.toInt();
  }
  else if (strcmp(Select->label, WebUiAlarmSaveTime) == 0 && type == B_DOWN)
  {
    SavePrefs = true;
  }
  else
  {
    return;
  }
  if (SavePrefs)
  {
    DEBUG_PRINT("Saved Alarm time to NVS" CLI_NL);
    DEBUG_PRINT("Alarm for %s set to %02d:%02d. IDX: %d" CLI_NL, Alarms[idx].WeekDayString, Alarms[idx].Hour, Alarms[idx].Minute, idx);
    prefs.begin(AlarmPrefsNameSpace);
    prefs.putBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms));
    prefs.end();
  }
  else
  {
  }
}

#undef DEBUG_MSG