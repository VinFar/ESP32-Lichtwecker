#include <Arduino.h>
#include "Debug.h"
#include "Preferences.h"
#include "myWifi.h"
#include "myRTC.h"
#include "myLED.h"

#define DEBUG_MSG DEBUG_MSG_MAIN

float Test = 1.324f;

void setup()
{
  Serial.begin(115200);
  xTaskCreatePinnedToCore(TaskWifi,"TaskWifi",4000,NULL,1,NULL,CONFIG_ARDUINO_RUNNING_CORE);
  LedWakeInit();
  LedWakeSetDutyCycle(27.3f);
  LedWakeFanSetDutyCycle(100.0f);
}


void loop()
{
  // put your main code here, to run repeatedly:
}

#undef DEBUG_MSG