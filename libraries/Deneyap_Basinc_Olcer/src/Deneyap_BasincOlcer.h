/*
*****************************************************************************
@file         Deneyap_BasincOlcer.h
@mainpage     Deneyap Barometer MS5637-02BA03 Arduino library header file
@version      v1.0.1
@date         September 20, 2022
@brief        This file contains all function prototypes and macros
              for Deneyap Barometer MS5637-02BA03 Arduino library
*****************************************************************************
*/

#ifndef _Deneyap_BasincOlcer_H
#define _Deneyap_BasincOlcer_H

#include "Arduino.h"
#include "Wire.h"

#define BARO_ADDR 0x76 // The default I2C address for the MS5637-02BA03 is 0x76

const uint8_t SamplingDelayMs[6] PROGMEM = {
    2,
    4,
    6,
    10,
    18,
    34};

#define CMD_RESET 0x1E
#define CMD_PROM_READ(offs) (0xA0 + (offs << 1))
#define CMD_START_D1(oversample_level) (0x40 + 2 * (int)oversample_level)
#define CMD_START_D2(oversample_level) (0x50 + 2 * (int)oversample_level)
#define CMD_READ_ADC 0x00

enum BarosampleLevel {
    OSR_256,
    OSR_512,
    OSR_1024,
    OSR_2048,
    OSR_4096,
    OSR_8192
};

typedef enum {
    CELSIUS,
    FAHRENHEIT
} TempUnit;

#define ERR_NOREPLY -1
#define ERR_BAD_READLEN -2
#define ERR_NEEDS_BEGIN -3

class AtmosphericPressure {
public:
    AtmosphericPressure() : initialised(false), err(ERR_NEEDS_BEGIN) {}
    void begin(uint8_t address, TwoWire &wirePort = Wire);
    float getTemp(TempUnit scale = CELSIUS,
                  BarosampleLevel level = OSR_8192);

    float getPressure(BarosampleLevel level = OSR_8192);
    bool getTempAndPressure(float *temperature,
                            float *pressure,
                            TempUnit tempScale = CELSIUS,
                            BarosampleLevel level = OSR_8192);

    inline bool isOK() { return initialised && err == 0; }
    inline byte getError() { return initialised ? err : ERR_NEEDS_BEGIN; }
    void dumpDebugOutput();

private:
    bool initialised;
    int8_t err;
    uint16_t c1, c2, c3, c4, c5, c6;
    uint32_t takeReading(uint8_t trigger_cmd, BarosampleLevel oversample_level);
};

#endif // End of __Deneyap_BasincOlcer_H__ definition check