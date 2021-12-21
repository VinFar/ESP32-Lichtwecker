#include "AlarmTask.h"
#include <Preferences.h>
#include "main.h"
#include "Debug.h"

#define DEBUG_MSG DEBUG_MSG_ALARMS

Preferences prefs;
const char* AlarmPrefsNameSpace = "Alarm";

Alarm_t Alarms[7] = {{Monday, "Montags"}, {Tuesday, "Dienstag"}, {Wednesday, "Mittwoch"}, {Thursday, "Donnerstag"}, {Friday, "Freitag"}, {Saturday, "Samstag"}, {Sunday, "Sonntag"}};

void AlarmGetAlarm(uint8_t idx,Alarm_t *Alarm){

    if(idx >ARRAY_LEN(Alarms)){
        DEBUG_PRINT("Requested IDX out of range" CLI_NL);
    }

}

void AlarmTaskInitPrefs(){

    if(!prefs.begin(AlarmPrefsNameSpace)){
        DEBUG_PRINT("Could not open Prefs Namespace" CLI_NL);
    }

    if(!prefs.getBytes(AlarmPrefsNameSpace,&Alarms,sizeof(Alarms))){
        DEBUG_PRINT("Could not Read Alarms struct" CLI_NL);
    }else{
        DEBUG_PRINT("Read Alarms struct" CLI_NL);
    }

    DEBUG_PRINT("Alarm %s %s" CLI_NL,Alarms[0].WeekDayString,Alarms[0].AlarmOnOff?"on":"off");
    prefs.end();

}


#undef DEBUG_MSG

