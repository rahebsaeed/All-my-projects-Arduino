#include <Wire.h> 
#include "SparkFunMPL3115A2.h"
#include "SparkFun_Si7021_Breakout_Library.h"
MPL3115A2 myPressure; 
Weather myHumidity;

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
byte seconds; //When it hits 60, increase the current minute
byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes; //Keeps track of where we are in various arrays of data
byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data


const byte WDIR = A0;
const byte LIGHT = A1;
const byte REFERENCE_3V3 = A3;
const byte RAIN = 2;
float batt_lvl = 11.8; //[analog value from 0 to 1023]
float light_lvl = 455; //[analog value from 0 to 1023]
float rainin = 0;
volatile float rainHour[60];
volatile float dailyrainin = 0;
volatile unsigned long raintime, rainlast, raininterval, rain;
void rainIRQ()
// Count rain gauge bucket tips as they occur
// Activated by the magnet and reed switch in the rain gauge, attached to input D2
{
    raintime = millis(); // grab current time
    raininterval = raintime - rainlast; // calculate interval between this and last event

    if (raininterval > 10) // ignore switch-bounce glitches less than 10mS after initial edge
    {
        dailyrainin += 0.011; //Each dump is 0.011" of water
        rainHour[minutes] += 0.011; //Increase this minute's amount of rain

        rainlast = raintime; // set up for next event
    }
}
void setup() {
  Serial.begin(9600);
    Serial.println("Weather Shield Example");
     myPressure.begin();
      myPressure.begin();
      myHumidity.begin();
pinMode(LIGHT, INPUT);
    pinMode(REFERENCE_3V3, INPUT);
    pinMode(RAIN, INPUT);

      attachInterrupt(0, rainIRQ, FALLING);
      interrupts();
      Serial.println("Weather Shield online!");

}

void loop() {
      float humidity = myHumidity.getRH();

  if (humidity == 998) //Humidty sensor failed to respond
    {
      Serial.println("I2C communication to sensors is not working. Check solder connections.");

      //Try re-initializing the I2C comm and the sensors
      myPressure.begin(); 
      myPressure.setModeBarometer();
      myPressure.setOversampleRate(7);
      myPressure.enableEventFlags();
      myHumidity.begin();
    }
    else
    { 
      if(++seconds > 59)
        {
            seconds = 0;

            if(++minutes > 59) minutes = 0;
            if(++minutes_10m > 9) minutes_10m = 0;

            rainHour[minutes] = 0; //Zero out this minute's rainfall amount
            
        }
         Serial.print(",rainin=");
    Serial.println(rainin, 2);
    Serial.print(",dailyrainin=");
    Serial.println(dailyrainin, 2);
      
      light_lvl = get_light_level();
    Serial.print("Light level = ");
      Serial.print(light_lvl*48.5,2);
      Serial.println("%,");
          Serial.println();

      Serial.print("Humidity = ");
      Serial.print(humidity);
      Serial.println("%,");
          Serial.println();

      float temp_h = myHumidity.getTempF();
      Serial.print(" temp_h = ");
      Serial.print(temp_h, 2);
      Serial.print("F,");
    float temp_c=((temp_h-32)*(5/9));
    Serial.print(temp_c, 2);
    Serial.println("C,");
          Serial.println();

 float pressure = myPressure.readPressure();
      Serial.print(" Pressure = ");
      Serial.print(pressure);
      Serial.println("Pa,");
    Serial.println();
    delay(1000);}
  
}
float get_light_level()
{
    float operatingVoltage = analogRead(REFERENCE_3V3);

    float lightSensor = analogRead(LIGHT);

    operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V

    lightSensor = operatingVoltage * lightSensor;

    return(lightSensor);
}