/*
*****************************************************************************
@file         Deneyap_BasincOlcer.cpp
@mainpage     Deneyap Barometer MS5637-02BA03 Arduino library source file
@maintainer   RFtek Electronics <techsupport@rftek.com.tr>
@version      v1.0.1
@date         September 20, 2022
@brief        Includes functions to control Deneyap Barometer MS5637-02BA03
              Arduino library

Library includes:
--> Configuration functions
--> Data manipulation functions
--> I2C communication functions
*****************************************************************************
*/

#include "Deneyap_BasincOlcer.h"

/**
 * @brief
 * @param  None
 * @retval
 **/
inline static int8_t _endTransmission(bool stop = true) {
    Wire.endTransmission();
#ifdef __AVR__
    int8_t res = Wire.endTransmission(stop);
    return res;
#else
    return 0;
#endif
}

/**
 * @brief  I2C initialization and read check
 * @param
 * @retval None
 **/
void AtmosphericPressure::begin(uint8_t address, TwoWire &wirePort) {
    Wire.begin();
    Wire.beginTransmission(BARO_ADDR);
    Wire.write(CMD_RESET);
    err = _endTransmission();
    if (err)
        return;

    uint16_t prom[7];
    for (int i = 0; i < 7; i++) {
        Wire.beginTransmission(BARO_ADDR);
        Wire.write(CMD_PROM_READ(i));
        err = _endTransmission(false);
        if (err)
            return;
        int req = Wire.requestFrom(BARO_ADDR, 2);
        if (req != 2) {
            err = ERR_BAD_READLEN;
            return;
        }
        prom[i] = ((uint16_t)Wire.read()) << 8;
        prom[i] |= Wire.read();
    }

    c1 = prom[1];
    c2 = prom[2];
    c3 = prom[3];
    c4 = prom[4];
    c5 = prom[5];
    c6 = prom[6];
    initialised = true;
}

/**
 * @brief  Read Temp status
 * @param
 * @retval  Temperature processed data
 **/
float AtmosphericPressure::getTemp(TempUnit scale, BarosampleLevel level) {
    float result;
    if (getTempAndPressure(&result, NULL, scale, level))
        return result;
    else
        return NAN;
}

/**
 * @brief  Read Pressure status
 * @param
 * @retval  Pressure processed data
 **/
float AtmosphericPressure::getPressure(BarosampleLevel level) {
    float result;
    if (getTempAndPressure(NULL, &result, CELSIUS, level))
        return result;
    else
        return NAN;
}

/**
 * @brief
 * @param
 * @retval
 **/
bool AtmosphericPressure::getTempAndPressure(float *temperature, float *pressure, TempUnit tempScale, BarosampleLevel level) {
    if (err || !initialised)
        return false;

    int32_t d2 = takeReading(CMD_START_D2(level), level);
    if (d2 == 0)
        return false;
    int64_t dt = d2 - c5 * (1L << 8);

    int32_t temp = 2000 + (dt * c6) / (1L << 23);

    int64_t t2;
    if (temp >= 2000) {
        /* High temperature */
        t2 = 5 * (dt * dt) / (1LL << 38);
    } else {
        /* Low temperature */
        t2 = 3 * (dt * dt) / (1LL << 33);
    }

    if (temperature != NULL) {
        *temperature = (float)(temp - t2) / 100;
        if (tempScale == FAHRENHEIT)
            *temperature = *temperature * 9 / 5 + 32;
    }

    if (pressure != NULL) {
        int32_t d1 = takeReading(CMD_START_D1(level), level);
        if (d1 == 0)
            return false;

        int64_t off = c2 * (1LL << 17) + (c4 * dt) / (1LL << 6);
        int64_t sens = c1 * (1LL << 16) + (c3 * dt) / (1LL << 7);

        if (temp < 2000) {
            /* Low temperature */
            int32_t tx = temp - 2000;
            tx *= tx;
            int32_t off2 = 61 * tx / (1 << 4);
            int32_t sens2 = 29 * tx / (1 << 4);
            if (temp < -1500) {
                /* Very low temperature */
                tx = temp + 1500;
                tx *= tx;
                off2 += 17 * tx;
                sens2 += 9 * tx;
            }
            off -= off2;
            sens -= sens2;
        }

        int32_t p = ((int64_t)d1 * sens / (1LL << 21) - off) / (1LL << 15);
        *pressure = (float)p / 100;
    }
    return true;
}

/**
 * @brief  I2C read command for 8bit unsigned data
 * @param
 * @retval  Return state (status_t)
 **/
uint32_t AtmosphericPressure::takeReading(uint8_t trigger_cmd, BarosampleLevel oversample_level) {
    Wire.beginTransmission(BARO_ADDR);
    Wire.write(trigger_cmd);
    err = _endTransmission();

    if (err)
        return 0;
    uint8_t sampling_delay = pgm_read_byte(SamplingDelayMs + (int)oversample_level);
    delay(sampling_delay);

    Wire.beginTransmission(BARO_ADDR);
    Wire.write(CMD_READ_ADC);
    err = _endTransmission(false);
    if (err)
        return 0;
    int req = Wire.requestFrom(BARO_ADDR, 3);
    if (req != 3)
        req = Wire.requestFrom(BARO_ADDR, 3);
    if (req != 3) {
        err = ERR_BAD_READLEN;
        return 0;
    }
    uint32_t result = (uint32_t)Wire.read() << 16;
    result |= (uint32_t)Wire.read() << 8;
    result |= Wire.read();
    return result;
}

/**
 * @brief  Print debug output
 * @param  None
 * @retval None
 **/
void AtmosphericPressure::dumpDebugOutput() {
    Serial.print(F("C1 = 0x"));
    Serial.println(c1, HEX);
    Serial.print(F("C2 = 0x"));
    Serial.println(c2, HEX);
    Serial.print(F("C3 = 0x"));
    Serial.println(c3, HEX);
    Serial.print(F("C4 = 0x"));
    Serial.println(c4, HEX);
    Serial.print(F("C5 = 0x"));
    Serial.println(c5, HEX);
    Serial.print(F("C6 = 0x"));
    Serial.println(c6, HEX);
    Serial.print(F("d1 first = 0x"));
    Serial.println(takeReading(CMD_START_D1(OSR_8192), OSR_8192));
    Serial.print(F("d2 first = 0x"));
    Serial.println(takeReading(CMD_START_D2(OSR_8192), OSR_8192));
    Serial.print(F("d1 second = 0x"));
    Serial.println(takeReading(CMD_START_D1(OSR_8192), OSR_8192));
    Serial.print(F("d2 second = 0x"));
    Serial.println(takeReading(CMD_START_D2(OSR_8192), OSR_8192));
    Serial.print(F("d1 third = 0x"));
    Serial.println(takeReading(CMD_START_D1(OSR_8192), OSR_8192));
    Serial.print(F("d2 third = 0x"));
    Serial.println(takeReading(CMD_START_D2(OSR_8192), OSR_8192));
    float temp, pressure;
    bool res = getTempAndPressure(&temp, &pressure);
    Serial.print(F("result (fourth) = "));
    Serial.println(res ? F("OK") : F("ERR"));
    Serial.print(F("Temp (fourth) = "));
    Serial.println(temp);
    Serial.print(F("Pressure (fourth) = "));
    Serial.println(pressure);
    Serial.print(F("Error (fourth) = "));
    Serial.println(err);
}