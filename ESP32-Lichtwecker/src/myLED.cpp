#include "myLED.h"
#include "Debug.h"
#include "WebServerStatusTab.h"
#include "WebServerUI.h"
#include "SinricSmart.h"
#include "TemperatureSensor.h"
#include "Display.h"

#define DEBUG_MSG DEBUG_MSG_LED

float MaxPwmFromTemp = 100.0f;
float CurrentDutyCycleOfLedMatched = 0.0f;
float CurrentDutyCycleOfLed = 0.0f;

void LedWakeInit()
{
    ledcSetup(LED_WAKE_CHANNEL, LED_WAKE_FREQUENCY, LED_WAKE_RESOLUTION);
    ledcAttachPin(LED_WAKE_PIN, LED_WAKE_CHANNEL);

    ledcSetup(LED_WAKE_FAN_CHANNEL, LED_WAKE_FAN_FREQUENCY, LED_WAKE_FAN_RESOLUTION);
    ledcAttachPin(LED_WAKE_FAN_PIN, LED_WAKE_FAN_CHANNEL);

    pinMode(LED_WAKE_RELAY_PIN, OUTPUT);
    
}

void LedWakeSetDutyCycle(float DutyCycle)
{

    if (DutyCycle < 0.0f)
        DutyCycle = 0.0f;
    if (DutyCycle > 100.0f)
        DutyCycle = 100.0f;

    DisplaySetPower(DutyCycle);
    WebUiLedPwmUpdateLabel(DutyCycle);
    CurrentDutyCycleOfLed = DutyCycle;
    if (DutyCycle != 0.0f)
        DutyCycle = DutyCycle * 0.90f + 10;

    if (DutyCycle > MaxPwmFromTemp){
        DutyCycle = MaxPwmFromTemp;
        DEBUG_PRINT("Reduced max PWM of LED to: %3.2f due to too high temperature!" CLI_NL,DutyCycle);
    }

    if(!TempSensorStatus()){
        DEBUG_PRINT("Disabling Wake LED due to error on Temp Sensor" CLI_NL);
        DutyCycle=0.0f;
    }

    CurrentDutyCycleOfLedMatched = DutyCycle;
    DEBUG_PRINT("DutyCycles for PWM Driver %3.2f" CLI_NL, DutyCycle);
    if(DutyCycle == 0.0f){
        LED_WAKE_RELAY_OFF;
    }else{
        LED_WAKE_RELAY_ON;
    }
    DutyCycle = 100.0f - DutyCycle;
    int DutyCycleValue = (int)((((float)(1 << LED_WAKE_RESOLUTION)) * DutyCycle) / 100.0f);
    ledcWrite(LED_WAKE_CHANNEL, DutyCycleValue);
}

void LedWakeFanSetDutyCycle(float DutyCycle)
{

    if (DutyCycle < 0.0f)
        DutyCycle = 0.0f;

    if (DutyCycle > 100.0f)
        DutyCycle = 100.0f;

    if (DutyCycle != 0.0f)
        DutyCycle = DutyCycle * 0.70f + 30;

    int DutyCycleValue = (int)((((float)(1 << LED_WAKE_FAN_RESOLUTION)) * DutyCycle) / 100.0f);
    ledcWrite(LED_WAKE_FAN_CHANNEL, DutyCycleValue);
}

void LedPwmMaxSet(float MaxPwm)
{
    MaxPwmFromTemp = MaxPwm;
    if (CurrentDutyCycleOfLedMatched > MaxPwmFromTemp)
        LedWakeSetDutyCycle(CurrentDutyCycleOfLedMatched);
}

float LedPwmGet()
{
    return CurrentDutyCycleOfLed;
}

int8_t ButtonLedSingleClickCallback()
{
    if (LedPwmGet() == 0.0f)
    {
        LedReadLightOn();
        return true;
    }
    else
    {
        LedReadLightOff();
        return false;
    }
    return 1;
}

int8_t LedReadLightOn()
{
    LedWakeSetDutyCycle(2.0f);
    DEBUG_PRINT("Reading Light on" CLI_NL);
    return 0;
}

int8_t LedReadLightOff()
{
    LedWakeSetDutyCycle(0.0f);
    DEBUG_PRINT("Reading Light off" CLI_NL);
    return 0;
}

#undef DEBUG_MSG