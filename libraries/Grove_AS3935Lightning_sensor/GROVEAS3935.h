
/*
//*********************************************************************************
//***************  Designed for Seeed Grove sensor developments *******************
//***************  GROVE-AS3935  LIGHTNING SENSOR               *******************
//***************  front of lightning fall detection sensor     ******************* 
//***************                                               *******************
//*************** first release: 1.0 - 12/15/2022               ******************* 
//*************** Author: Davide Cogliati                       *******************
//*********************************************************************************
//                                                              *******************
//                HEADER FILE  FOR LIBRARY                      *******************
//*********************************************************************************
*/ 
#ifndef GROVEAS3935_h
#define GROVEAS3935_h

#include <Arduino.h>
#include <Wire.h>     /*FOR I2C MODE COMUNICATION BUS*/


//-----------------------------
//constants
//-----------------------------
#define SET_INDOOR  0b00100100  //this is the suggested value for indoor gain of the sensor, modify if necessary  (bit 1,2,3,4,5 only)
#define SET_OUTDOOR 0b00011100  //this is the suggested value for outdoor gain of the sensor, modify if necessary (bit 1,2,3,4,5 only)
#define IRQ_LCO     0b10000000  //set IRQ pin to show LCO frequency     
#define IRQ_LIGHTNING 0b00000000  //set IRQ pin to show event Lightning trigger
#define MASK_ON     0b00100000  //mask the disturber
#define MASK_OFF    0b00000000  //UNmask the disturber
#define POWER_DOWN  0b00000000  //bit 0 to 0 is power down
#define POWER_UP    0b00000001  //bit 0 to 1 is power up
#define CLEAR_STAT_ON   0b10111111
#define CLEAR_STAT_OFF  0b01000000
#define CALIBRATE_AUTO  0X96  //direct command
#define SET_DEFAULT     0X96  //direct command


  
class GROVEAS3935
{
  private:
  
      int _device;


  public: 
      //-----------------------------
      //variables
      //----------------------------- */
      unsigned int  tuningCap;          //tuning capacitor add on, level 0 to 15, from 0 to 120pF in steps of 8pF
      unsigned int  numberLightning;    //number of lightnings before alert
      unsigned int  noiseLevel;         //noise level to filter ambient electromagnetic noise, values: 0 to 7
      unsigned int  spikeReject;        //spike rejection threshold, values: 0 to 15
      unsigned int  watchDog;           //watchdog threshold, the signal is to be higher this threshold to be validated, values: 0 to 15
      unsigned int  distanceEstimated;  //estimated distance in km of the front of storm 0 (overhead) to 40 km
    
      unsigned int  strikeLSB;          //LSB of the intensity recorded 
      unsigned int  strikeMSB;          //MSB of the intensity recorded
      unsigned int  strikeMMSB;         //MMSB of the intensisty recorded
      unsigned int  strikeEval;         //a composition of LSB, MSB, MMSB to an intensity evaluation factor
      unsigned int  interruptVal;       //interrupt produced by noise, disturbance or a good Lightning signal (or very similar)
      unsigned int  gain;                //gain factor of the sensor 
               int  device;             //address of device for I2C bus can be: 1,2,3
               int  ndata;                  //general use variable 
               int  dataWrite;
               int  adr_I2C;  
               int  data_read;      

 
   /* //--------------------------------------------
      //functions for operating the Grove-AS3935
      //-------------------------------------------- */
  public:     
      void gainAS3935 (unsigned int gain); //set gain of the sensor internal receiver circuit
      void setDefault(void);               //set the internal register set up to default values (Constructor default)
      void calibrateAuto(void);            //make self calibration of sensor, this is stored in internal volatile memory
      void writeToAS3935 (int dataWrite, int adr_Device, int adr_I2C); //write data to the internal registers trough adr_I2C
      int  readFromAS3935 ( int adr_Device, int adr_I2C); //read data from internal registers trough adr_I2C
      unsigned int calculateIntensity (void); //weighted sum of the LSB, MSB, MMSB registers to figure the intensity of the striking
      unsigned int frontDistance(void);    //estimated distance from the appropriate internal register
      unsigned int getNoise(void);          //noise level sensed  from sensor
      void scanSensor(void);              //scan sensor registers to get all values of interest in one shot
      void maskON(void);                  //mask sensor ON
      void maskOFF(void);                    //mask sensor OFF
      void setOutdoor(void);              //set for indoor position of sensor
      void setIndoor(void);               //set for outdoor position of sensor
      void powerDwn(void);                //power down sensor such as on shiny sunny days to prevent false alarms
      void powerUpn(void);                //power up sensor for approaching storms or bad weather condition to get alarms
      void clearStatOn(void);             //clears the cumulated statistical data in sensor during operation, resets internal calculations
      void clearStatOff(void);            //keeps the running calculations on the sensor and statistical evaluations in activity
      void setIrqfreq(void);              //set the IRQ pin to output the LCO oscillator frequency that is to be multiplied by the factor
      void setIrqEvent(void);             //set the IRQ pin to give a trigger transition when an event has been sensed    
      void tuneCapacitor(unsigned int tuningCap);   //set the correction capacitor to get 500 kHz as close < 3,5%
      void noiseFloorLevel(unsigned int noiseLevel ); //the floor level above it is a good signal to examine
      void watchDogLevel(unsigned int watchDog );   //intensity of signal to reject spurious noise
      void lightningNum(unsigned int numberLightning );    //number of minimum lightnings to superate in order to give an IRQ alarm
      void rejectionSpike(unsigned int spikeReject);  //spike rejection threshold to reject any spike isnot a good signal
      void initializeDevice(int device);        //initialize I2C Device ADDRESS (physical address) of the board
};

 
#endif

/*
//-----------------------------------------------------------------------------
//------------------ END OF *.h  file -----------------------------------------
//-----------------------------------------------------------------------------
*/
