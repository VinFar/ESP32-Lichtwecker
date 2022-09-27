#include <Arduino.h>
#include "Debug.h"
#include "Preferences.h"
#include "myWifi.h"
#include "myRTC.h"
#include "myLED.h"
#include "ESPUI.h"
#include "AlarmTask.h"
#include "TemperatureSensor.h"
#include "WebServerUI.h"
#include "Button.h"
#include "NeoPixel.h"
#include "SinricSmart.h"
#include "OTA.h"

#define DEBUG_MSG DEBUG_MSG_MAIN

float Test = 1.324f;

void setup()
{
  Serial.begin(1000000);
  NeoPixelInit();
  LedWakeInit();
  LedWakeSetDutyCycle(0);
  LedWakeFanSetDutyCycle(100.0f);
  TempSensorInit();
  ButtonInit();
  AlarmTaskInitPrefs();
  
  xTaskCreatePinnedToCore(TaskWifi,"TaskWifi",4000,NULL,1,NULL,CONFIG_ARDUINO_RUNNING_CORE);

  // ESPUI.prepareFileSystem();
}

void loop()
{
  ButtonTick();
  NeoPixelTick();
  SinricHandle();
  OtaTick();
  TempSensorTick();
}

#undef DEBUG_MSG