#ifndef MYWIRE_H_
#define MYWIRE_H_

#include "main.h"

void myWireInit();
UBaseType_t myWireSemaphoreGive();
UBaseType_t myWireSemaphoreTake(TickType_t TimeToWait);

#endif