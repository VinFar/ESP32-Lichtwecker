#include "AlarmTask.h"
#include <Preferences.h>
#include "main.h"
#include "Debug.h"
#include "myRTC.h"
#include "myLED.h"

#define DEBUG_MSG DEBUG_MSG_ALARMS

Preferences prefs;
const char *AlarmPrefsNameSpace = "Alarm";
const char *AlarmPrefAlarmStatus = "AlarmStatus";
const char *AlarmPrefsLedPowerOffTimer = "LedPowerOff";
TaskHandle_t TaskAlarmTriggeredTaskHandle = NULL;
bool AlarmStatusAll = true;
TaskHandle_t TaskAlarmTaskHandle = NULL;

int AlarmCheckForAlarmTrigger(struct tm CurrentTime, int *AlarmIndex);
static void TaskAlarmTriggered(void *arg);

Alarm_t Alarms[7] = {{Monday, "Montags"}, {Tuesday, "Dienstag"}, {Wednesday, "Mittwoch"}, {Thursday, "Donnerstag"}, {Friday, "Freitag"}, {Saturday, "Samstag"}, {Sunday, "Sonntag"}};
int AlarmIndexes[7];

int AlarmLedOffTimerValue;

void AlarmGetAlarm(uint8_t idx, Alarm_t *Alarm)
{

    if (idx > ARRAY_LEN(Alarms))
    {
        DEBUG_PRINT("Requested IDX out of range" CLI_NL);
    }
}

void AlarmTaskInitPrefs()
{

    prefs.begin(AlarmPrefsNameSpace);
    // prefs.putBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms));
    prefs.getBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms));
    DEBUG_PRINT("Alarm %s %s" CLI_NL, Alarms[0].WeekDayString, Alarms[0].AlarmOnOff ? "on" : "off");
    DEBUG_PRINT("Alarm Rising Time %d min" CLI_NL, Alarms[0].AlarmDuration);
    prefs.end();

    prefs.begin(AlarmPrefsLedPowerOffTimer);
    // prefs.putInt(AlarmPrefsLedPowerOffTimer,15);
    AlarmLedOffTimerValue = prefs.getInt(AlarmPrefsLedPowerOffTimer);
    DEBUG_PRINT("Alarm LED Power OFF Timer %d" CLI_NL, AlarmLedOffTimerValue);
    prefs.end();

    prefs.begin(AlarmPrefAlarmStatus);
    prefs.getBytes(AlarmPrefAlarmStatus, &AlarmStatusAll, sizeof(AlarmStatusAll));
    DEBUG_PRINT("Alarm General Status %s" CLI_NL, AlarmStatusAll ? "on" : "off");
    prefs.end();
}

void TaskAlarm(void *arg)
{
    DEBUG_PRINT("Created Task for Alarm Checking" CLI_NL);
    TaskAlarmTaskHandle = xTaskGetCurrentTaskHandle();
    int AlarmToTrigger;
    struct tm CurrentRtcTime;
    int AlarmIndex;
    while (1)
    {
        RtcGetCurrentTime(&CurrentRtcTime);
        if (AlarmStateGet())
        {
            AlarmToTrigger = AlarmCheckForAlarmTrigger(CurrentRtcTime, &AlarmIndex);
            if (AlarmToTrigger == 1)
            {
                if (TaskAlarmTriggeredTaskHandle == NULL)
                {
                    DEBUG_PRINT("Alarm for %s triggered. Starting Task..." CLI_NL, Alarms[AlarmIndex].WeekDayString);
                    AlarmIndexes[AlarmIndex] = AlarmIndex;
                    xTaskCreatePinnedToCore(TaskAlarmTriggered, "TaskAlarmTriggered", 4000, &AlarmIndexes[AlarmIndex], 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
                }
                else
                {
                    DEBUG_PRINT("Alarm triggered but another alarms is already running" CLI_NL);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(59000));
        }
        else
        {
            DEBUG_PRINT("Alarm Status is set off. Suspending TaskAlarm" CLI_NL);
            vTaskSuspend(NULL);
            DEBUG_PRINT("TaskAlarm resumed" CLI_NL);
        }
    }
}

TaskHandle_t TaskAlarmGetTaskHandle()
{
    return TaskAlarmTaskHandle;
}

static void TaskAlarmTriggered(void *arg)
{
    uint32_t IndexOfAlarmStruct = (uint32_t) * ((uint32_t *)arg);
    float CurrentPwmLedWake = 0.0f;
    float CurrentPwm = 1.0f;
    float MaximumPwm = Alarms[IndexOfAlarmStruct].AlarmMaxLight;
    uint32_t EndTicks = xTaskGetTickCount() + pdMS_TO_TICKS(Alarms[IndexOfAlarmStruct].AlarmDuration * 60 * 1000);
    float PwmToIncreasePerStep = MaximumPwm / (Alarms[IndexOfAlarmStruct].AlarmDuration * 60);

    DEBUG_PRINT("Task started for triggering alarm. Index: %d | Max Power: %d | DC/s: %.4f P/s" CLI_NL, IndexOfAlarmStruct, MaximumPwm, PwmToIncreasePerStep);
    TaskAlarmTriggeredTaskHandle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        LedWakeSetDutyCycle(CurrentPwm);
        CurrentPwm += PwmToIncreasePerStep;
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (xTaskGetTickCount() > EndTicks)
        {
            break;
        }
        DEBUG_PRINT("PWM set to %.4f" CLI_NL, CurrentPwm);
    }
    DEBUG_PRINT("Finished with Rising LED Power. Powering LED for %d mins" CLI_NL, AlarmLedOffTimerGet());
    vTaskDelay(pdMS_TO_TICKS(AlarmLedOffTimerGet() * 60 * 1000));
    LedWakeSetDutyCycle(0.0f);
    DEBUG_PRINT("Alarm is finished! Deleting Task..." CLI_NL);
    TaskAlarmTriggeredTaskHandle = NULL;
    vTaskDelete(NULL);
}

/*
 * Sunday is 0 in tm struct
 */
int AlarmCheckForAlarmTrigger(struct tm CurrentTime, int *AlarmIndex)
{
    DEBUG_PRINT("Checking if Alarm needs to be triggered. Current Time: %02d:%02d" CLI_NL, CurrentTime.tm_hour, CurrentTime.tm_min);
    for (int i = 0; i < ARRAY_LEN(Alarms); i++)
    {
        DEBUG_PRINT("Checking idx %d" CLI_NL, i);
        if (Alarms[i].WeekDay == CurrentTime.tm_wday)
        {
            DEBUG_PRINT("WDay OK" CLI_NL);
            if (Alarms[i].Hour == CurrentTime.tm_hour)
            {
                DEBUG_PRINT("Hours OK" CLI_NL);
                if (Alarms[i].Minute == CurrentTime.tm_min)
                {
                    DEBUG_PRINT("Minutes OK" CLI_NL);
                    if (Alarms[i].AlarmOnOff)
                    {
                        DEBUG_PRINT("Triggered Alarm %s at %02d:%02d" CLI_NL, Alarms[i].WeekDayString, Alarms[i].Hour, Alarms[i].Minute);
                        *AlarmIndex = i;
                        return 1;
                    }
                }
            }
        }
    }
    *AlarmIndex = -1;
    return 0;
}

void AlarmStatusSet(bool NewState)
{
    AlarmStatusAll = NewState;
}

bool AlarmStateGet()
{
    return AlarmStatusAll;
}

bool *AlarmStatusGetAddress()
{
    return &AlarmStatusAll;
}

int8_t AlarmStatusSaveToNvs()
{
    if (!prefs.begin(AlarmPrefAlarmStatus, false))
    {
        DEBUG_PRINT("Could not open NameSpace" CLI_NL);
    }
    if (!prefs.putBytes(AlarmPrefAlarmStatus, &AlarmStatusAll, sizeof(AlarmStatusAll)))
    {
        DEBUG_PRINT("Could not write Namespace %s" CLI_NL, AlarmStatusAll);
    }
    prefs.end();
    DEBUG_PRINT("Wrote Alarm Status to NVS" CLI_NL);
}

void AlarmSetLedPower(uint8_t Index, float DC)
{
    Alarms[Index].AlarmMaxLight = DC;
}

void AlarmSetTimeInterval(uint8_t Index, uint32_t Time)
{
    Alarms[Index].AlarmDuration = Time;
}

void AlarmSetLedOffTimer(int NewValue)
{
    DEBUG_PRINT("LED Power Off Timer set to %d" CLI_NL, NewValue);
    AlarmLedOffTimerValue = NewValue;
}

int AlarmLedOffTimerGet()
{
    return AlarmLedOffTimerValue;
}

void AlarmLedOffTimerSaveToNVS()
{
    prefs.begin(AlarmPrefsLedPowerOffTimer);
    prefs.putInt(AlarmPrefsLedPowerOffTimer, AlarmLedOffTimerValue);
    prefs.end();

    DEBUG_PRINT("LED Power Off Timer saved to NVs" CLI_NL);
}

#undef DEBUG_MSG
