#ifndef WEBSERVERALARMTAB_H_
#define WEBSERVERALARMTAB_H_

#include <ESPUI.h>
#include "AlarmTask.h"

void slider(Control *sender, int type);
void WebUiAlarmTabInit();
void WebUiAlarmSwitchSetState(const uint16_t Id, const bool newState);
void WebUiAlarmSwitchUpdateAll();
void WebUiAlarmGetInstanceById(uint16_t id, Alarm_t *Alarm);

#endif