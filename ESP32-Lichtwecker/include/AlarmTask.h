#ifndef ALARMTASK_H_
#define ALARMTASK_H_

#include "time.h"
#include "main.h"
#include "Preferences.h"

enum WeekDay
{
    Monday = 0,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
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
    }EspUiId;
} Alarm_t;

extern Alarm_t Alarms[7];
extern Preferences prefs;
extern const char *AlarmPrefsNameSpace;

void AlarmTaskInitPrefs();

#endif