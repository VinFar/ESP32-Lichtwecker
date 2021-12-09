#ifndef DEBUG_H_
#define DEBUG_H_

#include "main.h"

typedef struct{
	uint8_t Enabled;
	const char DebugPrefix[20];
}DebugPrefix_t;


#define DEBUG_PREFIX_MAIN "[MAIN]"

#define DEBUG_MSG_MAIN (DebugPrefixes[0])

extern DebugPrefix_t DebugPrefixes[1];

size_t DebugPrintFunction(DebugPrefix_t DebugPrefix,
		const char *format, ...); 

#endif