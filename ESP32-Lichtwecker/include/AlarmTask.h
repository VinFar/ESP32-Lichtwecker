#ifndef ALARMTASK_H_
#define ALARMTASK_H_

#include "time.h"
#include "main.h"

enum WeekDay{
    Monday=0,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

typedef struct
{
    const enum WeekDay WeekDay;
    const char WeekDayString[15];
    uint8_t Hour;
    uint8_t Minute;
    uint8_t AlarmOnOff;
    float AlarmMaxLight;
    uint32_t AlarmDuration;
    uint16_t EspUiControl;
} Alarm_t;





#endif