#include "main.h"
#include "TemperatureSensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ESPUI.h"
#include "WebServerStatusTab.h"

#define DEBUG_MSG DEBUG_MSG_TEMP

//Temperaturesensor is on Pin 14
#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature TemperatureSensor(&oneWire);
DeviceAddress LedTempAddress;
void TaskTemperature(void *arg);
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
        //DEBUG_PRINT("Temperature: %3.2f°C" CLI_NL, CurrentTemp);
        TemperatureString = String(CurrentTemp,2);
        ESPUI.print(WebUiGetTemperatureLabelId(),TemperatureString);
        
        vTaskDelay(pdMS_TO_TICKS(900));
    }
}

#undef DEBUG_MSG