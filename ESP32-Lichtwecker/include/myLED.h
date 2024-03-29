#ifndef MYLED_H_
#define MYLED_H_

#include "main.h"

#define LED_WAKE_PIN 13
#define LED_WAKE_FREQUENCY 200
#define LED_WAKE_CHANNEL 0
#define LED_WAKE_RESOLUTION 12

#define LED_WAKE_FAN_PIN 12
#define LED_WAKE_FAN_FREQUENCY 20000
#define LED_WAKE_FAN_CHANNEL 8
#define LED_WAKE_FAN_RESOLUTION 12

#define LED_WAKE_RELAY_PIN 15
#define LED_WAKE_RELAY_ON digitalWrite(LED_WAKE_RELAY_PIN, HIGH)
#define LED_WAKE_RELAY_OFF digitalWrite(LED_WAKE_RELAY_PIN, LOW)

void LedWakeInit();
void LedWakeSetDutyCycle(float DutyCycle);
void LedWakeFanSetDutyCycle(float DutyCycle);
void LedPwmMaxSet(float MaxPwm);
float LedPwmGet();
int8_t ButtonLedSingleClickCallback();
int8_t LedReadLightOn();
int8_t LedReadLightOff();

#endif