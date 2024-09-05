
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
//------------------------------------------------------------------------------------------------------
//This example makes a reset to DEFAULT values so to restart in a known setup the sensor
// 
// 
//----------------------------------------------------------------------------------------------------
 
#include <GROVEAS3935.h>


//--------------------------------------------------------------------------------
//I2C ADDRESS : Default value is 3    
//              modify in the  #define Device_adress ...  
//--------------------------------------------------------------------------------
 

GROVEAS3935   Sensor;  //scope declaration of the functions and variables Public in 
                       //the library to Sensor

#define Device_address 3   //IMPORTANT, after running the example GROVE-AS3935-I2C-test and I2C is working
#define DEFAULT_RESET  1   // 0= don't set to default values just read ACTUAL values, 
                           // 1= reset registers to default value

int dataRead =0; 
int device;

void setup()
{
  
    //-------------------------------------
    //Initialize HW  I2C address of board
    //-------------------------------------
    device = Device_address;
    Sensor.initializeDevice(device);
      Serial.begin(9600); 
      Serial.println("AS3935 LIGHTNING SENSOR| i2C SCANNER");
      Serial.println("Resetting to DEFAULT ....");
      Serial.println("------------------------------------");
      delay(1000);
      
  Wire.begin();
  Wire.setClock(100000); // set I2C  speed 

}

 
void loop()
{   //**************************************************************************
    //  This loop runs 1 time only to perform changes in setup
    //**************************************************************************
  
    for (int times=0; times<1; times++)   
     {    
      
        if(DEFAULT_RESET == 1)
          {
            Sensor.setDefault();
          }     

        if(DEFAULT_RESET == 0)
          {
            Serial.print("No refresh to DEFAULT values, just read register content");
            Serial.println();
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
        Serial.print("DEVICE REGISTERS [0X00] to [0x08]  and [0x3A, 0X3B] ");
        int c;
        for (int i=0; i<9; i++)
          { 
       
            //------------------------------------------------------------------------- 
            Serial.println();             
            Serial.print("REGISTER address = 0x0");   
            Serial.print(i);               // Print the ADDRESS
            //-------------------------------------------------------------------------                                     
            dataRead= Sensor.readFromAS3935 (Device_address, i); //read data from internal registers trough adr_I2C
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
             if(DEFAULT_RESET == 1)
          {
            Serial.println();
            Serial.println("*************************************************************************");
            Serial.println("* The sensor has been now loaded DEFAULT setup   ************************");
            Serial.println("*************************************************************************");
          }   
            
       while(1)
        { 
          delay(100);    //pausing in endless loop
        } 


   
    //----------------END void LOOP ----------------------------------------------------------------------------
} 
