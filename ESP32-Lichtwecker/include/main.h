#ifndef MAIN_H_
#define MAIN_H_

#include "stdint.h"
#include <Arduino.h>

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_MSG "[DEF] "

#define DEBUG_PRINT(fmt, ...) DebugPrintFunction(DEBUG_MSG,fmt,##__VA_ARGS__)
#endif

#endif