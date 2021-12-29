#include "myLED.h"
#include "Debug.h"

#define DEBUG_MSG DEBUG_MSG_LED

float MaxPwmFromTemp = 100.0f;
float CurrentDutyCycle = 0.0f;

void LedWakeInit()
{
    ledcSetup(LED_WAKE_CHANNEL, LED_WAKE_FREQUENCY, LED_WAKE_RESOLUTION);
    ledcAttachPin(LED_WAKE_PIN, LED_WAKE_CHANNEL);

    ledcSetup(LED_WAKE_FAN_CHANNEL, LED_WAKE_FAN_FREQUENCY, LED_WAKE_FAN_RESOLUTION);
    ledcAttachPin(LED_WAKE_FAN_PIN, LED_WAKE_FAN_CHANNEL);
    ledcWrite(1, 1000);
}

void LedWakeSetDutyCycle(float DutyCycle)
{

    if (DutyCycle < 0.0f)
        DutyCycle = 0.0f;
    if (DutyCycle > 100.0f)
        DutyCycle = 100.0f;

    if(DutyCycle > MaxPwmFromTemp)
        DutyCycle = MaxPwmFromTemp;

    CurrentDutyCycle = DutyCycle;
    int DutyCycleValue = (int)((((float)(1 << LED_WAKE_RESOLUTION)) * DutyCycle) / 100.0f);
    ledcWrite(LED_WAKE_CHANNEL, DutyCycleValue);
}

void LedWakeFanSetDutyCycle(float DutyCycle)
{

    if (DutyCycle < 0.0f)
    {
        DutyCycle = 0.0f;
    }
    if (DutyCycle > 100.0f)
    {
        DutyCycle = 100.0f;
    }

    int DutyCycleValue = (int)((((float)(1 << LED_WAKE_FAN_RESOLUTION)) * DutyCycle) / 100.0f);
    ledcWrite(LED_WAKE_FAN_CHANNEL, DutyCycleValue);
}

void LedPwmMaxSet(float MaxPwm)
{
    MaxPwmFromTemp = MaxPwm;
    if(CurrentDutyCycle > MaxPwmFromTemp)
        LedWakeSetDutyCycle(CurrentDutyCycle);
}

#undef DEBUG_MSG