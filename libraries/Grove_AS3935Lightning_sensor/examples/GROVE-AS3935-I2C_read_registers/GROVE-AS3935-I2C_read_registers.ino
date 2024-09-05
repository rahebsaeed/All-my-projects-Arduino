
//*********************************************************************************
//***************  Designed for GROVE-AS3935-Lightning sensor   *******************
//***************  GROVE-AS3935-Lightning sensor HW rev.1.1     *******************
//***************  front of lightning fall detection sensor     ******************* 
//***************                                               *******************
//*************** first release: 1.0 - 12/15/2022               ******************* 
//*************** Author: Davide Cogliati                       *******************
//*********************************************************************************
//
//Abstract of the example: READ INTERNAL SETUP REGISTERS of GROVE-AS3935-Lightning sensor
//------------------------------------------------------------------------------------------------------
//This example show how to read internal setup registers of the sensor. Please mind that some 
//bit of the registers are only READABLE (READ MODE ONLY) others are READ/WRITE and can be modified 
//as needed. To read or write the sensor registers use the following instructions to perform random
//operation, otherwise use the specialized instructions to the item is intended to be modified. 
//
//READ:   unsigned int  readFromAS3935 (int dataRead, int adr_Device, int adr_I2C);
//WRITE:  void writeToAS3935 (int dataWrite, int adr_Device, int adr_I2C);
// 
//----------------------------------------------------------------------------------------------------


#include <GROVEAS3935.h>


//--------------------------------------------------------------------------------
//I2C ADDRESS : Default value is 3    
//              modify in the  #define Device_adress ...  
//--------------------------------------------------------------------------------
 

GROVEAS3935   Sensor;  //scope declaration of the functions and variables Public in the library to Sensor

#define Device_adress 3   //IMPORTANT, after running the example GROVE-AS3935-I2C-test and I2C is working
                          //correctly, assign the HW programmed I2C Device address, by default is = 3.
 
 
int dataRead =0; 
unsigned int device;

void setup()
{
  Serial.begin(9600); 
  
  
      Serial.println("AS3935 LIGHTNING SENSOR READER");
      Serial.println("Starting REGISTER reading");
      Serial.println("------------------------------------");
      delay(1000);
      
  Wire.begin();
  Wire.setClock(100000); // set I2C  speed 

}

 
void loop()
{   //**************************************************************************
    //  This loop runs endless to read during time the registers
    //**************************************************************************

        device= Device_adress; 
        //---------------------------------------------
        //readout of the programmed REGISTER in sensor
        //---------------------------------------------
        int NOISEFLOOR;
        int SPIKEREJECTION; 
        int WATCHDOGTHRESHOLD; 

       if(true)
    { 
        //-------------------------------------------------------------------------  
        Serial.print("DEVICE REGISTERS [0X00] to [0x08]  ");
        int c;
        for (int i=0; i<9; i++)
          { 
       
            //------------------------------------------------------------------------- 
            Serial.println();             
            Serial.print("REGISTER read address = 0x0");   
            Serial.print(i);               // Print the ADDRESS
            //-------------------------------------------------------------------------                                     
            dataRead= Sensor.readFromAS3935 (Device_adress, i); //read data from internal registers trough adr_I2C
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
            
                          //--------------------------------------------------------
            delay(300);    //pausing in endless loop - fix as preferred !!! ---------
                          //--------------------------------------------------------
      }

   
    //----------------END void LOOP ----------------------------------------------------------------------------
} 
