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

#ifndef TSL2591TWOWIRE_H_
#define TSL2591TWOWIRE_H_

#include "TSL2591MI.h"

#include <Wire.h>

class TSL2591TwoWire: public TSL2591MI
{
public:
    TSL2591TwoWire();
    TSL2591TwoWire(TwoWire *wire);
    virtual ~TSL2591TwoWire();

    //inintializes the sensor on the wire interface provide by constructor, otherwise on the default Wire.
    bool begin();
  
    //inintializes the sensor on wire.
    bool begin(TwoWire *wire);
  
protected:
    TwoWire *wire_;

private:
    virtual bool beginInterface();

	virtual uint8_t readRegister(uint8_t reg);

	virtual uint32_t readRegisterBurst(uint8_t reg, uint8_t length);

	virtual void writeRegister(uint8_t reg, uint8_t value);
};

#endif /* TSL2591TWOWIRE_H_ */
