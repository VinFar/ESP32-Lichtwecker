#include "AlarmTask.h"
#include <Preferences.h>
#include "main.h"
#include "Debug.h"
#include "myRTC.h"
#include "myLED.h"
#include "NeoPixel.h"

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
static void AlarmTaskReadStruct(Alarm_t *AlarmPtr, size_t size);
static int AlarmTaskReadLedPowerOffTimer();
static bool AlarmTaskReadAlarmStatusAll();

Alarm_t Alarms[7] = {{Monday, "Montags"}, {Tuesday, "Dienstag"}, {Wednesday, "Mittwoch"}, {Thursday, "Donnerstag"}, {Friday, "Freitag"}, {Saturday, "Samstag"}, {Sunday, "Sonntag"}};
int AlarmIndexes[7];

int AlarmLedOffTimerValue;

void AlarmTaskInitPrefs()
{
    AlarmTaskReadStruct(Alarms, sizeof(Alarms));

    AlarmLedOffTimerValue = AlarmTaskReadLedPowerOffTimer();
    AlarmStatusAll = AlarmTaskReadAlarmStatusAll();
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
                    xTaskCreatePinnedToCore(TaskAlarmTriggered, "TaskAlarmTriggered", 4000, &AlarmIndexes[AlarmIndex], 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
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
    float CurrentPwm = 1.0f;
    float MaximumPwm = Alarms[IndexOfAlarmStruct].AlarmMaxLight;
    uint32_t EndTicks = xTaskGetTickCount() + pdMS_TO_TICKS(Alarms[IndexOfAlarmStruct].AlarmDuration * 60 * 1000);
    float PwmToIncreasePerStep;
    if(Alarms[IndexOfAlarmStruct].AlarmDuration==0.0f){
        PwmToIncreasePerStep=100.0f;
        CurrentPwm=100.0f;
    }else{
        PwmToIncreasePerStep = MaximumPwm / (Alarms[IndexOfAlarmStruct].AlarmDuration * 60);
    }

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

int8_t ButtonSingleClickAlarmCallback()
{
    if (TaskAlarmTriggeredTaskHandle != NULL)
    {
        vTaskDelete(TaskAlarmTriggeredTaskHandle);
        TaskAlarmTriggeredTaskHandle = NULL;
        LedWakeSetDutyCycle(0.0f);
        DEBUG_PRINT("Button was pressed while Alarm was running. Aborted the Alarm" CLI_NL);
        return 1;
    }
    return 0;
}

/*
 * Sunday is 0 in tm struct
 */
int AlarmCheckForAlarmTrigger(struct tm CurrentTime, int *AlarmIndex)
{
    DEBUG_PRINT("Checking if Alarm needs to be triggered. Current Time: %02d:%02d" CLI_NL, CurrentTime.tm_hour, CurrentTime.tm_min);
    for (int i = 0; i < ARRAY_LEN(Alarms); i++)
    {
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
        NeoPixelShowStatusError();
        return -1;
    }
    if (!prefs.putBytes(AlarmPrefAlarmStatus, &AlarmStatusAll, sizeof(AlarmStatusAll)))
    {
        DEBUG_PRINT("Could not write Namespace %s" CLI_NL, AlarmStatusAll);
        NeoPixelShowStatusError();
        return -1;
    }
    prefs.end();
    DEBUG_PRINT("Wrote Alarm Status to NVS" CLI_NL);
    return 0;
}

void AlarmSetLedPower(uint8_t Index, float DC)
{
    Alarms[Index].AlarmMaxLight = DC;
}

void AlarmSetTimeInterval(uint8_t Index, uint32_t Time)
{
    if(Time > 60)
        Time = 60;
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
    // prefs.putInt(AlarmPrefsLedPowerOffTimer, AlarmLedOffTimerValue);
    prefs.end();
    NeoPixelBlinkForFeedback(0,255,0);

    DEBUG_PRINT("LED Power Off Timer saved to NVs" CLI_NL);
}

static void AlarmTaskReadStruct(Alarm_t *AlarmPtr, size_t size)
{

    prefs.begin(AlarmPrefsNameSpace);
    // prefs.putBytes(AlarmPrefsNameSpace, &Alarms, size);
    if(!prefs.getBytes(AlarmPrefsNameSpace, AlarmPtr, size)){
        NeoPixelShowStatusError();
    }
    DEBUG_PRINT("Alarm %s %s" CLI_NL, AlarmPtr[0].WeekDayString, AlarmPtr[0].AlarmOnOff ? "on" : "off");
    DEBUG_PRINT("Alarm Rising Time %d min" CLI_NL, AlarmPtr[0].AlarmDuration);
    prefs.end();
}

static int AlarmTaskReadLedPowerOffTimer()
{
    prefs.begin(AlarmPrefsLedPowerOffTimer);
    // prefs.putInt(AlarmPrefsLedPowerOffTimer,15);
    int ret = AlarmLedOffTimerValue = prefs.getInt(AlarmPrefsLedPowerOffTimer);
    DEBUG_PRINT("Alarm LED Power OFF Timer %d" CLI_NL, AlarmLedOffTimerValue);
    prefs.end();

    return ret;
}

static bool AlarmTaskReadAlarmStatusAll()
{

    bool ret;

    prefs.begin(AlarmPrefAlarmStatus);
    // prefs.putBytes(AlarmPrefAlarmStatus, &ret, sizeof(ret));
    if(!prefs.getBytes(AlarmPrefAlarmStatus, &ret, sizeof(ret))){
        NeoPixelShowStatusError();
    }
    DEBUG_PRINT("Alarm General Status %s" CLI_NL, ret ? "on" : "off");
    prefs.end();
    return ret;
}

#undef DEBUG_MSG
