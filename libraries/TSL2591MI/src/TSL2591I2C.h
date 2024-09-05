//Yet Another Arduino ams TSL2591 Lux Sensor (using Wire)
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


#ifndef TSL2591I2C_H_
#define TSL2591I2C_H_

#include "TSL2591TwoWire.h"

class TSL2591I2C final: public TSL2591TwoWire
{
public:
    TSL2591I2C();
    ~TSL2591I2C();
};

#endif /* TSL2591I2C_H_ */
