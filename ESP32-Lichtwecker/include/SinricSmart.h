#ifndef SINRIC_SMART_H_
#define SINRIC_SMART_H_

#include "SinricCredentials.h"

// You have to create a header file "SinricCredentials.h" with
// the following macros:
// #define APP_KEY      "xxx"   // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
//#define APP_SECRET    "xxx"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
//#define LIGHT_ID      "xxx"   // Should look like "5dc1564130xxxxxxxxxxxxxx"

void setupSinricPro();
void SinricHandle();

#endif