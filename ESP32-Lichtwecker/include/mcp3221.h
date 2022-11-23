#ifndef MCP3221_H_
#define MCP3221_H_

#include "Wire.h"

class MCP3221 {
  public:
    explicit MCP3221(uint8_t slave_adr);
    ~MCP3221(void);
    void init(uint32_t SampleRate, uint32_t BufferLen);
    uint16_t read();
    uint16_t readMean();
    float toVoltage(uint16_t data, uint32_t vref);
    void setSampleRate(float Freq);
    void tick();
    uint16_t readFromDevice();

  protected:
    TwoWire * _i2c;
    const uint8_t _address;
    uint32_t _LastMillis;
    uint32_t _MillisInterval;
    float *_ResMeanBuffer;
    uint32_t _BufferLen;
    uint32_t _ResMeanBufferIdx;
    uint16_t _LastResult;

};

#endif
