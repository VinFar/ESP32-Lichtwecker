#include <Arduino.h>
#include "Debug.h"
#include "Preferences.h"

#define DEBUG_MSG DEBUG_MSG_MAIN


Preferences SsidAndPw;

void PrintTest(void *arg)
{
  while (1)
  {
    DEBUG_PRINT("%d\n", 43);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

float Test = 1.324f;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(921600);
  //xTaskCreate(PrintTest,"Test",1000,NULL,1,NULL);

  SsidAndPw.begin("WifiCredentials",false);
  //SsidAndPw.clear();

  Test = SsidAndPw.getFloat("Float1",0);

  DEBUG_PRINT("Float: %f",Test);
  Test+=1.0f;
  DEBUG_PRINT("Float after: %f",Test);
  SsidAndPw.putFloat("Float1",Test);
  DEBUG_PRINT("Float after put: %f",Test);

}


void loop()
{
  // put your main code here, to run repeatedly:
}

#undef DEBUG_MSG