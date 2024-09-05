

#include "TSL2591TwoWire.h"

TSL2591TwoWire::TSL2591TwoWire() : 
wire_(nullptr)
{
    //nothing to do here...
}


TSL2591TwoWire::TSL2591TwoWire(TwoWire *wire) : 
wire_(wire)
{
    //nothing to do here...
}


TSL2591TwoWire::~TSL2591TwoWire()
{
    //wire_ is NOT deleted as it may be used elsewhere
    wire_ = nullptr;
}


bool TSL2591TwoWire::begin()
{
    if (!wire_)
        wire_ = &Wire; //init default wire
    return TSL2591MI::begin();
}


bool TSL2591TwoWire::begin(TwoWire *wire)
{
    wire_ = wire;
    return TSL2591MI::begin();
}


bool TSL2591TwoWire::beginInterface()
{
    if (wire_)
        return true;

    return false;
}

uint8_t TSL2591TwoWire::readRegister(uint8_t reg)
{
    if (!wire_)
        return 0;

#if defined(ARDUINO_SAM_DUE)
	//workaround for Arduino Due. The Due seems not to send a repeated start with the code above, so this 
	//undocumented feature of Wire::requestFrom() is used. can be used on other Arduinos too (tested on Mega2560)
	//see this thread for more info: https://forum.arduino.cc/index.php?topic=385377.0
	wire_->requestFrom(TSL2591_I2C_ADDRESS, 1, reg, 1, true);
#else
	wire_->beginTransmission(TSL2591_I2C_ADDRESS);
	wire_->write(reg);
	wire_->endTransmission(false);
	wire_->requestFrom(TSL2591_I2C_ADDRESS, static_cast<uint8_t>(1));
#endif

	return wire_->read();
}

uint32_t TSL2591TwoWire::readRegisterBurst(uint8_t reg, uint8_t length)
{
    if (!wire_)
        return 0L;
        
	if (length > 4)
		return 0L;

	uint32_t data = 0L;

#if defined(ARDUINO_SAM_DUE)
	//workaround for Arduino Due. The Due seems not to send a repeated start with the code below, so this 
	//undocumented feature of Wire::requestFrom() is used. can be used on other Arduinos too (tested on Mega2560)
	//see this thread for more info: https://forum.arduino.cc/index.php?topic=385377.0
	wire_->requestFrom(TSL2591_I2C_ADDRESS, length, data, length, true);
#else
	wire_->beginTransmission(TSL2591_I2C_ADDRESS);
	wire_->write(reg);
	wire_->endTransmission(false);
	wire_->requestFrom(TSL2591_I2C_ADDRESS, static_cast<uint8_t>(length));

	for (uint8_t i = 0; i < length; i++)
	{
		data <<= 8;
		data |= wire_->read();
	}
#endif

	return data;
}

void TSL2591TwoWire::writeRegister(uint8_t reg, uint8_t value)
{
    if (!wire_)
        return;
        
	wire_->beginTransmission(TSL2591_I2C_ADDRESS);
	wire_->write(reg);
	wire_->write(value);
	wire_->endTransmission();
}
