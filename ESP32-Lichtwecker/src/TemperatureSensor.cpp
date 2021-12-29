#include "main.h"
#include "TemperatureSensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ESPUI.h"
#include "WebServerStatusTab.h"
#include "myLED.h"
#include "WebServerUI.h"

#define DEBUG_MSG DEBUG_MSG_TEMP

// Temperaturesensor is on Pin 14
#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature TemperatureSensor(&oneWire);
DeviceAddress LedTempAddress;
void TaskTemperature(void *arg);
static float TemperatureCalcMaxPWM(float Temp);
float CurrentTemp;
String TemperatureString = "-20.0";

void TempSensorInit()
{

    TemperatureSensor.begin();

    if (!TemperatureSensor.getAddress(LedTempAddress, 0))
    {
        DEBUG_PRINT("Unable to find address for Device 0");
    }
    else
    {
        DEBUG_PRINT("Found Temperature sensor" CLI_NL);
    }
    xTaskCreatePinnedToCore(TaskTemperature, "TaskTemperature", 4000, NULL, 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void TaskTemperature(void *arg)
{

    DEBUG_PRINT("Created Temperature Task" CLI_NL);

    while (1)
    {
        TemperatureSensor.requestTemperatures();

        // wait for data AND detect disconnect
        while (!TemperatureSensor.isConversionComplete())
        {
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        CurrentTemp = TemperatureSensor.getTempC(LedTempAddress);
        // DEBUG_PRINT("Temperature: %3.2f°C" CLI_NL, CurrentTemp);
        TemperatureString = String(CurrentTemp, 2);
        if (WebUiIsStarted())
        {
            ESPUI.print(WebUiGetTemperatureLabelId(), TemperatureString);
        }
        float MaxPwm = TemperatureCalcMaxPWM(CurrentTemp);
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

    float pwm;

    pwm = -10 * Temp + 900;

    if (pwm > 100.0f)
        return 100.0f;
    if (pwm < 0.0f)
        return 0.0f;
    DEBUG_PRINT("Limiting Max PWM to %.2f. Current Temperature: %.2f°C" CLI_NL, pwm, Temp);
    return pwm;
}

#undef DEBUG_MSG