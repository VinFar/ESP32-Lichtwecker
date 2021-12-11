#ifndef MYRTC_H_
#define MYRTC_H_

#define TZ_INFO "CET-1CEST,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time

void TaskSNTP(void *arg);
TaskHandle_t TaskSNTPHandleGet();

#endif