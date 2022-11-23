#ifndef MCP3221_H_
#define MCP3221_H_

#include "Wire.h"

class MCP3221 {
  public:
    explicit MCP3221(uint8_t slave_adr);
    ~MCP3221(void);
    void init(TwoWire * i2c_obj);
    void init();
    uint16_t read();
    float toVoltage(uint16_t data, uint32_t vref);

  protected:
    TwoWire * _i2c;
    const uint8_t _address;
};

#endif
