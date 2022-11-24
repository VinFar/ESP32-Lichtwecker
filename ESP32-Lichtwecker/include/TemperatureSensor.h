#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

#define ADC_I2C_ADDRESS 0b1001110

#define TEMP_ERROR 1042.0f

#define LED_TEMP_MAX 90.0f
#define LED_TEMP_BEGIN 80.0f

#define TASK_TEMPERATURE_TICK 1000

void TempSensorInit();
void TempSensorTick();
int TempSensorStatus();
float TemperatureSensorGetTempC();

#endif