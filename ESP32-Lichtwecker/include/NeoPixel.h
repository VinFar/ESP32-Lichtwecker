#ifndef NEOPIXEL_H_
#define NEOPIXEL_H_

#include "main.h"

#define NEOPIXEL_PIN 32

void NeoPixelInit();
void WebUiRGBInit();
void WebUiRGBCreate();
void TaskNeoPixel(void *arg);
void TaskNeoPixelStart();
void NeoPixelTick();
int ButtonNeoPixelSingleClickCallback();
void setAll(byte red, byte green, byte blue);

#endif