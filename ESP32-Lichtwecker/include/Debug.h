#ifndef DEBUG_H_
#define DEBUG_H_

#include "main.h"
#include "stdint.h"

typedef struct{
	uint8_t Enabled;
	const char DebugPrefix[20];
}DebugPrefix_t;

#define DEBUG 1

void DebugPrintFunction(DebugPrefix_t DebugPrefix,
		const char *format, ...); 

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) DebugPrintFunction(DEBUG_MSG,fmt,##__VA_ARGS__)
#endif

#define CLI_NL "\r\n"

#define DEBUG_PREFIX_MAIN		"[MAIN] "
#define DEBUG_PREFIX_WIFI		"[WIFI] "
#define DEBUG_PREFIX_SNTP		"[SNTP] "
#define DEBUG_PREFIX_LED		"[LED] "
#define DEBUG_PREFIX_WEBUI		"[WEBUI] "

#define DEBUG_MSG_MAIN (DebugPrefixes[0])
#define DEBUG_MSG_WIFI (DebugPrefixes[1])
#define DEBUG_MSG_SNTP (DebugPrefixes[2])
#define DEBUG_MSG_LED (DebugPrefixes[3])
#define DEBUG_MSG_WEBUI (DebugPrefixes[4])

extern DebugPrefix_t DebugPrefixes[5];


#endif