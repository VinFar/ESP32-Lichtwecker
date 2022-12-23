#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "main.h"
#include "Adafruit_SSD1306.h"

#define I2C_SSD1306_ADDRESS 0x3C

void DisplayInit();
void DisplayTick();
void DisplaySetIPAddress(IPAddress ip);
void DisplaySetTemperature(float Temperature);
void DisplaySetPower(float Power);
void DisplaySetStatus(uint8_t Sinric, uint8_t Wifi, uint8_t Temp);

#endif