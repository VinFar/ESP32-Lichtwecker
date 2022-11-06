#ifndef NEOPIXEL_H_
#define NEOPIXEL_H_


#include "Arduino.h"

#define NEOPIXEL_PIN 32

void NeoPixelInit();
void WebUiRGBInit();
void WebUiRGBCreate();
void NeoPixelTick();
int ButtonNeoPixelSingleClickCallback();
void setAll(byte red, byte green, byte blue);
void NeoPixelsetAll(byte red, byte green, byte blue);
void NeoPixelsetPixel(int Pixel, byte red, byte green, byte blue);
void NeoPixelSetBrightness(byte brightness);
void NeoPixelAllOff();
void NeoPixelSetColorForAnimation(uint8_t R, uint8_t G, uint8_t B);
void NeoPixelShowStatusError();
void NeoPixelShowStatusBooting();
void NeoPixelShowStatusWifiConfig();
void NeoPixelShowStatusBootOk();
void NeoPixelBlinkForFeedback(uint8_t R, uint8_t G, uint8_t B);

#endif