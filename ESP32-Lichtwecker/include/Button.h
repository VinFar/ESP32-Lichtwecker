#ifndef BUTTON_H_
#define BUTTON_H_

#define BUTTON_PIN 25

enum ButtonTimerPwmDirection{
    PWM_INCREASE=1,
    PWM_DECREASE,
};

void ButtonTick();
void ButtonInit();


#endif