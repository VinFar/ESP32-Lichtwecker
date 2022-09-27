#include "Debug.h"
#include "CLI.h"

#ifdef DEBUG

DebugPrefix_t DebugPrefixes[12] = {{1, DEBUG_PREFIX_MAIN}, {1, DEBUG_PREFIX_WIFI}, {1, DEBUG_PREFIX_SNTP}, {1, DEBUG_PREFIX_LED},{1, DEBUG_PREFIX_WEBUI},{1,DEBUG_PREFIX_ALARMS},{1,DEBUG_PREFIX_TEMP},{1,DEBUG_PREFIX_BUTTON},{1,DEBUG_PREFIX_NEOPIXEL},{1,DEBUG_PREFIX_SINRIC},{1,DEBUG_PREFIX_OTA},{1,DEBUG_PREFIX_CLI}};

void DebugPrintFunction(DebugPrefix_t DebugPrefix,
                        const char *format, ...)
{
    if (DebugPrefix.Enabled)
    {
        Serial.print(DebugPrefix.DebugPrefix);
        CliWrite(DebugPrefix.DebugPrefix);
        char loc_buf[64];
        char *temp = loc_buf;
        va_list arg;
        va_list copy;
        va_start(arg, format);
        va_copy(copy, arg);
        int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
        va_end(copy);
        if (len < 0)
        {
            va_end(arg);
            return;
        };
        if (len >= sizeof(loc_buf))
        {
            temp = (char *)malloc(len + 1);
            if (temp == NULL)
            {
                va_end(arg);
                return;
            }
            len = vsnprintf(temp, len + 1, format, arg);
        }
        va_end(arg);
        len = Serial.write((uint8_t *)temp, len);
        CliWrite(temp);
        if (temp != loc_buf)
        {
            free(temp);
        }
        return;
    }
    return;
}

#endif