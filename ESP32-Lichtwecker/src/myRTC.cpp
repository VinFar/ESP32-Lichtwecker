#include "main.h"
#include "myRTC.h"
#include "Debug.h"
#include "time.h"

#define DEBUG_MSG DEBUG_MSG_SNTP

String time_str;
void printLocalTime(struct tm dt);

TaskHandle_t TaskSNTPHandle;

void TaskSNTP(void *arg)
{
    TaskSNTPHandle = xTaskGetCurrentTaskHandle();

    DEBUG_PRINT("Created TaskSNTP" CLI_NL);

    configTzTime(TZ_INFO, "europe.pool.ntp.org");
    struct tm dt;
    while (1)
    {
        if (!getLocalTime(&dt))
        {
            DEBUG_PRINT("Failed to obtain time" CLI_NL);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        else
        {
            printLocalTime(dt);
            vTaskDelay(pdMS_TO_TICKS(60000));
        }
    }
}

TaskHandle_t TaskSNTPHandleGet()
{
    return TaskSNTPHandle;
}



void printLocalTime(struct tm dt)
{

    DEBUG_PRINT("Date & time: %02d.%02d.%04d, %02d:%02d:%02d" CLI_NL, (int)dt.tm_mday,
                (int)dt.tm_mon + 1, (int)dt.tm_year + 1900, (int)dt.tm_hour,
                (int)dt.tm_min, (int)dt.tm_sec);

    return;
}

#undef DEBUG_MSG