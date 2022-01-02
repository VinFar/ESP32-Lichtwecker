#include "myLED.h"
#include "Debug.h"
#include "WebServerStatusTab.h"
#include "WebServerUI.h"

#define DEBUG_MSG DEBUG_MSG_LED

float MaxPwmFromTemp = 100.0f;
float CurrentDutyCycleOfLed = 0.0f;

bool LedWakeReadingLight = false;

void LedWakeInit()
{
    ledcSetup(LED_WAKE_CHANNEL, LED_WAKE_FREQUENCY, LED_WAKE_RESOLUTION);
    ledcAttachPin(LED_WAKE_PIN, LED_WAKE_CHANNEL);

    ledcSetup(LED_WAKE_FAN_CHANNEL, LED_WAKE_FAN_FREQUENCY, LED_WAKE_FAN_RESOLUTION);
    ledcAttachPin(LED_WAKE_FAN_PIN, LED_WAKE_FAN_CHANNEL);
}

void LedWakeSetDutyCycle(float DutyCycle)
{

    if (DutyCycle < 0.0f)
        DutyCycle = 0.0f;
    if (DutyCycle > 100.0f)
        DutyCycle = 100.0f;

    if (DutyCycle != 0.0f)
        DutyCycle = DutyCycle * 0.90f + 10;

    if (DutyCycle > MaxPwmFromTemp)
        DutyCycle = MaxPwmFromTemp;

    CurrentDutyCycleOfLed = DutyCycle;
    WebUiLedPwmUpdateLabel(DutyCycle);
    DEBUG_PRINT("DutyCycle for PWM Driver %f" CLI_NL, DutyCycle);
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
    if (CurrentDutyCycleOfLed > MaxPwmFromTemp)
        LedWakeSetDutyCycle(CurrentDutyCycleOfLed);
}

float LedPwmGet()
{
    return CurrentDutyCycleOfLed;
}

int8_t ButtonLedSingleClickCallback()
{
    if (!LedWakeReadingLight)
    {
        LedWakeSetDutyCycle(2.0f);
        LedWakeReadingLight = true;
        DEBUG_PRINT("Reading Light on" CLI_NL);
    }
    else
    {
        LedWakeSetDutyCycle(0.0f);
        LedWakeReadingLight = false;
        DEBUG_PRINT("Reading Light off" CLI_NL);
    }
    return 1;
}

#undef DEBUG_MSG