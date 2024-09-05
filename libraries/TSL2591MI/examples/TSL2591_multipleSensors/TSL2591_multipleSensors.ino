// TSL2591_multipleSensors.ino
//
// shows how to use the TSL2591MI library with multiple sensors. This requires an Arduino with more than one Hardware I2C bus
// or a Software I2C library that implements the TwoWire interface. This example works on an Arduino Mega2560 or an Arduino Due. 
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
//
// connect the second TSL2591 to the Arduino like this:
//
// Arduino - TSL2591
// 3.3V ---- VCC
// GND ----- GND
// SDA1 ---- SDA
// SCL1 ---- SCL
//
// other pins can be left unconnected.

#include <Arduino.h>
#include <Wire.h>
#include <TSL2591TwoWire.h>

//TSL2591 I2C address is fixed, so only one sensor is allowed per I2C bus. 
TSL2591TwoWire tsl2591a(&Wire);   //using SDA  / SCL
TSL2591TwoWire tsl2591b(&Wire1);  //using SDA1 / SCL1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //wait for serial connection to open (only necessary on some boards)
  while (!Serial);

  //setup of sensor on first I2C interface
  Wire.begin();

  if (!tsl2591a.begin())
  {
    Serial.println("begin() on IC2 interface 0 failed. check the connection from SDA/SCL to your TSL2591.");
    while (1);
  }

  //reset sensor to default parameters.
  tsl2591a.resetToDefaults();  

  //set channel
  tsl2591a.setChannel(TSL2591MI::TSL2591_CHANNEL_0);

  //set gain
  tsl2591a.setGain(TSL2591MI::TSL2591_GAIN_MED);

  //set integration time
  tsl2591a.setIntegrationTime(TSL2591MI::TSL2591_INTEGRATION_TIME_100ms);

  //setup of sensor on second I2C interface

  Wire1.begin();

  if (!tsl2591b.begin())
  {
    Serial.println("begin() on IC2 interface 1 failed. check the connection from SDA1/SCL1 to your TSL2591.");
    while (1);
  }

  //reset sensor to default parameters.
  tsl2591b.resetToDefaults();  

  //set channel
  tsl2591b.setChannel(TSL2591MI::TSL2591_CHANNEL_0);

  //set gain
  tsl2591b.setGain(TSL2591MI::TSL2591_GAIN_HIGH);

  //set integration time
  tsl2591b.setIntegrationTime(TSL2591MI::TSL2591_INTEGRATION_TIME_300ms);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(1000);

  //start a measurements
  if (!tsl2591a.measure())
  {
    Serial.println("could not start measurement on sensor on interface 1");
    return;
  }
  if (!tsl2591b.measure())
  {
    Serial.println("could not start measurement on sensor on interface 2");
    return;
  }

  //wait for both measurement to finish
  do
  {
    delay(100);
  } while (!(tsl2591a.hasValue() && tsl2591b.hasValue()));

  Serial.print("Brightness (sensor 1): "); Serial.println(tsl2591a.getBrightness());
  Serial.print("Brightness (sensor 2): "); Serial.println(tsl2591b.getBrightness());
}
