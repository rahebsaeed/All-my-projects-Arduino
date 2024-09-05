// TSL2591_I2C.ino
//
// shows how to use the TSL2591MI library with the sensor connected using I2C.
//
// Copyright (c) 2019-2020 Gregor Christandl
//
// connect the TSL2591 to the Arduino like this:
//
// Arduino - TSL2591
// 3.3V ---- VCC
// GND ----- GND
// SDA ----- SDA
// SCL ----- SCL
// other pins can be left unconnected.

#include <Arduino.h>
#include <Wire.h>
#include <TSL2591I2C.h>

TSL2591I2C tsl2591;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);

	//wait for serial connection to open (only necessary on some boards)
	while (!Serial);

#if defined(ESP32)
	Wire.begin(SDA, SCL);
#elif defined(ESP8266)
	Wire.begin(D2, D3);
#else
	Wire.begin();
#endif 

	if (!tsl2591.begin())
	{
		Serial.println("begin() failed. check the connection to your TSL2591.");
		while (1);
	}

	Serial.print("sensor ID: "); Serial.println(tsl2591.getID(), HEX);

	tsl2591.resetToDefaults();

	//set channel
	tsl2591.setChannel(TSL2591MI::TSL2591_CHANNEL_0);

	//set gain
	tsl2591.setGain(TSL2591MI::TSL2591_GAIN_MED);

	//set integration time
	tsl2591.setIntegrationTime(TSL2591MI::TSL2591_INTEGRATION_TIME_100ms);
}

void loop() {
	// put your main code here, to run repeatedly:

	delay(1000);

	//start a measurement
	if (!tsl2591.measure())
	{
		Serial.println("could not start measurement. ");
		return;
	}

	//wait for the measurement to finish
	do
	{
		delay(100);
	} while (!tsl2591.hasValue());

	Serial.print("Irradiance: "); Serial.print(tsl2591.getIrradiance(), 7); Serial.println(" W / m^2");
	Serial.print("Brightness: "); Serial.print(tsl2591.getBrightness(), 7); Serial.println(" lux");
}
