//Yet Another Arduino ams TSL2591MI Lux Sensor 
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

#include "TSL2591MI.h"


TSL2591MI::TSL2591MI() :
	channel_(TSL2591MI::TSL2591_CHANNEL_0),
	c0data_(0x0000),
	c1data_(0x0000),
	cfCountsToIrradianceCh0_(TSL2591_CF_CTI_C0_ISO9845),
	cfCountsToIrradianceCh1_(TSL2591_CF_CTI_C1_ISO9845),
	cfIrradianceToBrightness_(TSL2591_CF_ITB_ISO9845),
	temperature_(NAN)
{
	//nothing to do here...
}

TSL2591MI::~TSL2591MI()
{
	//nothing to do here...
}

bool TSL2591MI::begin()
{
	if (!beginInterface())
		return false;

	if (getID() != TSL2591_ID)
		return false;

	setPowerOn(true);

	setALSEnabled(true);

	return true;
}

bool TSL2591MI::getALSEnabled()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_AEN));
}

void TSL2591MI::setALSEnabled(bool enable)
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_AEN, enable ? 1 : 0);
}

bool TSL2591MI::getPowerOn()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_PON));
}

void TSL2591MI::setPowerOn(bool on)
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_PON, on ? 1 : 0);
}

bool TSL2591MI::measure()
{
	//power on if necessary
	if (!getPowerOn())
		setPowerOn(true);

	//activate ALS if necessary
	if (!getALSEnabled())
		setALSEnabled(true);

	return true;
}

bool TSL2591MI::hasValue()
{
	//query sensor for completion
	bool complete = hasALSValid();

	//download measurement results if measurement has been completed
	if (complete)
	{
		c0data_ = swapByteOrder(readRegisterValueBurst(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_C0DATAL, (TSL2591_MASK_C0DATAH << 8) | TSL2591_MASK_C0DATAL, 2));
		c1data_ = swapByteOrder(readRegisterValueBurst(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_C1DATAL, (TSL2591_MASK_C1DATAH << 8) | TSL2591_MASK_C1DATAL, 2));

		c0data_ = temperatureCompensation(c0data_, TSL2591MI::TSL2591_CHANNEL_0);
		c1data_ = temperatureCompensation(c1data_, TSL2591MI::TSL2591_CHANNEL_1);
	}

	return complete;
}

uint16_t TSL2591MI::getValue()
{
	switch (channel_)
	{
	case TSL2591MI::TSL2591_CHANNEL_0:
		return c0data_;
	case TSL2591MI::TSL2591_CHANNEL_1:
		return c1data_;
	}

	return 0;
}

double TSL2591MI::getIrradiance()
{
	double value = NAN;

	switch (channel_)
	{
	case TSL2591MI::TSL2591_CHANNEL_0:
		value = countsToIrradiance(c0data_, TSL2591MI::TSL2591_CHANNEL_0, getIntegrationTime(), getGain());
		break;
	case TSL2591MI::TSL2591_CHANNEL_1:
		value = countsToIrradiance(c1data_, TSL2591MI::TSL2591_CHANNEL_1, getIntegrationTime(), getGain());
		break;
	}

	return value;
}

double TSL2591MI::getBrightness()
{
	return irradianceToBrightness(getIrradiance());
}

uint16_t TSL2591MI::readValue()
{
	//start a measurement
        measure();    // returns always true!

	//wait for measurement to finished
	do
	{
		delay(100);
	} while (!hasValue());

	return getValue();
}

double TSL2591MI::readBrightness()
{
	//start a measurement
	if (!measure())
		return NAN;

	//wait for measurement to finished
	do
	{
		delay(100);
	} while (!hasValue());

	return getBrightness();
}

double TSL2591MI::readIrradiance()
{
	if (!measure())
		return NAN;

	//wait for measurement to finished
	do
	{
		delay(100);
	} while (!hasValue());

	return getIrradiance();
}

uint8_t TSL2591MI::getChannel()
{
	return channel_;
}

bool TSL2591MI::setChannel(uint8_t channel)
{
	switch (channel)
	{
	case TSL2591MI::TSL2591_CHANNEL_0:
	case TSL2591MI::TSL2591_CHANNEL_1:
		channel_ = channel;
		return true;
	default:
		break;
	}

	return false;
}

uint8_t TSL2591MI::getGain()
{
	return readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_CONFIG, TSL2591_MASK_AGAIN);
}

bool TSL2591MI::setGain(uint8_t gain)
{
	switch (gain)
	{
	case TSL2591MI::TSL2591_GAIN_LOW:
	case TSL2591MI::TSL2591_GAIN_MED:
	case TSL2591MI::TSL2591_GAIN_HIGH:
	case TSL2591MI::TSL2591_GAIN_MAX:
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_CONFIG, TSL2591_MASK_AGAIN, gain);
		return true;
	default:
		break;
	}

	return false;
}

uint8_t TSL2591MI::getIntegrationTime()
{
	return readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_CONFIG, TSL2591_MASK_ATIME);
}

bool TSL2591MI::setIntegrationTime(uint8_t integration_time)
{
	switch (integration_time)
	{
	case TSL2591MI::TSL2591_INTEGRATION_TIME_100ms:
	case TSL2591MI::TSL2591_INTEGRATION_TIME_200ms:
	case TSL2591MI::TSL2591_INTEGRATION_TIME_300ms:
	case TSL2591MI::TSL2591_INTEGRATION_TIME_400ms:
	case TSL2591MI::TSL2591_INTEGRATION_TIME_500ms:
	case TSL2591MI::TSL2591_INTEGRATION_TIME_600ms:
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_CONFIG, TSL2591_MASK_ATIME, integration_time);
		return true;
	default:
		break;
	}

	return false;
}

bool TSL2591MI::getALSInterrupt()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_AIEN));
}

void TSL2591MI::setALSInterrupt(bool enabled)
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_AIEN, enabled ? 1 : 0);
}

bool TSL2591MI::getNPALSInterrupt()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_NPIEN));
}

void TSL2591MI::setNPALSInterrupt(bool enabled)
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_NPIEN, enabled ? 1 : 0);
}

uint16_t TSL2591MI::getALSInterruptThreshold(uint8_t threshold)
{
	uint16_t value = 0;

	switch (threshold)
	{
	case TSL2591MI::TSL2591_THRESHOLD_ALS_LOW:
		value = swapByteOrder(readRegisterValueBurst(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AILTL, (TSL2591_MASK_AILTL << 8) | TSL2591_MASK_AILTH, 2));
		// value =
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AILTL, TSL2591_MASK_AILTL) |
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AILTH, TSL2591_MASK_AILTH) << 8;
		break;
	case TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH:
		value = swapByteOrder(readRegisterValueBurst(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AIHTL, (TSL2591_MASK_AIHTL << 8) | TSL2591_MASK_AIHTH, 2));
		// value =
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AIHTL, TSL2591_MASK_AIHTL) |
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AIHTH, TSL2591_MASK_AIHTH) << 8;
		break;
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW:
		value = swapByteOrder(readRegisterValueBurst(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAILTL, (TSL2591_MASK_NPAILTL << 8) | TSL2591_MASK_NPAILTH, 2));
		// value =
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAILTL, TSL2591_MASK_NPAILTL) |
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAILTH, TSL2591_MASK_NPAILTH) << 8;
		break;
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH:
		value = swapByteOrder(readRegisterValueBurst(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAIHTL, (TSL2591_MASK_NPAIHTL << 8) | TSL2591_MASK_NPAIHTH, 2));
		// value =
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAIHTL, TSL2591_MASK_NPAIHTL) |
			// readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAIHTH, TSL2591_MASK_NPAIHTH) << 8;
		break;
	default:
		break;
	}

	return value;
}

bool TSL2591MI::setALSInterruptThreshold(uint8_t threshold, uint16_t value)
{
	switch (threshold)
	{
	case TSL2591MI::TSL2591_THRESHOLD_ALS_LOW:
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AILTL, TSL2591_MASK_AILTL, value);
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AILTH, TSL2591_MASK_AILTH, value >> 8);
		break;
	case TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH:
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AIHTL, TSL2591_MASK_AIHTL, value);
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_AIHTH, TSL2591_MASK_AIHTH, value >> 8);
		break;
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW:
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAILTL, TSL2591_MASK_NPAILTL, value);
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAILTH, TSL2591_MASK_NPAILTH, value >> 8);
		break;
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH:
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAIHTL, TSL2591_MASK_NPAIHTL, value);
		writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_NPAIHTH, TSL2591_MASK_NPAIHTH, value >> 8);
		break;
	default:
		return false;
	}

	return true;
}

double TSL2591MI::getALSInterruptThresholdIrradiance(uint8_t threshold)
{
	switch (threshold)
	{
	case TSL2591MI::TSL2591_THRESHOLD_ALS_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH:
		return countsToIrradiance(
			getALSInterruptThreshold(threshold),
			TSL2591MI::TSL2591_CHANNEL_0,
			getIntegrationTime(),
			getGain());
	default:
		break;
	}

	return false;
}

bool TSL2591MI::setALSInterruptThresholdIrradiance(uint8_t threshold, double value)
{
	switch (threshold)
	{
	case TSL2591MI::TSL2591_THRESHOLD_ALS_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH:
	{
		uint16_t counts = irradianceToCounts(
			value,
			TSL2591MI::TSL2591_CHANNEL_0,
			getIntegrationTime(),
			getGain()
		);

		//do nothing if an over / underflow occurred
		if ((counts == 0x0000) || (counts == 0xFFFF))
			return false;

		return setALSInterruptThreshold(threshold, counts);
	}
	default:
		break;
	}

	return false;
}

double TSL2591MI::getALSInterruptThresholdBrightness(uint8_t threshold)
{
	switch (threshold)
	{
	case TSL2591MI::TSL2591_THRESHOLD_ALS_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH:
		return irradianceToBrightness(
			countsToIrradiance(
				getALSInterruptThreshold(threshold),
				TSL2591MI::TSL2591_CHANNEL_0,
				getIntegrationTime(),
				getGain()
			)
		);
	default:
		break;
	}

	return false;
}

bool TSL2591MI::setALSInterruptThresholdBrightness(uint8_t threshold, double value)
{
	switch (threshold)
	{
	case TSL2591MI::TSL2591_THRESHOLD_ALS_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW:
	case TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH:
	{
		uint16_t counts = irradianceToCounts(
			brightnessToIrradiance(value),
			TSL2591MI::TSL2591_CHANNEL_0,
			getIntegrationTime(),
			getGain()
		);

		//do nothing if an over / underflow occurred
		if ((counts == 0x0000) || (counts == 0xFFFF))
			return false;

		return setALSInterruptThreshold(
			threshold,
			counts
		);
	}
	default:
		break;
	}

	return false;
}

uint8_t TSL2591MI::getALSInterruptPersistence()
{
	return readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_PERSIST, TSL2591_MASK_APERS);
}

bool TSL2591MI::setALSInterruptPersistence(uint8_t value)
{
	switch (value)
	{
	case TSL2591MI::TSL2591_PERSISTENCE_EVERY_CYCLE:
	case TSL2591MI::TSL2591_PERSISTENCE_ANY:
	case TSL2591MI::TSL2591_PERSISTENCE_2:
	case TSL2591MI::TSL2591_PERSISTENCE_3:
	case TSL2591MI::TSL2591_PERSISTENCE_5:
	case TSL2591MI::TSL2591_PERSISTENCE_10:
	case TSL2591MI::TSL2591_PERSISTENCE_15:
	case TSL2591MI::TSL2591_PERSISTENCE_20:
	case TSL2591MI::TSL2591_PERSISTENCE_25:
	case TSL2591MI::TSL2591_PERSISTENCE_30:
	case TSL2591MI::TSL2591_PERSISTENCE_35:
	case TSL2591MI::TSL2591_PERSISTENCE_40:
	case TSL2591MI::TSL2591_PERSISTENCE_45:
	case TSL2591MI::TSL2591_PERSISTENCE_50:
	case TSL2591MI::TSL2591_PERSISTENCE_55:
	case TSL2591MI::TSL2591_PERSISTENCE_60:
		break;
	default:
		return false;
	}

	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_PERSIST, TSL2591_MASK_APERS, value);

	return true;
}

bool TSL2591MI::getSleepAfterInterrupt()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_SAI));
}

void TSL2591MI::setSleepAfterInterrupt(bool enabled)
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, TSL2591_MASK_SAI, enabled ? 1 : 0);
}

bool TSL2591MI::hasALSInterrupt()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_STATUS, TSL2591_MASK_AINT));
}

bool TSL2591MI::hasALSNPInterrupt()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_STATUS, TSL2591_MASK_NPINTR));
}

bool TSL2591MI::hasALSValid()
{
	return static_cast<bool>(readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_STATUS, TSL2591_MASK_AVALID));
}

uint8_t TSL2591MI::getID()
{
	return readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ID, TSL2591_MASK_ID);
}

bool TSL2591MI::checkConnection()
{
	return (readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ID, TSL2591_MASK_ID) == TSL2591MI::TSL2591_ID);
}

bool TSL2591MI::checkInterrupt(uint8_t pin)
{
	bool result = true;

	//memorize enable register settings
	uint8_t reg_enable = readRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, 0xFF);

	//switch off ALS to keep it from triggering interrupts
	setALSEnabled(false);

	//keep sensor awake after interrupt
	setSleepAfterInterrupt(false);

	//enable interrupts
	setALSInterrupt(true);
	setNPALSInterrupt(true);

	//clear interrupts
	clearALSNPInterrupt();

	//wait some time
	delay(1);

	//check fails if the interrupt pin cannot be cleared
	if (!digitalRead(pin))
		result = false;
	else
	{
		//force interrupts
		setInterrupt();

		//wait some time
		delay(1);

		//check fails if interrupt pin is still cleared
		if (digitalRead(pin))
			result = false;
	}

	//clear interrupts
	clearALSNPInterrupt();

	//wait some time
	delay(1);

	//restore enable register settings
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_ENABLE, 0xFF, reg_enable);

	return result;
}

void TSL2591MI::setInterrupt()
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_SPECIAL | TSL2591MI::TSL2591_SF_SET_INTERRUPT, 1, 1);
}

void TSL2591MI::clearALSInterrupt()
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_SPECIAL | TSL2591MI::TSL2591_SF_CLEAR_ALS_INTERRUPT, 1, 1);
}

void TSL2591MI::clearALSNPInterrupt()
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_SPECIAL | TSL2591MI::TSL2591_SF_CLEAR_ALS_NP_INTERRUPT, 1, 0);
}

void TSL2591MI::clearNPInterrupt()
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_SPECIAL | TSL2591MI::TSL2591_SF_CLEAR_NP_INTERRUPT, 1, 1);
}

double TSL2591MI::getSensorTemperature()
{
	return temperature_;
}

void TSL2591MI::setSensorTemperature(double temperature)
{
	temperature_ = temperature;
}

void TSL2591MI::resetToDefaults()
{
	writeRegisterValue(TSL2591_CMD | TSL2591_TRANSACTION_NORMAL | TSL2591_REG_CONFIG, TSL2591_MASK_SRESET, 1);
}

uint16_t TSL2591MI::swapByteOrder(uint16_t data)
{
	//move LSB to MSB and MSB to LSB
	return (data << 8) | (data >> 8);
}

uint8_t TSL2591MI::setMaskedBits(uint8_t reg, uint8_t mask, uint8_t value)
{
	//clear mask bits in register
	reg &= (~mask);

	//set masked bits in register according to value
	return ((value << getMaskShift(mask)) & mask) | reg;
}

uint8_t TSL2591MI::readRegisterValue(uint8_t reg, uint8_t mask)
{
	return getMaskedBits(readRegister(reg), mask);
}

void TSL2591MI::writeRegisterValue(uint8_t reg, uint8_t mask, uint8_t value)
{
	uint8_t reg_val = readRegister(reg);
	writeRegister(reg, setMaskedBits(reg_val, mask, value));
}

uint32_t TSL2591MI::readRegisterValueBurst(uint8_t reg, uint32_t mask, uint8_t length)
{
	return getMaskedBits(readRegisterBurst(reg, length), mask);
}

uint32_t TSL2591MI::readRegisterBurst(uint8_t reg, uint8_t length)
{
	if (length > 4)
		return 0L;

	uint32_t data = 0L;

	for (uint8_t i = 0; i < length; i++)
	{
		data <<= 8;
		data |= static_cast<uint32_t>(readRegister(reg));
	}

	return data;
}

double TSL2591MI::countsToIrradiance(uint16_t counts, uint8_t channel, uint8_t integration_time, uint8_t gain)
{
	//at 100ms integration time, maximum possible counter value is 0x9400.
	if (((integration_time == TSL2591MI::TSL2591_INTEGRATION_TIME_100ms) && (counts >= 0x9400)) || (counts == 0xFFFF))
		return INFINITY;

	double value = static_cast<double>(counts);

	//convert counts to uW / cm^2
	switch (channel)
	{
	case TSL2591MI::TSL2591_CHANNEL_0:
		//for white light 
		value /= (6024.0 / 9876.0);
		//value /= 0.60996354799513973268529769137303;
		//for 850nm 
		//value /= (5338.0 / 9876.0);
		//value /= 0.54050222762251923855812069663832;  
		break;
	case TSL2591MI::TSL2591_CHANNEL_1:
		//for white light 
		value /= (1003.0 / 9876.0);
		//value /= 0.10155933576346699068448764682058;
		//for 850nm 
		//value /= (3474.0 / 9876.0);
		//value /= 0.35176184690157958687727825030377;
		break;
	default:
		return NAN;
	}

	switch (integration_time)
	{
	case TSL2591MI::TSL2591_INTEGRATION_TIME_100ms:
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_200ms:
		value /= 2.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_300ms:
		value /= 3.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_400ms:
		value /= 4.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_500ms:
		value /= 5.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_600ms:
		value /= 6.0;
		break;
	default:
		return NAN;
	}

	switch (gain)
	{
	case TSL2591MI::TSL2591_GAIN_LOW:
		break;
	case TSL2591MI::TSL2591_GAIN_MED:
		value /= 25.0;
		break;
	case TSL2591MI::TSL2591_GAIN_HIGH:
		value /= 428.0;
		break;
	case TSL2591MI::TSL2591_GAIN_MAX:
		value /= 9876.0;
		break;
	default:
		return NAN;
	}

	//convert from uW / cm^2 to W / m^2
	value /= 100.0;

	return value;
}

uint16_t TSL2591MI::irradianceToCounts(double irradiance, uint8_t channel, uint8_t integration_time, uint8_t gain)
{
	int32_t counts = 0;

	//convert irradiance from W / m^2 to uW / cm^2 
	irradiance *= 100.0;

	switch (gain)
	{
	case TSL2591MI::TSL2591_GAIN_LOW:
		break;
	case TSL2591MI::TSL2591_GAIN_MED:
		irradiance *= 25.0;
		break;
	case TSL2591MI::TSL2591_GAIN_HIGH:
		irradiance *= 428.0;
		break;
	case TSL2591MI::TSL2591_GAIN_MAX:
		irradiance *= 9876.0;
		break;
	default:
		return 0;
	}

	switch (integration_time)
	{
	case TSL2591MI::TSL2591_INTEGRATION_TIME_100ms:
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_200ms:
		irradiance *= 2.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_300ms:
		irradiance *= 3.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_400ms:
		irradiance *= 4.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_500ms:
		irradiance *= 5.0;
		break;
	case TSL2591MI::TSL2591_INTEGRATION_TIME_600ms:
		irradiance *= 6.0;
		break;
	default:
		return 0;
	}

	switch (channel)
	{
	case TSL2591MI::TSL2591_CHANNEL_0:
		//for white light 
		irradiance *= (6024.0 / 9876.0);
		//value *= 0.60996354799513973268529769137303;
		//for 850nm 
		//value *= (5338.0 / 9876.0);
		//value *= 0.54050222762251923855812069663832;  
		break;
	case TSL2591MI::TSL2591_CHANNEL_1:
		//for white light 
		irradiance *= (1003.0 / 9876.0);
		//value *= 0.10155933576346699068448764682058;
		//for 850nm 
		//value *= (3474.0 / 9876.0);
		//value *= 0.35176184690157958687727825030377;
		break;
	default:
		return 0;
	}

	counts = static_cast<int32_t>(irradiance);

	//at 100ms integration time, maximum possible counter value is 0x9400.
	if ((integration_time == TSL2591MI::TSL2591_INTEGRATION_TIME_100ms) && (counts >= 0x9400))
		counts = 0xFFFF;

	//constrain counts to range valid for uint16_t type
	counts = constrain(counts, 0, 0xFFFF);

	return static_cast<uint16_t>(counts);
}

double TSL2591MI::irradianceToBrightness(double irradiance)
{
	return irradiance * cfIrradianceToBrightness_;
}

double TSL2591MI::brightnessToIrradiance(double brightness)
{
	return brightness / cfIrradianceToBrightness_;
}

uint16_t TSL2591MI::temperatureCompensation(uint16_t counts, uint8_t channel)
{
	//no temperature compensation is performed if no temperature is set
        if (isnan(temperature_))
		return counts;

	int32_t adjusted = 0;

	switch (channel)
	{
	case TSL2591MI::TSL2591_CHANNEL_0:
		//temperature coefficients extracted from datasheet
		adjusted = static_cast<int32_t>(static_cast<double>(counts) * (0.000710004516169 * temperature_ + 0.981259762126851));
		break;
	case TSL2591MI::TSL2591_CHANNEL_1:
		adjusted = static_cast<int32_t>(static_cast<double>(counts) * (-0.001931424031172 * temperature_ + 1.04866966736681));
		break;
	}

	return static_cast<uint16_t>(constrain(adjusted, 0, 65535));
}
