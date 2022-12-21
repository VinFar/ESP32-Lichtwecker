#include <Wire.h>
#include "mcp3221.h"
#include <Arduino.h>
#include "myWire.h"

MCP3221::MCP3221(uint8_t slave_adr) : _address(slave_adr)
{
}

MCP3221::~MCP3221(void)
{
}

void MCP3221::init(uint32_t SampleRate, uint32_t BufferLen)
{
    _i2c = &Wire;
    _LastMillis = millis();
    _MillisInterval = 1000 / SampleRate;
    _BufferLen = BufferLen;
    _ResMeanBuffer = new float[_BufferLen];
    _ResMeanBufferIdx = 0;

    memset(_ResMeanBuffer, 0, sizeof(*_ResMeanBuffer));
}

uint16_t MCP3221::read()
{
    return _LastResult;
}

uint16_t MCP3221::readFromDevice(){
    
    uint16_t rv = 0xFFFF;

    if(myWireSemaphoreTake(pdMS_TO_TICKS(100)) == pdTRUE){

        _i2c->requestFrom(_address, 2U);

        while(_i2c->available()<2);

        if (_i2c->available() == 2) {
            rv = ((_i2c->read() << 8) | (_i2c->read()));
        }
        myWireSemaphoreGive();
    }
    return rv;
}

uint16_t MCP3221::readMean()
{
    uint32_t sum = 0;
    for (int i = 0; i < _BufferLen; i++)
    {
        sum += (uint32_t)_ResMeanBuffer[i];
    }
    uint16_t mean = sum / _BufferLen;
    return mean;
}

float MCP3221::toVoltage(uint16_t data, uint32_t vref)
{
    return ((float)((uint32_t)vref * data) / 4095.0f);
}

void MCP3221::tick()
{

    if ((_LastMillis + _MillisInterval) < millis())
    {
        _LastMillis = millis();
        uint16_t result = readFromDevice();

        _LastResult = result;
        _ResMeanBuffer[_ResMeanBufferIdx++] = result;
        if (_ResMeanBufferIdx > _BufferLen)
        {
            _ResMeanBufferIdx = 0;
        }
    }
}
