#ifndef ALARMTASK_H_
#define ALARMTASK_H_

#include "time.h"
#include "main.h"
#include "Preferences.h"

#define SLEEP_TIME_NO_ALARM_S 300

enum WeekDay
{
    Monday = 1,
    Tuesday = 2,
    Wednesday = 3,
    Thursday = 4,
    Friday = 5,
    Saturday = 6,
    Sunday = 0
};

typedef struct
{
    enum WeekDay WeekDay;
    char WeekDayString[15];
    uint8_t Hour;
    uint8_t Minute;
    bool AlarmOnOff;
    float AlarmMaxLight;
    uint32_t AlarmDuration;
    struct EspUiIds
    {
        uint16_t EspUiControlSwitcher;
        uint16_t EspUiControlHour;
        uint16_t EspUiControlMinute;
        uint16_t EspUiControlSaveButton;
    } EspUiId;
} Alarm_t;

extern Alarm_t Alarms[7];
extern Preferences prefs;
extern const char *AlarmPrefsNameSpace;
extern const char *AlarmPrefAlarmStatus;

void AlarmTaskInitPrefs();
void TaskAlarm(void *arg);
bool AlarmStateGet();
void AlarmStatusSet(bool NewState);
bool *AlarmStatusGetAddress();
int8_t AlarmStatusSaveToNvs();
TaskHandle_t TaskAlarmGetTaskHandle();
void AlarmSetLedPower(uint8_t Index,float DC);

#endif