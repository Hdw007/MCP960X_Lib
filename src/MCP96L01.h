#ifndef MCP96L01_H
#define MCP96L01_H

#include "Arduino.h"
#include <Wire.h>

// Register Adressen
#define MCP96L01_REG_HOT_JUNCTION 0x00
#define MCP96L01_REG_TEMP_DELTA 0x01
#define MCP96L01_REG_COLD_JUNCTION 0x02
#define MCP96L01_REG_STATUS 0x04
#define MCP96L01_REG_SENSOR_CONFIG 0x05
#define MCP96L01_REG_DEVICE_CONFIG 0x06
#define MCP96L01_REG_DEVICE_ID 0x20

// Status-Register Bits
#define MCP96L01_STATUS_BURST_COMPLETE (1 << 7)
#define MCP96L01_STATUS_TH_UPDATE (1 << 6)
#define MCP96L01_STATUS_SHORT_CIRCUIT (1 << 5)
#define MCP96L01_STATUS_OPEN_CIRCUIT (1 << 4)

// Thermoelement Typen (Register 0x05, Bits 6-4)
#define MCP96L01_TYPE_K 0x0
#define MCP96L01_TYPE_J 0x1
#define MCP96L01_TYPE_T 0x2
#define MCP96L01_TYPE_N 0x3
#define MCP96L01_TYPE_S 0x4
#define MCP96L01_TYPE_E 0x5
#define MCP96L01_TYPE_B 0x6
#define MCP96L01_TYPE_R 0x7

// ADC Auflösung (Register 0x06, Bits 6-5)
#define MCP96L01_RES_18BIT 0x0  // 0.0625°C
#define MCP96L01_RES_16BIT 0x1  // 0.25°C
#define MCP96L01_RES_14BIT 0x2  // 1.0°C
#define MCP96L01_RES_12BIT 0x3  // 4.0°C

// Kaltstellen-Auflösung (Register 0x06, Bit 7)
#define MCP96L01_AMBIENT_RES_0_0625 0x0
#define MCP96L01_AMBIENT_RES_0_25   0x1

// Shutdown Modes (Register 0x06, Bits 1-0)
#define MCP96L01_MODE_NORMAL   0x0
#define MCP96L01_MODE_SHUTDOWN 0x1
#define MCP96L01_MODE_BURST    0x2

// Filter Coefficients (Register 0x05, Bits 2-0)
#define MCP96L01_FILTER_OFF 0
#define MCP96L01_FILTER_MIN 2
#define MCP96L01_FILTER_MID 4
#define MCP96L01_FILTER_MAX 7

// Burst Samples (Register 0x06, Bits 4-2)
#define MCP96L01_BURST_1    0x0
#define MCP96L01_BURST_2    0x1
#define MCP96L01_BURST_4    0x2
#define MCP96L01_BURST_8    0x3
#define MCP96L01_BURST_16   0x4
#define MCP96L01_BURST_32   0x5
#define MCP96L01_BURST_64   0x6
#define MCP96L01_BURST_128  0x7

class MCP96L01
{
public:
    MCP96L01();
    bool begin(uint8_t i2c_addr, TwoWire *wire = &Wire);
    bool isConnected();

    // Auslesen
    float readHotJunction();
    float readColdJunction();
    float readTempDelta();
    uint8_t getThermocoupleType();
    uint8_t getFilterCoefficient();
    uint8_t getADCResolution();
    uint8_t getBurstSamples();

    // Optimiertes Status-Management
    uint8_t getStatus();     // Liest das Byte nur einmal ein
    void clearStatusFlags();
    bool isBurstComplete(uint8_t s);
    bool isDataUpdated(uint8_t s);
    bool isSensorError(uint8_t s);

    // Konfiguration
    void setThermocoupleType(uint8_t type);
    void setFilterCoefficient(uint8_t coeff);
    void setADCResolution(uint8_t res);
    void setShutdownMode(uint8_t mode);
    void setBurstSamples(uint8_t samples);
    void setAmbientResolution(uint8_t res);

    // Hilfsfunktionen für Betriebsmodi
    void enterShutdown();
    void exitShutdown();
    void startBurst();

private:
    uint8_t _i2c_addr;
    TwoWire *_wire;
    bool readRegister8(uint8_t reg, uint8_t &val);
    bool readRegister16(uint8_t reg, uint16_t &val);
    void writeRegister8(uint8_t reg, uint8_t val);
    void updateRegisterBit(uint8_t reg, uint8_t mask, uint8_t shift, uint8_t val);
};

#endif
