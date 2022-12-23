#include "main.h"
#include "TemperatureSensor.h"
#include "ESPUI.h"
#include "WebServerStatusTab.h"
#include "myLED.h"
#include "WebServerUI.h"
#include "NeoPixel.h"
#include <Wire.h>
#include "mcp3221.h"
#include "myWire.h"
#include "Display.h"

#define DEBUG_MSG DEBUG_MSG_TEMP

MCP3221 mcp3221(ADC_I2C_ADDRESS);
const uint8_t address = ADC_I2C_ADDRESS;
const uint16_t ref_voltage = 4086;  // in mV

TimerHandle_t TempSensorWatchDogTimer=NULL;

void TaskTemperature(void *arg);
static float TemperatureCalcMaxPWM(float Temp);
static float TemperatureCalcFanPower(float Temp);
static void TempSensorOkCallback();
static void TempSensorNotOkCallback();
void TempSensorWatchDogExpiredCallback(TimerHandle_t TimerID);
void TemperatureSensorRead();

float CurrentTemp;
String TemperatureString = "-20.00";
int TempSensorOk=false;

void TempSensorInit()
{
    if(myWireSemaphoreTake(pdMS_TO_TICKS(1000)) == pdTRUE){
        DEBUG_PRINT("Init MCP 3221" CLI_NL);
        mcp3221.init(20, 60);
        myWireSemaphoreGive();
    }

}

void TaskTemperatureCreate(){
    xTaskCreatePinnedToCore(TaskTemperature, "TaskTemperature", 8000, NULL, 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    TempSensorWatchDogTimer = xTimerCreate("TempSensorWatchdog",pdMS_TO_TICKS(5000),pdTRUE,(void*)0,TempSensorWatchDogExpiredCallback);
    if(TempSensorWatchDogTimer != NULL){
        xTimerStart(TempSensorWatchDogTimer,pdMS_TO_TICKS(100));
    }else{
        DEBUG_PRINT("Could not create TempSensorWatchdog Timer" CLI_NL);
        TempSensorOk=false;
    }
}

void TaskTemperature(void *arg)
{
    while(1){
        TemperatureSensorRead();
        vTaskDelay(TASK_TEMPERATURE_TICK);
    }
}

float TemperatureSensorGetTempC(){
    uint16_t result = mcp3221.readMean();

    return (mcp3221.toVoltage(result, ref_voltage) / 10.0f) - 273.15f;
}

void TemperatureSensorRead(){

    CurrentTemp = TemperatureSensorGetTempC();
    if (CurrentTemp != TEMP_ERROR)
    {
        //DEBUG_PRINT("Temperature: %3.2f°C" CLI_NL, CurrentTemp);
        DisplaySetTemperature(CurrentTemp);
        TemperatureString = String(CurrentTemp, 2);
        if (WebUiIsStarted())
        {
            ESPUI.print(WebUiGetTemperatureLabelId(), TemperatureString);
        }
        float MaxPwm = TemperatureCalcMaxPWM(CurrentTemp);
        float FanPwm = TemperatureCalcFanPower(CurrentTemp);
        LedWakeFanSetDutyCycle(FanPwm);
        LedPwmMaxSet(MaxPwm);
        TempSensorOk=true;
        if(TempSensorWatchDogTimer != NULL){
            xTimerReset(TempSensorWatchDogTimer,pdMS_TO_TICKS(100));
        }
    }else{
        DEBUG_PRINT("Could not read Temp Sensor. Resetting oneWire." CLI_NL);
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
}

static void TempSensorOkCallback(){
    DEBUG_PRINT("Temperature Sensor OK" CLI_NL);
    TempSensorOk=true;
}

void TempSensorTick()
{
    mcp3221.tick();
}


static float TemperatureCalcMaxPWM(float Temp)
{
    if (Temp >115.0f)
        return 0.0f;
    if (Temp < 0.0f)
        return 50.0f;
    if (!TempSensorStatus())
        return 0.0f;

    float pwm;

    pwm = -9 * Temp + 1000;

    if (pwm > 100.0f)
        return 100.0f;
    if (pwm < 0.0f)
        return 0.0f;
    DEBUG_PRINT("Limiting Max PWM to %.2f. Current Temperature: %.2f°C" CLI_NL, pwm, Temp);
    return pwm;
}

static float TemperatureCalcFanPower(float Temp)
{

    float pwm = 3.333 * Temp - 266.666;

    if (pwm < 0)
        pwm = 0.0f;
    if (pwm > 100.0f)
        pwm = 100.0f;

    return pwm;
}

#undef DEBUG_MSG