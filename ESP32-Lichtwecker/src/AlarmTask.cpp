#include "AlarmTask.h"
#include <Preferences.h>
#include "main.h"
#include "Debug.h"
#include "myRTC.h"

#define DEBUG_MSG DEBUG_MSG_ALARMS

Preferences prefs;
const char *AlarmPrefsNameSpace = "Alarm";
const char *AlarmPrefAlarmStatus = "AlarmStatus";
TaskHandle_t TaskAlarmTriggeredTaskHandle = NULL;
bool AlarmStatusAll = true;
TaskHandle_t TaskAlarmTaskHandle = NULL;

int AlarmCheckForAlarmTrigger(struct tm CurrentTime, int *AlarmIndex);
static void TaskAlarmTriggered(void *arg);

Alarm_t Alarms[7] = {{Monday, "Montags"}, {Tuesday, "Dienstag"}, {Wednesday, "Mittwoch"}, {Thursday, "Donnerstag"}, {Friday, "Freitag"}, {Saturday, "Samstag"}, {Sunday, "Sonntag"}};
int AlarmIndexes[7];

void AlarmGetAlarm(uint8_t idx, Alarm_t *Alarm)
{

    if (idx > ARRAY_LEN(Alarms))
    {
        DEBUG_PRINT("Requested IDX out of range" CLI_NL);
    }
}

void AlarmTaskInitPrefs()
{

    if (!prefs.begin(AlarmPrefsNameSpace))
    {
        DEBUG_PRINT("Could not open Prefs Namespace" CLI_NL);
    }

    // if(!prefs.putBytes(AlarmPrefsNameSpace,&Alarms,sizeof(Alarms))){
    //     DEBUG_PRINT("Could not write Alarms struct" CLI_NL);
    // }else{
    //     DEBUG_PRINT("Wrote Alarms struct" CLI_NL);
    // }

    if (!prefs.getBytes(AlarmPrefsNameSpace, &Alarms, sizeof(Alarms)))
    {
        DEBUG_PRINT("Could not Read Alarms struct" CLI_NL);
    }
    else
    {
        DEBUG_PRINT("Read Alarms struct" CLI_NL);
    }
    DEBUG_PRINT("Alarm %s %s" CLI_NL, Alarms[0].WeekDayString, Alarms[0].AlarmOnOff ? "on" : "off");
    prefs.end();

    if (!prefs.begin(AlarmPrefAlarmStatus))
    {
        DEBUG_PRINT("Could not open Prefs Namespace" CLI_NL);
    }

    // if (!prefs.putBytes(AlarmPrefAlarmStatus, &AlarmStatusAll, sizeof(AlarmStatusAll)))
    // {
    //     DEBUG_PRINT("Could not write Alarms struct" CLI_NL);
    // }
    // else
    // {
    //     DEBUG_PRINT("Wrote Alarm status" CLI_NL);
    // }

    if (!prefs.getBytes(AlarmPrefAlarmStatus, &AlarmStatusAll, sizeof(AlarmStatusAll)))
    {
        DEBUG_PRINT("Could not Read Alarms Status" CLI_NL);
    }
    else
    {
        DEBUG_PRINT("Read Alarms Status" CLI_NL);
    }

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
    DEBUG_PRINT("Task started for triggering alarm" CLI_NL);
    float CurrentPwmLedWake = 0.0f;
    TaskAlarmTriggeredTaskHandle = xTaskGetCurrentTaskHandle();
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
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

void AlarmSetLedPower(uint8_t Index,float DC){
    Alarms[Index].AlarmMaxLight = DC;
}

#undef DEBUG_MSG
