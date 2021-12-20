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
    enum WeekDay WeekDay;
    char WeekDayString[15];
    uint8_t Hour;
    uint8_t Minute;
    bool AlarmOnOff;
    float AlarmMaxLight;
    uint32_t AlarmDuration;
    uint16_t EspUiControl;
} Alarm_t;





#endif