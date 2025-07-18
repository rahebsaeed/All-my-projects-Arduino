

#include <Wire.h> //I2C needed for sensors
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFun_Si7021_Breakout_Library.h" //Humidity sensor - Search "SparkFun Si7021" and install from Library Manager

MPL3115A2 myPressure; //Create an instance of the pressure sensor
Weather myHumidity;//Create an instance of the humidity sensor



//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// digital I/O pins
const byte WSPEED = 3;
const byte RAIN = 2;
const byte STAT1 = 7;
const byte STAT2 = 8;
// analog I/O pins
const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;
const byte WDIR = A0;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Weather sensor;
//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
byte seconds; //When it hits 60, increase the current minute
byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes; //Keeps track of where we are in various arrays of data
byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data

long lastWindCheck = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;

//We need to keep track of the following variables:
//Wind speed/dir each update (no storage)
//Wind gust/dir over the day (no storage)
//Wind speed/dir, avg over 2 minutes (store 1 per second)
//Wind gust/dir over last 10 minutes (store 1 per minute)
//Rain over the past hour (store 1 per minute)
//Total rain over date (store one per day)

byte windspdavg[120]; //120 bytes to keep track of 2 minute average

#define WIND_DIR_AVG_SIZE 120
int winddiravg[WIND_DIR_AVG_SIZE]; //120 ints to keep track of 2 minute average
float windgust_10m[10]; //10 floats to keep track of 10 minute max
int windgustdirection_10m[10]; //10 ints to keep track of 10 minute max
volatile float rainHour[60]; //60 floating numbers to keep track of 60 minutes of rain

//These are all the weather values that wunderground expects:
int winddir = 0; // [0-360 instantaneous wind direction]
float windspeedmph = 0; // [mph instantaneous wind speed]
float windgustmph = 0; // [mph current wind gust, using software specific time period]
int windgustdir = 0; // [0-360 using software specific time period]
float windspdmph_avg2m = 0; // [mph 2 minute average wind speed mph]
int winddir_avg2m = 0; // [0-360 2 minute average wind direction]
float windgustmph_10m = 0; // [mph past 10 minutes wind gust mph ]
int windgustdir_10m = 0; // [0-360 past 10 minutes wind gust direction]
float humidity = 0; // [%]
float tempf = 0; // [temperature F]
float rainin = 0; // [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min
volatile float dailyrainin = 0; // [rain inches so far today in local time]
//float baromin = 30.03;// [barom in] - It's hard to calculate baromin locally, do this in the agent
float pressure = 0;
//float dewptf; // [dewpoint F] - It's hard to calculate dewpoint locally, do this in the agent

float batt_lvl = 11.8; //[analog value from 0 to 1023]
float light_lvl = 455; //[analog value from 0 to 1023]

// volatiles are subject to modification by IRQs
volatile unsigned long raintime, rainlast, raininterval, rain;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Interrupt routines (these are called by the hardware interrupts, not by the main code)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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

void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
    if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
    {
        lastWindIRQ = millis(); //Grab the current time
        windClicks++; //There is 1.492MPH for each click per second.
    }
}


void setup()
{
    Serial.begin(115400);
    Serial.println("projet station meteo");

    pinMode(STAT1, OUTPUT); //Status LED Blue
    pinMode(STAT2, OUTPUT); //Status LED Green

    pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
    pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor

    pinMode(REFERENCE_3V3, INPUT);
    pinMode(LIGHT, INPUT);

    //Configure the pressure sensor
    myPressure.begin(); // Get sensor online
    myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
    myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
    myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

    //Configure the humidity sensor
    myHumidity.begin();
    sensor.begin();

    seconds = 0;
    lastSecond = millis();

    // attach external interrupt pins to IRQ functions
    attachInterrupt(0, rainIRQ, FALLING);
    attachInterrupt(1, wspeedIRQ, FALLING);

    // turn on interrupts
    interrupts();

    Serial.println("Weather Shield en ligne!");

}

void loop()
{
    //Keep track of which minute it is
  if(millis() - lastSecond >= 1000)
    {
        digitalWrite(STAT1, HIGH); //Blink stat LED

    lastSecond += 1000;

        //Take a speed and direction reading every second for 2 minute average
        if(++seconds_2m > 119) seconds_2m = 0;

        //Calc the wind speed and direction every second for 120 second to get 2 minute average
        float currentSpeed = get_wind_speed();
        windspeedmph = currentSpeed;//update global variable for windspeed when using the printWeather() function
        //float currentSpeed = random(5); //For testing
        int currentDirection = get_wind_direction();
        windspdavg[seconds_2m] = (int)currentSpeed;
        winddiravg[seconds_2m] = currentDirection;
        //if(seconds_2m % 10 == 0) displayArrays(); //For testing

        //Check to see if this is a gust for the minute
        if(currentSpeed > windgust_10m[minutes_10m])
        {
            windgust_10m[minutes_10m] = currentSpeed;
            windgustdirection_10m[minutes_10m] = currentDirection;
        }

        //Check to see if this is a gust for the day
        if(currentSpeed > windgustmph)
        {
            windgustmph = currentSpeed;
            windgustdir = currentDirection;
        }

        if(++seconds > 59)
        {
            seconds = 0;

            if(++minutes > 59) minutes = 0;
            if(++minutes_10m > 9) minutes_10m = 0;

            rainHour[minutes] = 0; //Zero out this minute's rainfall amount
            windgust_10m[minutes_10m] = 0; //Zero out this minute's gust
        }

        //Report all readings every second
        printWeather();

        digitalWrite(STAT1, LOW); //Turn off stat LED
    }

  delay(4000);
}

//Calculates each of the variables that wunderground is expecting
void calcWeather()
{
    //Calc winddir
    winddir = get_wind_direction();

    //Calc windspeed
    //windspeedmph = get_wind_speed(); //This is calculated in the main loop on line 185

    //Calc windgustmph
    //Calc windgustdir
    //These are calculated in the main loop

    //Calc windspdmph_avg2m
    float temp = 0;
    for(int i = 0 ; i < 120 ; i++)temp += windspdavg[i];
    temp /= 120.0;
    windspdmph_avg2m = temp;

    //Calc winddir_avg2m, Wind Direction
    //You can't just take the average. Google "mean of circular quantities" for more info
    //We will use the Mitsuta method because it doesn't require trig functions
    //And because it sounds cool.
    //Based on: http://abelian.org/vlf/bearings.html
    //Based on: http://stackoverflow.com/questions/1813483/averaging-angles-again
    long sum = winddiravg[0];
    int D = winddiravg[0];
    for(int i = 1 ; i < WIND_DIR_AVG_SIZE ; i++)
    {
        int delta = winddiravg[i] - D;

        if(delta < -180)
            D += delta + 360;
        else if(delta > 180)
            D += delta - 360;
        else
            D += delta;

        sum += D;
    }
    winddir_avg2m = sum / WIND_DIR_AVG_SIZE;
    if(winddir_avg2m >= 360) winddir_avg2m -= 360;
    if(winddir_avg2m < 0) winddir_avg2m += 360;

    //Calc windgustmph_10m
    //Calc windgustdir_10m
    //Find the largest windgust in the last 10 minutes
    windgustmph_10m = 0;
    windgustdir_10m = 0;
    //Step through the 10 minutes
   for(int i = 0; i < 10 ; i++)
    {
        if(windgust_10m[i] > windgustmph_10m)
        {
            windgustmph_10m = windgust_10m[i];
            windgustdir_10m = windgustdirection_10m[i];
        }
    } 

    //Calc humidity
    humidity = myHumidity.getRH();
    //float temp_h = myHumidity.readTemperature();
    //Serial.print(" TempH:");
    //Serial.print(temp_h, 2);

    //Calc tempf from pressure sensor
    tempf = sensor.getTempF();
   // Serial.print(" TempP:");
   // Serial.println(tempf, 2);

    //Total rainfall for the day is calculated within the interrupt
    //Calculate amount of rainfall for the last 60 minutes
    rainin = 0;
    for(int i = 0 ; i < 60 ; i++)
        rainin += rainHour[i];

    //Calc pressure
    pressure = myPressure.readPressure();


    //Calc light level
    light_lvl = get_light_level();

    //Calc battery level
    batt_lvl = get_battery_level();
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level()
{
    float operatingVoltage = analogRead(REFERENCE_3V3);

    float lightSensor = analogRead(LIGHT);

    operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V

    lightSensor = operatingVoltage * lightSensor;

    return(lightSensor);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level()
{
    float operatingVoltage = analogRead(REFERENCE_3V3);

    float rawVoltage = analogRead(BATT);

    operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V

    rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin

    rawVoltage *= 20; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage

    return(rawVoltage);
}

//Returns the instataneous wind speed
float get_wind_speed()
{
    float deltaTime = millis() - lastWindCheck; //750ms

    deltaTime /= 1000.0; //Covert to seconds

    float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

    windClicks = 0; //Reset and start watching for new wind
    lastWindCheck = millis();

    windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

    /* Serial.println();
     Serial.print("Windspeed:");
     Serial.println(windSpeed);*/

    return(windSpeed);
}

//Read the wind direction sensor, return heading in degrees
int get_wind_direction()
{
   unsigned int adc;

    adc = analogRead(WDIR); // get the current reading from the sensor

    
    if (adc>380&&adc<460) return (0);
    //if (adc < 456) return (158);
    if (adc >460&&adc<550) return (45);
    //if (adc < 551) return (203);
    if (adc>550&&adc<690) return (90);
   // if (adc < 680) return (23);
    if (adc>690&&adc<790) return (315);
    //if (adc < 801) return (248);
    if (adc>790&&adc<890) return (135);
    //if (adc < 878) return (338);
    if (adc>890&&adc<910) return (270);
    //if (adc < 940) return (293);
    if (adc >910&&adc<960) return (225);
    if (adc>960&&adc<990) return (180);
    return (-1); // error, disconnected?
}


//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather()
{
    calcWeather(); //Go calc all the various sensors

    Serial.println();
    Serial.print("#,Direction du vent=");
    if(winddir==0)Serial.print(",  north wind (N)");
    //if(winddir==23)Serial.print("north-northeast wind (NNE)");
    if(winddir==45)Serial.print("northeast wind (NE)");
   // if(winddir==68)Serial.print("east-northeast wind (ENE)");
    if(winddir==90)Serial.print(" east wind (E)");
   // if(winddir==113)Serial.print(" east-southeast wind (ESE)");
    if(winddir==135)Serial.print(" southeast wind (SE)");
    //if(winddir==158)Serial.print("south-southeast wind (SSE)");
    if(winddir==180)Serial.print("south wind (S)");
    //if(winddir==203)Serial.print("south-southwest wind (SSW)");
    if(winddir==225)Serial.print("southwest wind (SW)");
   //if(winddir==248)Serial.print("north-northwest wind (NNW)");
    if(winddir==270)Serial.print(",  west wind (W)");
   //if(winddir==293)Serial.println(",  west-northwest wind (WNW)");
    if(winddir==315)Serial.print(",  northwest wind (NW)");
    //if(winddir==338)Serial.println(",  north-northwest wind (NNW)");

    Serial.print(winddir);//winddir
    Serial.println("°");
    Serial.print(",vitesse du vent=");
    Serial.print(windspeedmph*1.60934, 1);
    Serial.println("m/h");
    Serial.print(",Rafale du vent=");
    Serial.print(windgustmph, 1);
    Serial.println("m/h");
    Serial.print(",Direction du Rafale du vent=");
    if(windgustdir==0)Serial.print(",  north wind (N)");
    //if(windgustdir==23)Serial.print("north-northeast wind (NNE)");
    if(windgustdir==45)Serial.print("northeast wind (NE)");
   // if(windgustdir==68)Serial.print("east-northeast wind (ENE)");
    if(windgustdir==90)Serial.print(" east wind (E)");
   // if(windgustdir==113)Serial.print(" east-southeast wind (ESE)");
    if(windgustdir==135)Serial.print(" southeast wind (SE)");
    //if(windgustdir==158)Serial.print("south-southeast wind (SSE)");
    if(windgustdir==180)Serial.print("south wind (S)");
    //if(windgustdir==203)Serial.print("south-southwest wind (SSW)");
    if(windgustdir==225)Serial.print("southwest wind (SW)");
   //if(windgustdir==248)Serial.print("north-northwest wind (NNW)");
    if(windgustdir==270)Serial.print(",  west wind (W)");
   //if(windgustdir==293)Serial.println(",  west-northwest wind (WNW)");
    if(windgustdir==315)Serial.print(",  northwest wind (NW)");
    //if(windgustdir==338)Serial.println(",  north-northwest wind (NNW)");
    Serial.print(windgustdir); Serial.println("°");
    Serial.print(",Vitesse du vent_Moy2m=");
    Serial.print(windspdmph_avg2m, 1);
    Serial.println("m/h");
    Serial.print(",Direction du vent_Moy2m=");
    if(winddir_avg2m>=349||winddir_avg2m<11)Serial.print(",  north wind (N)");
    if(winddir_avg2m>=11&&winddir_avg2m<34)Serial.print("north-northeast wind (NNE)");
    if(winddir_avg2m>=34&&winddir_avg2m<56)Serial.print("northeast wind (NE)");
   if(winddir_avg2m>=56&&winddir_avg2m<79)Serial.print("east-northeast wind (ENE)");
    if(winddir_avg2m>=79&&winddir_avg2m<101)Serial.print(" east wind (E)");
    if(winddir_avg2m>=101&&winddir_avg2m<124)Serial.print(" east-southeast wind (ESE)");
    if(winddir_avg2m>=124&&winddir_avg2m<146)Serial.print(" southeast wind (SE)");
    if(winddir_avg2m>=146&&winddir_avg2m<169)Serial.print("south-southeast wind (SSE)");
    if(winddir_avg2m>=169&&winddir_avg2m<191)Serial.print("south wind (S)");
    if(winddir_avg2m>=191&&winddir_avg2m<214)Serial.print("south-southwest wind (SSW)");
    if(winddir_avg2m>=214&&winddir_avg2m<236)Serial.print("southwest wind (SW)");
   if(winddir_avg2m>=236&&winddir_avg2m<259)Serial.print("north-northwest wind (NNW)");
    if(winddir_avg2m>=259&&winddir_avg2m<281)Serial.print(",  west wind (W)");
   if(winddir_avg2m>=281&&winddir_avg2m<304)Serial.print(",  west-northwest wind (WNW)");
    if(winddir_avg2m>=304&&winddir_avg2m<326)Serial.print(",  northwest wind (NW)");
    if(winddir_avg2m>=326&&winddir_avg2m<349)Serial.print(",  north-northwest wind (NNW)");
    Serial.print(winddir_avg2m);Serial.println("°");
    Serial.print(",Rafale du vent_10m=");
    Serial.print(windgustmph_10m, 1);
    Serial.println("m/h");
    Serial.print(",Direction du Rafale du vent_10m=");
    if(windgustdir_10m==0)Serial.print(",  north wind (N)");
    //if(windgustdir_10m==23)Serial.print("north-northeast wind (NNE)");
    if(windgustdir_10m==45)Serial.print("northeast wind (NE)");
   // if(windgustdir_10m==68)Serial.print("east-northeast wind (ENE)");
    if(windgustdir_10m==90)Serial.print(" east wind (E)");
   // if(windgustdir_10m==113)Serial.print(" east-southeast wind (ESE)");
    if(windgustdir_10m==135)Serial.print(" southeast wind (SE)");
    //if(windgustdir_10m==158)Serial.print("south-southeast wind (SSE)");
    if(windgustdir_10m==180)Serial.print("south wind (S)");
    //if(windgustdir_10m==203)Serial.print("south-southwest wind (SSW)");
    if(windgustdir_10m==225)Serial.print("southwest wind (SW)");
   //if(windgustdir_10m==248)Serial.print("north-northwest wind (NNW)");
    if(windgustdir_10m==270)Serial.print(",  west wind (W)");
   //if(windgustdir_10m==293)Serial.println(",  west-northwest wind (WNW)");
    if(windgustdir_10m==315)Serial.print(",  northwest wind (NW)");
    //if(windgustdir_10m==338)Serial.println(",  north-northwest wind (NNW)");
    Serial.print(windgustdir_10m);Serial.println("°");
    Serial.print(",Humidite=");
    Serial.print(humidity, 1);
    Serial.println("%,");
    Serial.print(",Temperature=");
    Serial.print(tempf, 1);
    Serial.print(" F,");
    Serial.print(((tempf-32)*5)/9, 1);
    Serial.println(" °C,");
    Serial.print(",Pluie=");
    Serial.print(rainin, 2);
    Serial.println("   mm/h,");
    Serial.print(",Pluie quotidienne=");
    Serial.println(dailyrainin, 2);
    Serial.print(",Pression=");
    Serial.print(pressure, 2);
    Serial.print("Pa ,   ");
    Serial.print((pressure/100000), 2);
    Serial.println("Bar ,");
    Serial.print(",Niveau de batterie=");
    Serial.print(batt_lvl, 2);
    Serial.println("%,");
    Serial.print(",Niveau de lumière=");
    Serial.print(light_lvl*48.5, 2);
    Serial.println("%,");
    Serial.print(",");
    Serial.println("#");

}