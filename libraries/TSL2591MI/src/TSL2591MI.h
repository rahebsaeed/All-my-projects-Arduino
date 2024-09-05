//Yet Another Arduino ams TSL2591 Lux Sensor 
// Copyright (c) 2019 Gregor Christandl <christandlg@yahoo.com>
// home: https://bitbucket.org/christandlg/as3935mi
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef AS3935MI_H_
#define AS3935MI_H_

#include <Arduino.h>

class TSL2591MI
{
public:
    enum channel_t : uint8_t 
    {
        TSL2591_CHANNEL_0 = 0,				//VIS channel
        TSL2591_CHANNEL_1 = 1,				//IR channel
    };

    enum gain_t : uint8_t 
    {
        TSL2591_GAIN_LOW = 0,
        TSL2591_GAIN_MED = 1,
        TSL2591_GAIN_HIGH = 2,
        TSL2591_GAIN_MAX = 3
    };

    enum integration_time_t : uint8_t 
    {
        TSL2591_INTEGRATION_TIME_100ms = 0,
        TSL2591_INTEGRATION_TIME_200ms = 1,
        TSL2591_INTEGRATION_TIME_300ms = 2,
        TSL2591_INTEGRATION_TIME_400ms = 3,
        TSL2591_INTEGRATION_TIME_500ms = 4,
        TSL2591_INTEGRATION_TIME_600ms = 5,
    };

	enum persistence_t : uint8_t
	{
		TSL2591_PERSISTENCE_EVERY_CYCLE = 0b0000,   //every ALS cycle generates an interrupt
		TSL2591_PERSISTENCE_ANY = 0b0001,           //any value outside the threshold range
		TSL2591_PERSISTENCE_2 = 0b0010,             //2 consecutive values out of range
		TSL2591_PERSISTENCE_3 = 0b0011,             //3 consecutive values out of range
		TSL2591_PERSISTENCE_5 = 0b0100,
		TSL2591_PERSISTENCE_10 = 0b0101,
		TSL2591_PERSISTENCE_15 = 0b0110,
		TSL2591_PERSISTENCE_20 = 0b0111,
		TSL2591_PERSISTENCE_25 = 0b1000,
		TSL2591_PERSISTENCE_30 = 0b1001,
		TSL2591_PERSISTENCE_35 = 0b1010,
		TSL2591_PERSISTENCE_40 = 0b1011,
		TSL2591_PERSISTENCE_45 = 0b1100,
		TSL2591_PERSISTENCE_50 = 0b1101,
		TSL2591_PERSISTENCE_55 = 0b1110,
		TSL2591_PERSISTENCE_60 = 0b1111				//60 consecutive values out of range
	};

    enum threshold_t : uint8_t 
    {
        TSL2591_THRESHOLD_ALS_LOW = 0x04,			//ALS interrupt low threshold
        TSL2591_THRESHOLD_ALS_HIGH = 0x06,			//ALS interrupt high threshold
        TSL2591_THRESHOLD_ALS_NP_LOW = 0x08,		//no persist ALS interrupt low threshold
        TSL2591_THRESHOLD_ALS_NP_HIGH = 0x0A		//no persist ALS interrupt high threshold 
    };

    static const uint8_t TSL2591_I2C_ADDRESS = 0x29;

    static const uint8_t TSL2591_ID = 0x50;

	static constexpr double TSL2591_CF_CTI_C0_ISO9845 = 0.025399;		//counts to irradiance conversion factor, preliminary value
	static constexpr double TSL2591_CF_CTI_C1_ISO9845 = 0.076293;		//counts to irradiance conversion factor, preliminary value

	static constexpr double TSL2591_CF_ITB_ISO9845 = 10.728;			//irradiance to brightness conversion factor, preliminary value

	TSL2591MI();
	virtual ~TSL2591MI();

    //inintializes the sensor. calls beginInterface() of derived classes to initialize interface
    bool begin();

	/*
	@return true if ambient light sensor (ALS) functionality is enabled, false otherwise. */
	bool getALSEnabled();

	/*
	enables / disalbes ambient light sensor (ALS) functionality. */
	void setALSEnabled(bool enable);

	/*
	@return true if the device is powered on, false otherwise. */
	bool getPowerOn();

	/*
	switches the device on or off. */
	void setPowerOn(bool on);

    /*
	starts a measurement. if the sensor is powered off, it will be powered on. if the ALS is disabled, it will be enabled.
    @return true on success, false otherwise. */
    bool measure();

    /*
	reads data from sensor is data is available. 
    @return true if data is available. */
    bool hasValue();

	/*
	returns counts for the selected channel. */
	uint16_t getValue();

	/*
	@return irradiance in W / m^2, measured with the selected channel. */
	double getIrradiance();

    /*
    @return brightness in Lux, measured with the selected channel. */
    double getBrightness();

	/*
	performs a measurement. starts a measurement, waits for completion and returns the
	recorded counts. do not use in event loops, as is blocks. execution can take more than 600ms,
	depending on integration time setting.
	@return sensor counts. */
	uint16_t readValue();

    /*
    performs a brightness measurement. starts a measurement, waits for completion and returns the 
    brightness. do not use in event loops, as is blocks. execution can take more than 600ms, 
    depending on integration time setting. 
    @return brightness in Lux. */
    double readBrightness();

	/*
	performs an irradiance measurement. starts a measurement, waits for completion and returns the
	irradiance. do not use in event loops, as is blocks. execution can take more than 600ms,
	depending on integration time setting.
	@return irradiance in W / m^2. */
	double readIrradiance();

    /*
    @return currently selected channel as channel_t. */
    uint8_t getChannel();

    /*
    @param channel (one of channel_t)
    @return true on success, false otherwise. */ 
    bool setChannel(uint8_t channel);

    /*
    @return currently selected gain setting as gain_t. */
    uint8_t getGain();

    /*
    @param gain setting (one of gain_t)
    @return true on success, false otherwise. */
    bool setGain(uint8_t gain);

    /*
    @return currently selected integration time as integration_time_t */
    uint8_t getIntegrationTime();

    /*
    @param integration time setting (one of integration_time_t)
    @return true on success, false otherwise. */
    bool setIntegrationTime(uint8_t time);

    /*
    @return true if ALS interrupts are enabled, false otherwise. */
    bool getALSInterrupt();

    /*
    @param enabled: true to enable ALS interrupts, false to disable. */
    void setALSInterrupt(bool enabled);

    /*
    @return true if no persist interrupts are enabled, false otherwise. */
    bool getNPALSInterrupt();

    /*
    @param enabled: true to enable no persist ALS interrupts, false to disable. 
    if enabled, interrupts are generated immediately, bypassing the persist filter. 
    ALS Interrupts must be enabled using setALSInterrupt(). */
    void setNPALSInterrupt(bool enabled);

	/*
	@param threshold one of the thresholds defined in threshold_t.
	@return threshold value in counts. returns 0 if threshold is invalid. */
	uint16_t getALSInterruptThreshold(uint8_t threshold);

	/*
	@param threshold one of the thresholds defined in threshold_t.
	@param value threshold value in counts.
	@return true on success, false otherwise. */
	bool setALSInterruptThreshold(uint8_t threshold, uint16_t value);
	
	/*
	@param threshold one of the thresholds defined in threshold_t.
	@return threshold value in counts. returns 0 if threshold is invalid. */
	double getALSInterruptThresholdIrradiance(uint8_t threshold);

	/*
	@param threshold one of the thresholds defined in threshold_t. fails if the threshold value is 
	too small or too large for the current gain and integration time settings. must be called again 
	with new values if gain or integration time has changed. 
	@param value threshold value in counts.
	@return true on success, false otherwise. */
	bool setALSInterruptThresholdIrradiance(uint8_t threshold, double value);

	/*
	@param threshold one of the thresholds defined in threshold_t.
	@return threshold value in counts. returns 0 if threshold is invalid. */
	double getALSInterruptThresholdBrightness(uint8_t threshold);

	/*
	@param threshold one of the thresholds defined in threshold_t. fails if the threshold value is 
	too small or too large for the current gain and integration time settings. must be called again 
	with new values if gain or integration time has changed. 
	@param value threshold value in counts.
	@return true on success, false otherwise. */
	bool setALSInterruptThresholdBrightness(uint8_t threshold, double value);

    /*
    @return ALS interrupt persistence filter value as persistence_t */
    uint8_t getALSInterruptPersistence();

    /*
    @param ALS interrupt persistence filter value as persistence_t 
    @return true on success, false otherwise. */
    bool setALSInterruptPersistence(uint8_t value);

    /*
    @return true if enabled, false otherwise. */
    bool getSleepAfterInterrupt();

    /*
    @param enabled: true to send the sensor to sleep mode after an interrupt has been generated. */
    void setSleepAfterInterrupt(bool enabled);

    /*
    @return true if an ALS interrupt has occurred. */
    bool hasALSInterrupt();

    /*
    @return true if ano persist ALS interrupt has occurred. */
    bool hasALSNPInterrupt();

    /*
    @return true if the ADC channels have completed an integration cycle since the ALS was enabled. */
    bool hasALSValid();

    /*
    reads and returns the device ID. A TSL2591 ID is always 0x50 and therefore this function can 
    be used as a connection test. 
    @return the device ID. */
    uint8_t getID();

    /*
    checks the connection by reading the sensor ID.
    @return true if the sensor ID was read correctly. */
    bool checkConnection();

    /*
    checks the Interrupt pin by forcing an interrupt. clears any active interrupts. no interrupt 
    must be attached to the interrupt pin during this test. sensor must be powered on before starting 
	this check. execution takes approx. 5ms.
	@param pin interrupt pin number
	@return true on success, false otherwise */
    bool checkInterrupt(uint8_t pin);

	/*
	sets the interrupt bits in the TSL2591 status register and exposes the interrupt on the int pin
	(if interrupt enable bits are set). */
	void setInterrupt();

	/*
	clears ALS interrupt. does not clear no persist ALS interrupts. */
	void clearALSInterrupt();

	/*
	clear ALS and no persist ALS interrupts. */
	void clearALSNPInterrupt();

	/*
	clears no persist ALS interrupts. does not clear ALS interrupts. */
	void clearNPInterrupt();

	/*
	@return the set sensor temperature for temperature compensation. */
	double getSensorTemperature();

	/*
	@param sensor temperature for temperature compensation. if set to NAN (default), temperature 
	compensation is disabled. note that the datasheet only specifies values for temperatures between 5°C - 70°C. 
	for temperatures outside this range, values are interpolated. */
	void setSensorTemperature(double temperature);

    /*
    resets the sensor to default values. */
    void resetToDefaults();

private:
    enum tsl2591_registers_t : uint8_t 
    {
        TSL2591_REG_ENABLE = 0x00,
        TSL2591_REG_CONFIG = 0x01,
        TSL2591_REG_AILTL = 0x04,
        TSL2591_REG_AILTH = 0x05,
        TSL2591_REG_AIHTL = 0x06,
        TSL2591_REG_AIHTH = 0x07,
        TSL2591_REG_NPAILTL = 0x08,
        TSL2591_REG_NPAILTH = 0x09,
        TSL2591_REG_NPAIHTL = 0x0A,
        TSL2591_REG_NPAIHTH = 0x0B,
        TSL2591_REG_PERSIST = 0x0C,
        TSL2591_REG_PID = 0x11,   // this apparently is always 00!
        TSL2591_REG_ID = 0x12,
        TSL2591_REG_STATUS = 0x13,
        TSL2591_REG_C0DATAL = 0x14,
        TSL2591_REG_C0DATAH = 0x15,
        TSL2591_REG_C1DATAL = 0x16,
        TSL2591_REG_C1DATAH = 0x17,
    };

    enum tsl2591_mask_t : uint8_t 
    {
        TSL2591_MASK_TRANSACTION = 0b01100000,
        TSL2591_MASK_ADDRSF = 0b00011111,
        TSL2591_MASK_NPIEN = 0b10000000,
        TSL2591_MASK_SAI = 0b01000000,
        TSL2591_MASK_AIEN = 0b00010000,
        TSL2591_MASK_AEN = 0b00000010,
        TSL2591_MASK_PON = 0b00000001,
        TSL2591_MASK_SRESET = 0b10000000,
        TSL2591_MASK_AGAIN = 0b00110000,
        TSL2591_MASK_ATIME = 0b00000111,
        TSL2591_MASK_AILTL = 0b11111111,
        TSL2591_MASK_AILTH = 0b11111111,
        TSL2591_MASK_AIHTL = 0b11111111,
        TSL2591_MASK_AIHTH = 0b11111111,
        TSL2591_MASK_NPAILTL = 0b11111111,
        TSL2591_MASK_NPAILTH = 0b11111111,
        TSL2591_MASK_NPAIHTL = 0b11111111,
        TSL2591_MASK_NPAIHTH = 0b11111111,
        TSL2591_MASK_APERS = 0b00001111,
        TSL2591_MASK_PID = 0b00110000,
        TSL2591_MASK_ID = 0b11111111,
        TSL2591_MASK_NPINTR = 0b00100000,
        TSL2591_MASK_AINT = 0b00010000,
        TSL2591_MASK_AVALID = 0b00000001,
        TSL2591_MASK_C0DATAL = 0b11111111,
        TSL2591_MASK_C0DATAH = 0b11111111,
        TSL2591_MASK_C1DATAL = 0b11111111,
        TSL2591_MASK_C1DATAH = 0b11111111,
    };

    static const uint32_t TSL2591_MASK_C0DATA = 0xFFFF;
    static const uint32_t TSL2591_MASK_C1DATA = 0xFFFF;

    static const uint32_t TSL2591_MASK_C01DATA = 0xFFFFFFFF;

    enum tsl2591_transaction_t : uint8_t 
    {
        TSL2591_TRANSACTION_NORMAL = 0b00100000,
        TSL2591_TRANSACTION_SPECIAL = 0b01100000,
    };

    static const uint8_t TSL2591_CMD = 0b10000000;

	enum special_function_t : uint8_t
	{
		TSL2591_SF_SET_INTERRUPT = 0b00100,          //forces interrupt generation
		TSL2591_SF_CLEAR_ALS_INTERRUPT = 0b00110,      //clears ALS interrupt
		TSL2591_SF_CLEAR_ALS_NP_INTERRUPT = 0b00111,   //clears ALS and no persist ALS interrupt
		TSL2591_SF_CLEAR_NP_INTERRUPT = 0b01010,       //clears no persist ALS interrupt
	};

    /*
	@param mask
	@return number of bits to shift value so it fits into mask. */
	template <class T> uint8_t getMaskShift(T mask)
	{
		uint8_t return_value = 0;

		//count how many times the mask must be shifted right until the lowest bit is set
		if (mask != 0)
		{
			while (!(mask & 1))
			{
				return_value++;
				mask >>= 1;
			}
		}

		return return_value;
	};

	/*
	@param register value of register.
	@param mask mask of value in register
	@return value of masked bits. */
	template <class T> T getMaskedBits(T reg, T mask)
	{
		//extract masked bits
		return ((reg & mask) >> getMaskShift(mask));
	};

	/*
	swaps the byte order (MSB -> LSB and LSB -> MSB) of a given uint16_t variable. 
	@param data 
	@return data*/
	uint16_t swapByteOrder(uint16_t data);

	/*
	@param register value of register
	@param mask mask of value in register
	@param value value to write into masked area
	@param register value with masked bits set to value. */
	uint8_t setMaskedBits(uint8_t reg, uint8_t mask, uint8_t value);

    /*
	reads the masked value from the register.
	@param reg register to read.
	@param mask mask of value.
	@return masked value in register. */
	uint8_t readRegisterValue(uint8_t reg, uint8_t mask);

	/*
	sets values in a register.
	@param reg register to set values in
	@param mask bits of register to set value in
	@param value value to set */
	void writeRegisterValue(uint8_t reg, uint8_t mask, uint8_t value);

	/*
	reads the masked values from multiple registers. maximum read length is 4 bytes. 
	@param reg register to read.
	@param mask mask of value.
	@param length number of bytes to read
	@return register content */
	uint32_t readRegisterValueBurst(uint8_t reg, uint32_t mask, uint8_t length);

	/*
	reads a register from the sensor. must be overwritten by derived classes.
	@param reg register to read.
	@return register content*/
	virtual uint8_t readRegister(uint8_t reg) = 0;

	/*
	reads a series of registers from the sensor. can be overwritten by derived classes. 
	if this function is not overwritten, the registers will be read using single-byte reads. 
	@param reg register to read.
	@param length number of bytes to read. maximum length is 4 bytes. 
	@return register content. LSB = last byte read. e.g., if start register = 0xE0 and length = 4, 
	the returned value will contain the register data like this: (MSB) [0xE0 0xE1 0xE2 0xE3] (LSB)*/
	virtual uint32_t readRegisterBurst(uint8_t reg, uint8_t length);

	/*
	writes a register to the sensor. must be overwritten by derived classes.
	this function is also used to send direct commands.
	@param reg register to write to.
	@param value value writeRegister write to register. */
	virtual void writeRegister(uint8_t reg, uint8_t value) = 0;

    /*
    initializes the interface. must be implemented by derived classes. */
    virtual bool beginInterface() = 0;

	/*
	converts a sensor reading from counts to irradiance in W / m^2. 
	@param counts sensor counts
	@param channel sensor channel as cannel_t (TSL2591_CHANNEL_0 or TSL2591_CHANNEL_1)
	@param integration_time integration time as integration_time_t
	@param gain gain setting as gain_t 
	@return irradiance in W / m^2. */
	double countsToIrradiance(uint16_t counts, uint8_t channel, uint8_t integration_time, uint8_t gain);

	/*
	converts an irradiance value from W / m^2 to sensor counts. 
	@oaram irradiance in W / m^2 
	@param channel sensor channel as cannel_t (TSL2591_CHANNEL_0 or TSL2591_CHANNEL_1)
	@param integration_time integration time as integration_time_t
	@param gain gain setting as gain_t 
	@return sensor counts. 0 indicates underflow, 0xFFFF indicates overflow. */
	uint16_t irradianceToCounts(double irradiance, uint8_t channel, uint8_t integration_time, uint8_t gain);

	/*
	converts sensor readings from irradiance to brightness.
	@param irradiance irradiance in W / m^2.
	@return brightness in lux */
	double irradianceToBrightness(double irradiance);

	/*
	converts brightness to irradiance. 
	@param brightness in lux. 
	@return irradiance in W / m^2. */
	double brightnessToIrradiance(double brightness);

	/*
	performs temperature compensation for sensor count values. if temperature_ is set to NAN (default), temperature 
	compensation is disabled. a linear relation between temperature and responsivity has been derived from 
	the datasheet. note that the datasheet only specifies values for temperatures between 5°C - 70°C. for 
	temperatures outside this range, values are interpolated.
	@param counts sensor counts
	@param channel as channel_t
	@return temperature compensated sensor counts. */
	uint16_t temperatureCompensation(uint16_t counts, uint8_t channel);
	

    uint8_t channel_;					//channel as channel_t. 

	uint16_t c0data_;					//channel 0 raw data from last measure command
	uint16_t c1data_;					//channel 1 raw data from last measure command

	double cfCountsToIrradianceCh0_;	//counts to irradiance conversion factor for channel 0
	double cfCountsToIrradianceCh1_;	//counts to irradiance conversion factor for channel 1

	double cfIrradianceToBrightness_;	//irradiance to brightness conversion factor 

	double temperature_;				//sensor temperature (for temperature compensation)
};

#endif /* AS3935MI_H_ */
