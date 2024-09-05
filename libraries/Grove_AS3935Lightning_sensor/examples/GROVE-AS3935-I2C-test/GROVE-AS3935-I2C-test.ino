
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
//Starting the example the I2C bus is scanned to get the I2C device address programmed by hardware
//(soldering taps) on the sensor. If none is found the cycle is endless otherwise the sensor is read.
//This example use the function "readFromAS3935" of the library to read the content of the internal
//setup registers of the AS3935 sensor device.
//The test is an endless loop so can  test several times the device or more than 1 device connected.
//----------------------------------------------------------------------------------------------------
 
//********************************************************************************
//********************************************************************************
//********************************************************************************

#include <GROVEAS3935.h>

//--------------------------------------------------------------------------------
//I2C ADDRESS SCANNER - TEST         |
//---------------------------------------
//checks the presence of a valid AS3935 sensor on I2C  bus (this part can be omitted 
//in final application but use it during setting up for first time the I2C address
//----------------------------------------------------------------------------------


//#define Device_address 3   //IMPORTANT, after running the example GROVE-AS3935-I2C-test 
                             //and I2C is working correctly, assign the
                             //HW programmed I2C Device address, by default is = 3
                             //in the sketch.

GROVEAS3935   Sensor;  //scope declaration of the functions and variables Public in the library to Sensor
int dataRead =0; 
int device;

void setup()
{
 
  Serial.begin(9600);   
      Serial.println("AS3935 LIGHTNING SENSOR| i2C SCANNER");
      Serial.println("Starting....");
      Serial.println("------------------------------------");
      delay(1000);
      
  Wire.begin();
  Wire.setClock(100000); // set I2C  speed 

}

 
void loop()
{   //**************************************************************************
    //  This loop runs continuosly until a valid I2C device is found
    //**************************************************************************
  int done =0;       //flag to start self test on I2C sensor address
  int c;
  int indevice;      //device address under test!
  
  indevice = 0;    //reset address value
    while (!done)
    {
    indevice = indevice +1 ; //increment address under test
    if (indevice > 7)
        indevice=0;
       
    Wire.beginTransmission(indevice);
    Wire.requestFrom(indevice, 1);    // Request 1 bytes from slave device 
    Wire.endTransmission();
    
   
       Serial.print("scanning device = 0x0");         // Print the character
       Serial.print(indevice);       
       Serial.println(" ");
      
    delay(250);
  
    //-----------------------------------------------------------
    // SCANNED & FOUND VALID  ADDRESS SHOWING REGISTER CONTENT
    //-----------------------------------------------------------
    //---------------------------------------------------------------
    //Initialize HW  I2C address of board with address found valid !
    //---------------------------------------------------------------
     device = indevice;
     Sensor.initializeDevice(device);
    //---------------------------------------------------------------
    while(Wire.available()) 
    {
        c = Wire.read();    // Receive a byte as character
    
           Serial.println();               
           Serial.print("data=.......... valid address .................. ");  //Print character RECEIVED
           Serial.println();             
           Serial.print("found ADDR: 0X0");
           Serial.print(indevice);
           Serial.print(" -->  answer: ");
           Serial.print("[ ");
           Serial.print((int)c);         // Print the character
           Serial.print(" ]");           
     
        //-------------------------------------------------------------------------------------------------
        //registers FOR SETTING I2C ADDRESS: A0 & A1 using the soldering TAPS on the GROVE SENSOR A0, A1
        //-------------------------------------------------------------------------------------------------
        //A0        A1         ADR  0X0 
        //****************************************************
        //SOLDER   SOLDER    < NOT ALLOWED !!!!>         *****
        //SOLDER   NO        0X01    ;SOLDER TAP A0      *****
        //NO       SOLDER    0X02    ;SOLDER TAP A1      *****
        //NO       NO        0X03    ;NO SOLDERED TAPS   *****
        //----------------------------------------------------
        //--------------------------------------------------------------------------------------------
        //LIST OF THE  SETUP   REGISTERS OF THE AS3935   READOUT ON I2C BUS
        //--------------------------------------------------------------------------------------------
        //
        //AS3935_REGISTER_NAMES     |  write=W / read=R  |   reserved bits = x, modifiable bits= a
        //--------------------------------------------------------------------------------------------
        // AFE_GAIN          = 0x00     W/R               > XXaa aaaa                            
        // THRESHOLD,        = 0X01     W/R               > aaaa aaaa
        // LIGHTNING_REG     = 0X02     W/R               > Xaaa aaaa
        // INT_MASK_ANT      = 0X03     W/R               > aaaa Xaaa
        // ENERGY_LIGHT_LSB  = 0X04     R                 > -
        // ENERGY_LIGHT_MSB  = 0X05     R                 > -
        // ENERGY_LIGHT_MMSB = 0X06     R                 > -
        // DISTANCE          = 0X07     R                 > -
        // DISP /TUNING CAP  = 0X08     W/R               > aaaa aaaa
        // CALIB_TRCO        = 0x3A     R                 > -
        // CALIB_SRCO        = 0x3B     R                 > -
        // PRESET            = 0x3C     W/R               > aaaa aaaa  <SEND DIRECT COMMAND(set all to default)!!!
        // CALIB_RCO         = 0x3D     W/R               > aaaa aaaa  <SEND DIRECT COMMAND(self calibration) !!!
        //--------------------------------------------------------------------------------------------
        //-------------------------------------------------------------------------
        //SCAN INTERNAL SETTING REGISTERS AND SHOW SETUP SAVED IN I2C   !!!
        //-------------------------------------------------------------------------
        int NOISEFLOOR;
        int SPIKEREJECTION; 
        int WATCHDOGTHRESHOLD; 
        //-------------------------------------------------------------------------  
        Serial.print("DEVICE REGISTERS [0X00] to [0x08]  and [0x3A, 0X3B] ");
        for (int i=0; i<9; i++)
          { 
             
            //int dataRead;  
            //------------------------------------------------------------------------- 
            Serial.println();             
            Serial.print("REGISTER address = 0x0");   
            Serial.print(i);               // Print the ADDRESS
            //-------------------------------------------------------------------------                                     
            dataRead= Sensor.readFromAS3935 (device, i); //read data from internal registers trough adr_I2C
            delay(200);        
                 c=0;
                 c = dataRead ;//Wire.read();    // Receive a byte as character 
                // c =  Wire.read();    // Receive a byte as character
               if (i== 1)
                {
                 NOISEFLOOR= c;
                 NOISEFLOOR = NOISEFLOOR & (0b01110000);
                 NOISEFLOOR = NOISEFLOOR / 16; ; //shift  to right 4 bits.
                }
               if (i== 1)         
                {
                 SPIKEREJECTION= c;
                 SPIKEREJECTION = SPIKEREJECTION & (0b00001111);
                }
                if (i== 2)
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
    
        }   
        delay(1000); 
     }
    //----------------END void LOOP ----------------------------------------------------------------------------
} 
