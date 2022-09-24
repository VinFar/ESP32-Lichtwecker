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
void TaskTemperature(void *arg);
static float TemperatureCalcMaxPWM(float Temp);
static float TemperatureCalcFanPower(float Temp);
static void TempSensorOkCallback();
static void TempSensorNotOkCallback();

float CurrentTemp;
String TemperatureString = "-20.0";
int TempSensorOk=false;

void TempSensorInit()
{

    TemperatureSensor.begin();
    TemperatureSensor.setWaitForConversion(false);

    if (!TemperatureSensor.getAddress(LedTempAddress, 0))
    {
        DEBUG_PRINT("Unable to find address for Device 0");
        TempSensorNotOkCallback();
    }
    else
    {
        DEBUG_PRINT("Found Temperature sensor" CLI_NL);
        TempSensorOkCallback();
    }
    xTaskCreatePinnedToCore(TaskTemperature, "TaskTemperature", 4000, NULL, 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

int TempSensorStatus(){
    return TempSensorOk;
}

static void TempSensorNotOkCallback(){
    DEBUG_PRINT("Error on Temperature Sensor" CLI_NL);
    NeoPixelShowStatusError();
    TempSensorOk=false;
}

static void TempSensorOkCallback(){
    DEBUG_PRINT("Temperature Sensor OK" CLI_NL);
    TempSensorOk=true;
}

void TaskTemperature(void *arg)
{

    DEBUG_PRINT("Created Temperature Task" CLI_NL);
    TemperatureSensor.requestTemperatures();
    while (1)
    {
        // wait for data AND detect disconnect
        while (!TemperatureSensor.isConversionComplete())
        {
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        CurrentTemp = TemperatureSensor.getTempC(LedTempAddress);
        if(CurrentTemp == DEVICE_DISCONNECTED_C){
            if(TempSensorStatus()){
            TempSensorNotOkCallback();
            }
        }else{
        DEBUG_PRINT("Temp: %3.2f°C" CLI_NL,CurrentTemp);
            if(!TempSensorStatus()){
            TempSensorOkCallback();
            }
        }
        
        TemperatureString = String(CurrentTemp, 2);
        if (WebUiIsStarted())
        {
            ESPUI.print(WebUiGetTemperatureLabelId(), TemperatureString);
        }
        float MaxPwm = TemperatureCalcMaxPWM(CurrentTemp);
        float FanPwm = TemperatureCalcFanPower(CurrentTemp);
        LedWakeFanSetDutyCycle(FanPwm);
        LedPwmMaxSet(MaxPwm);
        vTaskDelay(pdMS_TO_TICKS(900));
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
    if(!TempSensorStatus())
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