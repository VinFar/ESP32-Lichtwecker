#include "Button.h"
#include "OneButton.h"
#include "main.h"
#include "Debug.h"
#include "AlarmTask.h"
#include "myLED.h"

#define DEBUG_MSG DEBUG_MSG_BUTTON

OneButton button(BUTTON_PIN, true);

static void ButtonDoubleClickCallback();
static void ButtonSingleClick();
static void ButtonLongPressStart();
static void ButtonLongPressEnd();
static void ButtonTimerCallbackFunction(TimerHandle_t xTimer);

TimerHandle_t ButtonTimer;
enum ButtonTimerPwmDirection ButtonPwmCurrentDirection = PWM_INCREASE;
void ButtonInit()
{

    pinMode(BUTTON_PIN, INPUT);

    button.attachDoubleClick(ButtonDoubleClickCallback);
    button.attachClick(ButtonSingleClick);
    button.attachLongPressStart(ButtonLongPressStart);
    button.attachLongPressStop(ButtonLongPressEnd);

    ButtonTimer = xTimerCreate("TimerButtonLongPress", pdMS_TO_TICKS(100), pdTRUE, (void *)0, ButtonTimerCallbackFunction);
    DEBUG_PRINT("Button Init" CLI_NL);
}

static void ButtonLongPressStart()
{
    xTimerStart(ButtonTimer, 0);
}

static void ButtonLongPressEnd()
{
    DEBUG_PRINT("Button released. Stopping Timter" CLI_NL);
    xTimerStop(ButtonTimer, portMAX_DELAY);
}

static void ButtonTimerCallbackFunction(TimerHandle_t xTimer)
{
    float CurrentPwm;
    CurrentPwm = LedPwmGet();
    if (ButtonPwmCurrentDirection == PWM_INCREASE)
    {
        CurrentPwm += 1.0f;
    }
    else
    {
        CurrentPwm -= 1.0f;
    }
    LedWakeSetDutyCycle(CurrentPwm);
    if (CurrentPwm > 100.0f || CurrentPwm < 0.0f)
    {
        if (ButtonPwmCurrentDirection == PWM_INCREASE)
        {
            ButtonPwmCurrentDirection = PWM_DECREASE;
        }
        else
        {
            ButtonPwmCurrentDirection = PWM_INCREASE;
        }
    }
}

void ButtonTick()
{
    button.tick();
}

static void ButtonSingleClick()
{
    DEBUG_PRINT("Button Single Click" CLI_NL);
    if (ButtonSingleClickAlarmCallback())
        return;
    if (ButtonLedSingleClickCallback())
        return;
}

static void ButtonDoubleClickCallback()
{
    DEBUG_PRINT("Button Double Click" CLI_NL);
}

#undef DEBUG_MSG