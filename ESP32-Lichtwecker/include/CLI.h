#ifndef CLI_H_
#define CLI_H_

#include "main.h"

#define CLI_PORT 8001

void CliInit();
int CliWrite(const char *str);

#endif