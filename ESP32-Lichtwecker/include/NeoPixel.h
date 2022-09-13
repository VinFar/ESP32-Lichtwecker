#ifndef NEOPIXEL_H_
#define NEOPIXEL_H_

#include "Arduino.h"

#define NEOPIXEL_PIN 32

void NeoPixelInit();
void WebUiRGBInit();
void WebUiRGBCreate();
void TaskNeoPixel(void *arg);
void TaskNeoPixelStart();
void NeoPixelTick();
int ButtonNeoPixelSingleClickCallback();
void NeoPixelsetAll(byte red, byte green, byte blue);
void NeoPixelsetPixel(int Pixel, byte red, byte green, byte blue);
void NeoPixelSetBrightness(byte brightness);


#endif