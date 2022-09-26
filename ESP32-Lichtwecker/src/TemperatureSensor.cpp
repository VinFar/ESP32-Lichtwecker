#include "main.h"
#include "TemperatureSensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ESPUI.h"
#include "WebServerStatusTab.h"
#include "myLED.h"
#include "WebServerUI.h"
#include "NeoPixel.h"

#define DEBUG_MSG DEBUG_MSG_TEMP

// Temperaturesensor is on Pin 14
#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature TemperatureSensor(&oneWire);
DeviceAddress LedTempAddress;
TimerHandle_t TempSensorWatchDogTimer=NULL;

void TaskTemperature(void *arg);
static float TemperatureCalcMaxPWM(float Temp);
static float TemperatureCalcFanPower(float Temp);
static void TempSensorOkCallback();
static void TempSensorNotOkCallback();
void TempSensorWatchDogExpiredCallback(TimerHandle_t TimerID);

float CurrentTemp;
String TemperatureString = "-20.0";
int TempSensorOk=false;

void TempSensorInit()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
    oneWire.reset();
    vTaskDelay(pdMS_TO_TICKS(1000));
    TemperatureSensor.begin();
    TemperatureSensor.setWaitForConversion(false);
    TemperatureSensor.setResolution(9);

    if (!TemperatureSensor.getAddress(LedTempAddress, 0))
    {
        DEBUG_PRINT("Unable to find address for Device 0");
        TempSensorOk=false;
    }
    else
    {
        DEBUG_PRINT("Found Temperature sensor" CLI_NL);
        TempSensorOk=true;
    }

    TempSensorWatchDogTimer = xTimerCreate("TempSensorWatchdog",pdMS_TO_TICKS(5000),pdTRUE,(void*)0,TempSensorWatchDogExpiredCallback);
    if(TempSensorWatchDogTimer != NULL){
        xTimerStart(TempSensorWatchDogTimer,pdMS_TO_TICKS(100));
    }else{
        DEBUG_PRINT("Could not create TempSensorWatchdog Timer" CLI_NL);
        TempSensorOk=false;
    }
}

int TempSensorStatus(){
    return TempSensorOk;
}

void TempSensorWatchDogExpiredCallback(TimerHandle_t TimerID){
    DEBUG_PRINT("Watchdog of TempSensor expired" CLI_NL);
    TempSensorNotOkCallback();
}

static void TempSensorNotOkCallback(){
    DEBUG_PRINT("Error on Temperature Sensor" CLI_NL);
    LedWakeSetDutyCycle(0.0f);
    NeoPixelShowStatusError();
    TempSensorOk=false;
    oneWire.reset();
    TemperatureSensor.begin();
}

static void TempSensorOkCallback(){
    DEBUG_PRINT("Temperature Sensor OK" CLI_NL);
    TempSensorOk=true;
}

void TempSensorTick()
{
    static int LastMillis;

    if ((LastMillis + 900) < millis())
    {
        LastMillis = millis();

        CurrentTemp = TemperatureSensor.getTempCByIndex(0);
        if (CurrentTemp != DEVICE_DISCONNECTED_C)
        {
            DEBUG_PRINT("Temperature: %3.2f°C" CLI_NL, CurrentTemp);
            TemperatureString = String(CurrentTemp, 2);
            if (WebUiIsStarted())
            {
                ESPUI.print(WebUiGetTemperatureLabelId(), TemperatureString);
            }
            float MaxPwm = TemperatureCalcMaxPWM(CurrentTemp);
            float FanPwm = TemperatureCalcFanPower(CurrentTemp);
            LedWakeFanSetDutyCycle(FanPwm);
            LedPwmMaxSet(MaxPwm);
            if(TempSensorWatchDogTimer != NULL){
                xTimerReset(TempSensorWatchDogTimer,pdMS_TO_TICKS(100));
            }
        }else{
            DEBUG_PRINT("Could not read Temp Sensor. Resetting oneWire." CLI_NL);
            //oneWire.reset();
        }
        TemperatureSensor.requestTemperatures();
    }
}


/*
 * This function calculates the maximum pwm for the LED for a given temp
 * Above 80.0°C the maximum pwm decreases with 10% per 1°C, so that the
 * maximum temp is somewhere between 80°C and 90°C.
 */
static float TemperatureCalcMaxPWM(float Temp)
{
    if (Temp > 90.0f)
        return 0.0f;
    if (Temp < 0.0f)
        return 50.0f;
    if (!TempSensorStatus())
        return 0.0f;

    float pwm;

    pwm = -10 * Temp + 800;

    if (pwm > 100.0f)
        return 100.0f;
    if (pwm < 0.0f)
        return 0.0f;
    DEBUG_PRINT("Limiting Max PWM to %.2f. Current Temperature: %.2f°C" CLI_NL, pwm, Temp);
    return pwm;
}

static float TemperatureCalcFanPower(float Temp)
{

    float pwm = 3.333 * Temp - 166.666;

    if (pwm < 0)
        pwm = 0.0f;
    if (pwm > 100.0f)
        pwm = 100.0f;

    return pwm;
}

#undef DEBUG_MSG