#include "Debug.h"

#ifdef DEBUG

DebugPrefix_t DebugPrefixes[1] = { { 1, DEBUG_PREFIX_MAIN }};

size_t DebugPrintFunction(DebugPrefix_t DebugPrefix,
		const char *format, ...) {
	if (DebugPrefix.Enabled) {
		Serial.print(DebugPrefix.DebugPrefix);
		    char loc_buf[64];
    char * temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if(len < 0) {
        va_end(arg);
        return 0;
    };
    if(len >= sizeof(loc_buf)){
        temp = (char*) malloc(len+1);
        if(temp == NULL) {
            va_end(arg);
            return 0;
        }
        len = vsnprintf(temp, len+1, format, arg);
    }
    va_end(arg);
    len = Serial.write((uint8_t*)temp, len);
    if(temp != loc_buf){
        free(temp);
    }
    return len;
	}
	return 0;
}

#endif