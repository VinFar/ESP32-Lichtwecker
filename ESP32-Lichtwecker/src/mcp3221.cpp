#include <Wire.h>
#include "mcp3221.h"

MCP3221::MCP3221(uint8_t slave_adr):
    _address(slave_adr) {
}

MCP3221::~MCP3221(void) {
    _i2c->~TwoWire();
}

void MCP3221::init() {
    _i2c = &Wire;
}

uint16_t MCP3221::read() {
    _i2c->requestFrom(_address, 2U);

    if (_i2c->available() == 2) {
        return ((_i2c->read() << 8) | (_i2c->read()));
    }

    return 0xFFFF;
}

float MCP3221::toVoltage(uint16_t data, uint32_t vref) {
    return ((float)(vref * data) / 4095.0f);
}
