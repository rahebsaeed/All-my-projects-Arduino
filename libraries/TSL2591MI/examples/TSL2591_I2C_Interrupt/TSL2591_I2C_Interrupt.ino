// TSL2591_I2C_Interrupt.ino
//
// shows how to use the TSL2591MI's interrupt feature.
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
// INT ----- PIN_INT
// other pins can be left unconnected.

#include <Arduino.h>
#include <Wire.h>
#include <TSL2591I2C.h>

#if defined(ESP32)
#define PIN_INT 4
#elif defined(ESP8266)
#define PIN_INT D1
#else
#define PIN_INT 12
#endif 

TSL2591I2C tsl2591;

volatile bool interrupt_;

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

	pinMode(PIN_INT, INPUT_PULLUP);

	Serial.print("sensor ID: "); Serial.println(tsl2591.getID(), HEX);

	tsl2591.resetToDefaults();

	tsl2591.setPowerOn(true);
	tsl2591.setALSEnabled(true);

	//set channel
	tsl2591.setChannel(TSL2591MI::TSL2591_CHANNEL_0);

	//set gain
	tsl2591.setGain(TSL2591MI::TSL2591_GAIN_HIGH);

	//set integration time
	tsl2591.setIntegrationTime(TSL2591MI::TSL2591_INTEGRATION_TIME_200ms);

	//interrupt setup
	//----------------------------------------------------------------------------------------------------
	//disable no persist interrupts
	Serial.print("ALSNPInterrupt: ");
	tsl2591.setNPALSInterrupt(false);
	Serial.println(tsl2591.getNPALSInterrupt());

	//enable interrupts with persistence 
	Serial.print("ALSInterrupt: ");
	tsl2591.setALSInterrupt(true);
	Serial.println(tsl2591.getALSInterrupt());

	//set interrupt persistence settings
	Serial.print("ALSInterruptPersistence: ");
	tsl2591.setALSInterruptPersistence(TSL2591MI::TSL2591_PERSISTENCE_ANY);		//TSL2591_PERSISTENCE_ANY: any value outside the threshold range generates an interrupt
	Serial.println(tsl2591.getALSInterruptPersistence());

	//disable sleep mode after interrupt
	Serial.print("SleepAfterInterrupt: ");
	tsl2591.setSleepAfterInterrupt(false);
	Serial.println(tsl2591.getSleepAfterInterrupt());

	//set low limit for interrupts with persistence
	Serial.print("ALSInterruptThreshold TSL2591_THRESHOLD_ALS_LOW: ");
	tsl2591.setALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_LOW, 0x0001);
	Serial.println(tsl2591.getALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_LOW), HEX);

	//set high limit for interrupts with persistence
	Serial.print("ALSInterruptThreshold TSL2591_THRESHOLD_ALS_HIGH: ");
	tsl2591.setALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH, 0x9000);
	Serial.println(tsl2591.getALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_HIGH), HEX);

	//set low limit for interrupts without persistence
	Serial.print("ALSInterruptThreshold TSL2591_THRESHOLD_ALS_NP_LOW: ");
	tsl2591.setALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW, 0x0001);
	Serial.println(tsl2591.getALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_NP_LOW), HEX);

	//set high limit for interrupts without persistence
	Serial.print("ALSInterruptThreshold TSL2591_THRESHOLD_ALS_NP_HIGH: ");
	tsl2591.setALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH, 0x9000);
	Serial.println(tsl2591.getALSInterruptThreshold(TSL2591MI::TSL2591_THRESHOLD_ALS_NP_HIGH), HEX);

	//check the interrupt pin connection by forcing an interrupt. 
	Serial.print("checkInterrupt(): ");
	if (tsl2591.checkInterrupt(PIN_INT))
		Serial.println("true");
	else 
	{
		Serial.println("could not verify interrupt pin connection.");
		Serial.print("please check if the interrupt pin is connected to pin PIN_INT (");
		Serial.print(PIN_INT);
		Serial.println(")");
		Serial.println("of the microcontroller. If the problem persists, try placing a pullup resistor ");
		Serial.println("between sensor pins Int and Vin. ");
		while(1);
	}

	//attach ISR to interrupt pin. mode must be FALLING because TSL2591 interrupt output is active low.
	attachInterrupt(digitalPinToInterrupt(PIN_INT), tsl2591_ISR, FALLING);

	Serial.println("setup() done, waiting for events...");
}

void loop() {
	// put your main code here, to run repeatedly:

	if (interrupt_)
	{
		interrupt_ = false;

		Serial.print("has value: "); Serial.println(tsl2591.hasValue());

		Serial.print("ALS Interrupt: "); Serial.println(tsl2591.hasALSInterrupt());
		Serial.print("ALS NP Interrupt: "); Serial.println(tsl2591.hasALSNPInterrupt());

		Serial.print("Irradiance: "); Serial.print(tsl2591.getIrradiance(), 7); Serial.println(" W / m^2");
		Serial.print("Brightness: "); Serial.print(tsl2591.getBrightness(), 7); Serial.println(" lux");

		tsl2591.clearALSNPInterrupt();
	}
}

//Interrupt Service Routine
#if defined(ESP32)
ICACHE_RAM_ATTR void tsl2591_ISR()
{
	interrupt_ = true;
}
#elif defined(ESP8266)
ICACHE_RAM_ATTR void tsl2591_ISR()
{
	interrupt_ = true;
}
#else
void tsl2591_ISR()
{
	interrupt_ = true;
}
#endif 

