#include "main.h"
#include "Debug.h"
#include <ESPUI.h>
#include "WebServerAlarmTab.h"
#include "AlarmTask.h"
#include "myLED.h"

#define DEBUG_MSG DEBUG_MSG_WEBUI

static const char *WebUiAlarmTabName = "Wecker Einstellungen";
static const char *WebUiAlarmOnOffButtonLabel = "Wecker Ein/Aus";
static const char *WebUiAlarmAddAlarm = "Wecker hinzufügen";
static const char *WebUiAlarmHoursLabel = "Stunde";
static const char *WebUiAlarmMinuteLabel = "Minute";
static const char *WebUiAlarmSaveTime = "Weckzeit speichern";
static const char *WebUiAlarmTimeIntervall = "Zeit Intervall zum Wecken (min)";
static const char *WebUiAlarmLedPower = "Lichtstärke zum Wecken (%)";
static const char *WebUiAlarmLedOffTimeLabel ="LED Einschaltzeit nach Wecker (min)";

static void
AlarmOnOffSwitchCallback(Control *Button, int value);
static void AlarmAddButtonClickCallback(Control *Button, int type);
static void WebUiAddAlarm(Alarm_t *Alarm);
static void AlarmNumberInputCallback(Control *Select, int type);
static void AlarmStatusSwitchCallback(Control *Button, int value);
static void AlarmLedPowerSliderCallback(Control *Select, int type);
static void AlarmLedPowerTimeoutCallback(TimerHandle_t xTimer);
static void AlarmLedTimeIntervallCallback(Control *Select, int type);
static void AlarmLedOffTimerNumberCallback(Control *Select,int type);

uint16_t WebUiAlarmTab;
uint16_t WebUiAlarmStatusSwitcherId;
uint16_t WebUiAlarmLedPowerSliderId;
uint16_t WebUiAlarmTimeIntervalId;
uint16_t WebUiAlarmLedPowerOffNumberId;
TimerHandle_t TimerLedPowerTimeoutHandle;
int AlarmOnOffSwitch;

void WebUiAlarmTabInit()
{
  DEBUG_PRINT("Created Alarm Tab" CLI_NL);

  WebUiAlarmTab = ESPUI.addControl(ControlType::Tab, WebUiAlarmTabName, WebUiAlarmTabName);
  WebUiAlarmStatusSwitcherId = ESPUI.addControl(ControlType::Switcher, WebUiAlarmOnOffButtonLabel, "", ControlColor::Turquoise, WebUiAlarmTab, &AlarmStatusSwitchCallback);
  String LedPowerString = String(Alarms[0].AlarmMaxLight,0);
  WebUiAlarmLedPowerSliderId = ESPUI.addControl(ControlType::Slider, WebUiAlarmLedPower, LedPowerString, ControlColor::Alizarin, WebUiAlarmTab, &AlarmLedPowerSliderCallback);
  WebUiAlarmTimeIntervalId= ESPUI.addControl(ControlType::Number, WebUiAlarmTimeIntervall, "", ControlColor::Alizarin, WebUiAlarmTab, &AlarmLedTimeIntervallCallback);
  WebUiAlarmLedPowerOffNumberId = ESPUI.addControl(ControlType::Number,WebUiAlarmLedOffTimeLabel,"",ControlColor::Alizarin,WebUiAlarmTab,&AlarmLedOffTimerNumberCallback);
  

  TimerLedPowerTimeoutHandle = xTimerCreate("LedPowerTimeoutTimer", pdMS_TO_TICKS(5000), 0, (void *)0, AlarmLedPowerTimeoutCallback);

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
  AlarmPrefsSaveToNvs();
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
  ESPUI.updateNumber(WebUiAlarmTimeIntervalId,Alarms[0].AlarmDuration);
  ESPUI.updateNumber(WebUiAlarmLedPowerOffNumberId,AlarmLedOffTimerGet());
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
    AlarmPrefsSaveToNvs();
  }
  else
  {
  }
}

static void AlarmLedPowerSliderCallback(Control *Select, int type)
{
  DEBUG_PRINT("LED Power Slider set to %d" CLI_NL, Select->value.toInt());
  xTimerReset(TimerLedPowerTimeoutHandle, 0);
  LedWakeSetDutyCycle(Select->value.toFloat());
  // ESPUI.updateSlider(WebUiAlarmLedPowerSliderId,Select->value.toInt());
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    AlarmSetLedPower(i, Select->value.toFloat());
  }
}

static void AlarmLedTimeIntervallCallback(Control *Select, int type)
{
  uint32_t value = Select->value.toInt();
  DEBUG_PRINT("Wake Light Time Interval set to %d min" CLI_NL,value);
  ESPUI.updateNumber(Select->id,value);
  for (int i = 0; i < ARRAY_LEN(Alarms); i++)
  {
    AlarmSetTimeInterval(i, value);
  }
  AlarmPrefsSaveToNvs();
}

static void AlarmLedOffTimerNumberCallback(Control *Select,int type){
  int ValueFromSlider = Select->value.toInt();
  ESPUI.updateNumber(Select->id,ValueFromSlider);
  AlarmSetLedOffTimer(ValueFromSlider);
  AlarmLedOffTimerSaveToNVS();
}

static void AlarmLedPowerTimeoutCallback(TimerHandle_t xTimer)
{
  LedWakeSetDutyCycle(0.0f);
  DEBUG_PRINT("LED PWM reset to 0%" CLI_NL);
  xTimerStop(xTimer, portMAX_DELAY);
  AlarmPrefsSaveToNvs();
}

void AlarmPrefsSaveToNvs()
{
  DEBUG_PRINT("Saved Alarms struct to NVS" CLI_NL);
  prefs.begin(AlarmPrefsNameSpace);
  prefs.putBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms));
  prefs.end();
}

#undef DEBUG_MSG