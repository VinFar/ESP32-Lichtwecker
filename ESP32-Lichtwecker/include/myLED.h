#ifndef MYLED_H_
#define MYLED_H_

#define LED_WAKE_PIN 13
#define LED_WAKE_FREQUENCY 200
#define LED_WAKE_CHANNEL 0
#define LED_WAKE_RESOLUTION 12

#define LED_WAKE_FAN_PIN 12
#define LED_WAKE_FAN_FREQUENCY 1000
#define LED_WAKE_FAN_CHANNEL 1
#define LED_WAKE_FAN_RESOLUTION 12

void LedWakeInit();
void LedWakeSetDutyCycle(float DutyCycle);
void LedWakeFanSetDutyCycle(float DutyCycle);
void LedPwmMaxSet(float MaxPwm);

#endif