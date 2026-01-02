#include "MCP96L01.h"

MCP96L01::MCP96L01() {}

bool MCP96L01::begin(uint8_t i2c_addr, TwoWire *wire) {
    _i2c_addr = i2c_addr;
    _wire = wire;
    return isConnected();
}

bool MCP96L01::isConnected() {
    uint16_t id;
    if (!readRegister16(MCP96L01_REG_DEVICE_ID, id)) return false;
    return ((id >> 8) == 0x41);
}

// --- Status Management ---
uint8_t MCP96L01::getStatus() {
    uint8_t val = 0;
    readRegister8(MCP96L01_REG_STATUS, val);
    return val;
}

void MCP96L01::clearStatusFlags() {
    // Wir schreiben 0, um die R/W Bits 6 & 7 zu nullen.
    writeRegister8(MCP96L01_REG_STATUS, 0x00);
}

bool MCP96L01::isBurstComplete(uint8_t s) { return (s & 0x80); }
bool MCP96L01::isDataUpdated(uint8_t s)    { return (s & 0x40); }
bool MCP96L01::isSensorError(uint8_t s)    { return (s & 0x30); }

// --- Konfiguration ---
void MCP96L01::setThermocoupleType(uint8_t type) {
    updateRegisterBit(MCP96L01_REG_SENSOR_CONFIG, 0x70, 4, type);
}

void MCP96L01::setFilterCoefficient(uint8_t coeff) {
    updateRegisterBit(MCP96L01_REG_SENSOR_CONFIG, 0x07, 0, coeff);
}

void MCP96L01::setADCResolution(uint8_t res) {
    updateRegisterBit(MCP96L01_REG_DEVICE_CONFIG, 0x60, 5, res);
}

void MCP96L01::setBurstSamples(uint8_t samples) {
    updateRegisterBit(MCP96L01_REG_DEVICE_CONFIG, 0x1C, 2, samples);
}

void MCP96L01::setShutdownMode(uint8_t mode) {
    updateRegisterBit(MCP96L01_REG_DEVICE_CONFIG, 0x03, 0, mode);
}

void MCP96L01::enterShutdown() { setShutdownMode(1); }
void MCP96L01::exitShutdown()  { setShutdownMode(0); }
void MCP96L01::startBurst()    { setShutdownMode(2); }

// --- Messwerte ---
float MCP96L01::readHotJunction() {
    uint16_t raw;
    if (!readRegister16(MCP96L01_REG_HOT_JUNCTION, raw)) return NAN;
    return (int16_t)raw * 0.0625;
}

float MCP96L01::readTempDelta() {
    uint16_t raw;
    if (!readRegister16(MCP96L01_REG_TEMP_DELTA, raw)) return NAN;
    return (int16_t)raw * 0.0625;
}

float MCP96L01::readColdJunction() {
    uint16_t raw;
    if (!readRegister16(MCP96L01_REG_COLD_JUNCTION, raw)) return NAN;
    return (int16_t)raw * 0.0625;
}

// --- Interne Helfer ---
void MCP96L01::updateRegisterBit(uint8_t reg, uint8_t mask, uint8_t shift, uint8_t val) {
    uint8_t oldVal;
    if (readRegister8(reg, oldVal)) {
        uint8_t newVal = (oldVal & ~mask) | ((val << shift) & mask);
        writeRegister8(reg, newVal);
    }
}

bool MCP96L01::readRegister8(uint8_t reg, uint8_t &value) {
    _wire->beginTransmission(_i2c_addr);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) return false;
    if (_wire->requestFrom(_i2c_addr, (uint8_t)1) != 1) return false;
    value = _wire->read();
    return true;
}

bool MCP96L01::readRegister16(uint8_t reg, uint16_t &value) {
    _wire->beginTransmission(_i2c_addr);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) return false;
    if (_wire->requestFrom(_i2c_addr, (uint8_t)2) != 2) return false;
    value = (_wire->read() << 8) | _wire->read();
    return true;
}

void MCP96L01::writeRegister8(uint8_t reg, uint8_t value) {
    _wire->beginTransmission(_i2c_addr);
    _wire->write(reg);
    _wire->write(value);
    _wire->endTransmission();
}

void MCP96L01::setAmbientResolution(uint8_t res) {
    updateRegisterBit(MCP96L01_REG_DEVICE_CONFIG, 0x80, 7, res);
}

uint8_t MCP96L01::getThermocoupleType() {
    uint8_t val;
    readRegister8(MCP96L01_REG_SENSOR_CONFIG, val);
    return (val & 0x70) >> 4;
}

uint8_t MCP96L01::getFilterCoefficient() {
    uint8_t val;
    readRegister8(MCP96L01_REG_SENSOR_CONFIG, val);
    return (val & 0x07);
}

uint8_t MCP96L01::getADCResolution() {
    uint8_t val;
    readRegister8(MCP96L01_REG_DEVICE_CONFIG, val);
    return (val & 0x60) >> 5;
}

uint8_t MCP96L01::getBurstSamples() {
    uint8_t val;
    readRegister8(MCP96L01_REG_DEVICE_CONFIG, val);
    return (val & 0x1C) >> 2;
}
