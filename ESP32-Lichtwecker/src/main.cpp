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
#include "Display.h"
#include "myWire.h"

#define DEBUG_MSG DEBUG_MSG_MAIN

int8_t InitDone = 0;

void TaskInit(void *arg);

void setup()
{
  Serial.begin(1000000);
  xTaskCreatePinnedToCore(TaskInit, "TaskInit", 4000, NULL, 6, NULL, CONFIG_ARDUINO_RUNNING_CORE);
  // ESPUI.prepareFileSystem();
}

void loop()
{
  if(InitDone){
    ButtonTick();
    NeoPixelTick();
    SinricHandle();
    OtaTick();
    TempSensorTick();
    DisplayTick();
  }
}

void TaskInit(void *arg){

  Wire.begin(SDA, SCL);
  myWireInit();
  DisplayInit();
  TempSensorInit();
  NeoPixelInit();
  LedWakeInit();
  LedWakeSetDutyCycle(0);
  LedWakeFanSetDutyCycle(100.0f);
  ButtonInit();
  AlarmTaskInitPrefs();

  xTaskCreatePinnedToCore(TaskWifi, "TaskWifi", 4000, NULL, 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
  TaskTemperatureCreate();
  InitDone = 1;

  vTaskDelete(NULL);

}

#undef DEBUG_MSG