#include <Arduino.h>
#include "Debug.h"
#include "Preferences.h"
#include "myWifi.h"
#include "myRTC.h"
#include "myLED.h"
#include "ESPUI.h"
#include "AlarmTask.h"
#include "TemperatureSensor.h"


#define DEBUG_MSG DEBUG_MSG_MAIN

float Test = 1.324f;

void setup()
{
  Serial.begin(115200);
  LedWakeInit();
  LedWakeSetDutyCycle(0);
  LedWakeFanSetDutyCycle(100.0f);
  TempSensorInit();
  AlarmTaskInitPrefs();
  xTaskCreatePinnedToCore(TaskWifi,"TaskWifi",4000,NULL,1,NULL,CONFIG_ARDUINO_RUNNING_CORE);
  
}


void loop()
{
  // put your main code here, to run repeatedly:
}

#undef DEBUG_MSG