#include "main.h"
#include "myRTC.h"
#include "Debug.h"
#include "time.h"
#include "AlarmTask.h"

#define DEBUG_MSG DEBUG_MSG_SNTP

String time_str;
void printLocalTime(struct tm dt);

TaskHandle_t TaskSNTPHandle;
struct tm CurrentRtcTime;
static bool AlarmTaskStarted;

void TaskSNTP(void *arg)
{
    TaskSNTPHandle = xTaskGetCurrentTaskHandle();
    AlarmTaskStarted = false;

    DEBUG_PRINT("Created TaskSNTP" CLI_NL);

    configTzTime(TZ_INFO, "europe.pool.ntp.org");
    while (1)
    {
        if (!getLocalTime(&CurrentRtcTime))
        {
            DEBUG_PRINT("Failed to obtain time. Retrying..." CLI_NL);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        else
        {
            if (AlarmTaskStarted == false)
            {
                AlarmTaskStarted = true;
                xTaskCreatePinnedToCore(TaskAlarm, "TaskAlarm", 4000, NULL, 1, NULL, CONFIG_ARDUINO_UDP_RUNNING_CORE);
            }
            printLocalTime(CurrentRtcTime);
            vTaskDelay(pdMS_TO_TICKS(60000));
        }
    }
}

void RtcGetCurrentTime(struct tm *time)
{
    memcpy(time, &CurrentRtcTime, sizeof(CurrentRtcTime));
}

TaskHandle_t TaskSNTPHandleGet()
{
    return TaskSNTPHandle;
}

void printLocalTime(struct tm dt)
{

    DEBUG_PRINT("Date & time: %02d.%02d.%04d, %02d:%02d:%02d, WDay: %d" CLI_NL, (int)dt.tm_mday,
                (int)dt.tm_mon + 1, (int)dt.tm_year + 1900, (int)dt.tm_hour,
                (int)dt.tm_min, (int)dt.tm_sec, dt.tm_wday);

    return;
}

#undef DEBUG_MSG