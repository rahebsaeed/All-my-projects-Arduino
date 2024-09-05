// TSL2591_TwoWire.ino
//
// shows how to use the TSL2591MI library with the sensor connected using a TwoWire object. 
//
// Copyright (c) 2019 Gregor Christandl
//
// connect the first TSL2591 to the Arduino like this:
//
// Arduino - TSL2591
// 3.3V ---- VCC
// GND ----- GND
// SDA ----- SDA
// SCL ----- SCL
// other pins can be left unconnected.

#include <Arduino.h>
#include <Wire.h>
#include <TSL2591TwoWire.h>


//TSL2591 I2C address is fixed, so only one sensor is allowed per I2C bus. 
TSL2591TwoWire tsl2591(&Wire);   //using SDA  / SCL

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //wait for serial connection to open (only necessary on some boards)
  while (!Serial);

//setup of sensor on first I2C interface
  Wire.begin();

  if (!tsl2591.begin())
  {
    Serial.println("begin() failed. check the connection to your TSL2591.");
    while (1);
  }

  //reset sensor to default parameters.
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

  //start a measurements
  if (!tsl2591.measure())
  {
    Serial.println("could not start measurement. ");
    return;
  }

  //wait for both measurement to finish
  do
  {
    delay(100);
  } while (!tsl2591.hasValue());

  Serial.print("Brightness: "); Serial.print(tsl2591.getBrightness()); Serial.println(" lux");
}
