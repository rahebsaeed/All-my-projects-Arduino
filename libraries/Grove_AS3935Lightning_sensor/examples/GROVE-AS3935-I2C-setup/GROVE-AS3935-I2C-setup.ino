
//*********************************************************************************
//***************  Designed for GROVE-AS3935-Lightning sensor   *******************
//***************  GROVE-AS3935-Lightning sensor HW rev.1.1     *******************
//***************  front of lightning fall detection sensor     ******************* 
//***************                                               *******************
//*************** first release: 1.0 - 12/15/2022               ******************* 
//*************** Author: Davide Cogliati                       *******************
//*********************************************************************************
//
//Abstract of the example:
//---------------------------------------------------------------------------------------------- 
//This example show how to change at startup  the sensor  internal register content in order
//to achieve the best setup for the environment where the sensor is installed.
//Please note that for every register not all values of parameter are valid, modify as suggested
//by the value interval.
//Some setup parameter influence greatly the behaviour of the sensor so modify and test if the
//value is satisfatory for your request.
//--------------------------------------------------------------------------------------------- 


      //----------------------------------------------------------------------------------
      //List of functions for operating the Grove-AS3935
      //----------------------------------------------------------------------------------
      //void gainAS3935 (unsigned int gain); //set gain of the sensor internal receiver circuit
      //void setDefault(void);               //set the internal register set up to default values
                                             //(Constructor default)
      //void calibrateAuto(void);            //self calibration of sensor and store in internal volatile memory
      //void writeToAS3935 (int dataWrite, int adr_Device, int adr_I2C); //write data to reg. trough adr_I2C
      //int  readFromAS3935 (int dataRead, int adr_Device, int adr_I2C); //read data from internal registers trough adr_I2C
      //unsigned int calculateIntensity (void); //weighted sum of the LSB, MSB, MMSB reg. to figure the intensity of the striking
      //unsigned int frontDistance(void);    //estimated distance from the appropriate internal register
      //unsigned int getNoise(void);        //noise level sensed  from sensor
      //void scanSensor(void);              //scan sensor registers to get all values of interest in one shot
      //void maskON(void);                  //mask sensor ON
      //void maskOFF(void);                 //mask sensor OFF
      //void setOutdoor(void);              //set for indoor position of sensor
      //void setIndoor(void);               //set for outdoor position of sensor
      //void powerDwn(void);                //power down sensor such as on shiny sunny days to prevent false alarms
      //void powerUpn(void);                //power up sensor for approaching storms or bad weather condition to get alarms
      //void clearStatOn(void);             //clears the cumulated statistical data in sensor during operation, 
                                            //resets internal calculations
      //void clearStatOff(void);            //keeps running calculations of sensor and statistical evaluations in activity
      //void setIrqfreq(void);              //set the IRQ pin to output the LCO oscillator frequency that is 
                                            //to be multiplied by the factor
      //void setIrqEvent(void);             //set the IRQ pin to give a trigger transition when an event has been sensed    
      //void tuneCapacitor(unsigned int);   //set the correction capacitor to get 500 kHz as close < 3,5%
      //void noiseFloorLevel(unsigned int); //the floor level above it is a good signal to examine
      //void watchDogLevel(unsigned int);   //intensity of signal to reject spurious noise
      //void lightningNum(unsigned int);    //number of minimum lightnings to superate in order to give an IRQ alarm
      //void rejectionSpike(unsigned int);  //spike rejection threshold to reject any spike isnot a good signal

 
//********************************************************************************
//********************************************************************************
//********************************************************************************

#include <GROVEAS3935.h>


//--------------------------------------------------------------------------------
//I2C ADDRESS : Default value is 3    
//              modify in the  #define Device_adress ...  
//--------------------------------------------------------------------------------
 

GROVEAS3935   Sensor;  //scope declaration of the functions and variables Public 
                       //in the library to Sensor

#define Device_address 3   //IMPORTANT, after running the example GROVE-AS3935-I2C-test 
                           //and I2C is working correctly, assign the
                           //HW programmed I2C Device address, by default is = 3.


#define ENVIRONMENT 0     //0= INdoor use of sensor, 1= OUTdoor use of sensor 
#define IRQ  1            //0= IRQ is LCO oscillator, 1= IRQ is Lightning event     
#define MASKING  0        //0= Mask is OFF, 1= Mask is ON   
#define POWER  1          //0= Power is OFF, 1= Power is ON  
#define DEFAULT_RESET   1 // 0= don't set to default values, 1= reset reg. to default value
#define GAIN 18           //ACCEPTED VALUES ARE:  0...31 default=18
#define WDT  2            //ACCEPTED VALUES ARE:  0...15 default=2
#define LNUM 0            //ACCEPTED VALUES ARE:  0...3  default=0
#define SPIKE_LEVEL  2    //ACCEPTED VALUES ARE:  0...15 default=2
#define NOISE_LEVEL 2     //ACCEPTED VALUES ARE:  0...7  default=2
 
int dataRead =0; 
unsigned int device;

void setup()
{   //-------------------------------------
    //Initialize HW  I2C address of board
    //-------------------------------------
      device = Device_address;
      Sensor.initializeDevice(device);
      Serial.begin(9600); 
      Serial.println("AS3935 LIGHTNING SENSOR setting UP");
      Serial.println("modified values in registers.");
      Serial.println("Starting....");
      Serial.println("------------------------------------");
      delay(1000);
      Wire.begin();
      Wire.setClock(100000); // set I2C  speed 

}

 
void loop()
{   //**************************************************************************
    //  This loop runs 1 time only to perform changes in setup
    //**************************************************************************
          // device= Device_adress;
           
    for (int times=0; times<1; times++)   
     {          
           //----- IRQ pin ---------------------------------------------------------
           Serial.println();               
           Serial.print("Perform setup for IRQ: ");  
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(IRQ);         // Print the character
        if(IRQ == 1)
          {
            Sensor.setIrqEvent(); 
          }
        if (IRQ == 0)
          {
            Sensor.setIrqfreq(); 
          }
           //----- environment indoor/outdoor  --------------------------------------
           Serial.println();               
           Serial.print("Perform setup for Environment: ");  
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(ENVIRONMENT);         // Print the character
        if(ENVIRONMENT == 1)
          {
            Sensor.setOutdoor();   
          }
        if (ENVIRONMENT == 0)
          {
            Sensor.setIndoor();
          }
          //----- Power ON/OFF   -----------------------------------------------------
           Serial.println();               
           Serial.print("Perform Power ON/OFF: ");   
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(POWER);         // Print the character
        if(POWER == 0)
          {
            Sensor.powerDwn();   
          }
        if (POWER == 1)
          {
            Sensor.powerUpn();
          }
          //----- Sensor masking for disturbs  ON/OFF   -------------------------------
           Serial.println();               
           Serial.print("Perform Mask ON/OFF: ");   
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(MASKING);         // Print the character
        if(MASKING == 1)
          {
            Sensor.maskON();  
          }
        if (MASKING == 0)
          {
            Sensor.maskOFF();
          }
          //----- GAIN AFE factor of the sensor           ------------------------------
           Serial.println();               
           Serial.print("Perform GAIN value: ");   
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(GAIN);         // Print the VALUE
           Sensor.gainAS3935(GAIN);
          //----- Watchdog treshold  of the sensor      -------------------------------
           Serial.println();               
           Serial.print("Perform Watchdog value: ");   
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(WDT);         // Print the VALUE
           Sensor.watchDogLevel(WDT);
           //----- Lightning number before alert  of the sensor      -----------------
           Serial.println();               
           Serial.print("Perform Lightning number value: ");   
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(LNUM);         // Print the VALUE
           Sensor.lightningNum(LNUM);
           //----- spike rejection level     -----------------------------------------
           Serial.println();               
           Serial.print("Perform spike rejection level: ");   
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(SPIKE_LEVEL);         // Print the VALUE 
           Sensor.rejectionSpike(SPIKE_LEVEL);
           //----- noise threshold level     -----------------------------------------
           Serial.println();               
           Serial.print("Perform noise threshold  level: ");   
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(NOISE_LEVEL);         // Print the VALUE 
           Sensor.noiseFloorLevel(NOISE_LEVEL);
           //-----------------------------------------------------------------------
           //----- THIS IS LAST OPTION SO TO OVERRIDE BEFORE CHANGES ! -------------
           //----- set to default registers   --------------------------------------
           //-----------------------------------------------------------------------
           Serial.println();               
           Serial.print("Perform setup for DEFAULT registers: ");  
           Serial.println();             
           Serial.print("Value programmed: ");
           Serial.print(DEFAULT_RESET);         // Print the character
        if(DEFAULT_RESET == 1)
          {
            Sensor.setDefault();
          }            
       //------------------------------------------
       //READING SETTINGS.....
       //------------------------------------------
       delay(200);    
       Serial.println();   
       Serial.println();        
     }  
        //---------------------------------------------
        //readout of the programmed REGISTER in sensor
        //---------------------------------------------
        int NOISEFLOOR;
        int SPIKEREJECTION; 
        int WATCHDOGTHRESHOLD; 
        //-------------------------------------------------------------------------  
        Serial.println();             
        Serial.print("REGISTER reading to see if modified values are stored");   
        Serial.println();  
        Serial.print("*****************************************************");   
        Serial.println();          
        Serial.print("DEVICE REGISTERS [0X00] to [0x08]  and [0x3A, 0X3B] ");
        int c;
        for (int i=0; i<9; i++)
          { 
       
            //------------------------------------------------------------------------- 
            Serial.println();             
            Serial.print("REGISTER address = 0x0");   
            Serial.print(i);               // Print the ADDRESS
            //-------------------------------------------------------------------------                                     
            dataRead= Sensor.readFromAS3935 (Device_address, i); //read data from internal 
                                                                 //registers trough adr_I2C
            delay(200);
            c=0;
            c = dataRead ;//Wire.read();    // Receive a byte as character 
            if (i== 1)
                {
                 NOISEFLOOR= c;
                 NOISEFLOOR = NOISEFLOOR & (0b01110000);
                 NOISEFLOOR = NOISEFLOOR / 16; ; //shift  to right 4 bits.
                }
            if (i== 2)         
                {
                 SPIKEREJECTION=   c;
                 SPIKEREJECTION = SPIKEREJECTION & (0b00001111);
                }
            if (i== 1)
                {
                  WATCHDOGTHRESHOLD= c;
                  WATCHDOGTHRESHOLD =  WATCHDOGTHRESHOLD & (0b00001111);
                }     
            Serial.print(" *** REGISTER data= [ ");         // Print the character
            Serial.print((unsigned int)c);
            Serial.print(" ]");
            delay(200);
          }
            //------------------------------------------------
            //OUTPUT OF MAIN SETUP
            //------------------------------------------------
            Serial.println();
            Serial.println("*************************************************************************");
            Serial.println("* THESE REGISTER INFLUENCE SYSTEM DETECTION FIGURE, MODIFY IF NECESSARY *");
            Serial.println("*************************************************************************");
            Serial.println();
            Serial.print("Noise floor set: ");
            Serial.println(NOISEFLOOR,DEC);
            Serial.print("Spike rejection is: ");
            Serial.println(SPIKEREJECTION,DEC);
            Serial.print("WATCHDOG setpoint is: ");
            Serial.println(WATCHDOGTHRESHOLD,DEC);
            //---------------------------------------------
       while(1)
        { 
          delay(100);    //pausing in endless loop
        } 


   
    //----------------END void LOOP ----------------------------------------------------------------------------
} 
