
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
//-----------------------------------------------------------------------------------------------
//This example show how to perform sensor calibration to get closest resonance value 500 kHz
//as possible. Good calibrations achieve a deviation < 3.5% from 500 kHz resonance frequency.
//If measure of the IRQ resonance frequency is > 3.5% then adjust the capacitor add-on register
//setup as hown in the example. The measure of the square wave output on IRQ pin is to be 
//multiplied by the LCO_FDIV [7:6] R/W Frequency division ratio  for antenna tuning, default
//value is bit7=0, bit6=0.
//------------------------------------------------
//Frequency Division Ratio for the Antenna Tuning-
//------------------------------------------------
//Division Ratio  REG0x03[7]  REG0x03[6]
//    16              0            0
//    32              0            1
//    64              1            0
//    128             1            1
//------------------------------------------------------------------------------------------------
//Tuning capacitor register: [0x08]
//
//TUN_CAP [3:0] 0000 Internal Tuning Capacitors (from 0 120pF in steps of 8pF), 
//the value 0000 for bits 0...3 is the default
//------------------------------------------------------------------------------------------------
 

 
//********************************************************************************
//********************************************************************************
//********************************************************************************

#include <GROVEAS3935.h>


//--------------------------------------------------------------------------------
//I2C ADDRESS : Default value is 3    
//              modify in the  #define Device_adress ...  
//--------------------------------------------------------------------------------
 

GROVEAS3935   Sensor;  //scope declaration of the functions and variables Public in 
                       //the library to Sensor.

#define Device_address 3   //IMPORTANT, after running the example GROVE-AS3935-I2C-test 
                          //and I2C is working correctly, assign the HW programmed I2C
                          //Device address, by default is = 3.


#define SELF_CALIBRATE 0  //0= NO self calibration, 1= YES make self calibration 
#define IRQ  0            //0= IRQ is LCO oscillator, 1= IRQ is Lightning event 
#define CAPACITOR_VALUE 0 //value range 0...15. This  adds a calibration 
                          //(internal) capacitor conformal to the following note:
//-----------------------------------------------------------------------------------  
//Tuning capacitor register: [0x08]
//
//TUN_CAP [3:0] 0000 Internal Tuning Capacitors (from 0 120pF in steps of 8pF), 
//the value 0000 for bits 0...3 is the default
//----------------------------------------------------------------------------------- 
 
int device;

void setup()
{   
    //-------------------------------------
    //Initialize HW  I2C address of board
    //-------------------------------------
    device = Device_address;
    Sensor.initializeDevice(device);
    
    Serial.begin(9600); 
    Serial.println("AS3935 LIGHTNING SENSOR CALIBRATION");
    Serial.println("Starting calibration....");
    Serial.println("------------------------------------");
    delay(1000);
    //---------------------------------------------------  
    Wire.begin();
    Wire.setClock(100000); // set I2C  speed 

}

 
void loop()
{   //**************************************************************************
    //  This loop runs 1 time only to perform changes in LCO frequency (<3.5%)
    //**************************************************************************
    //measure with oscilloscope frequency of square wave on pin IRQ
    //remember to multiply for DIVSION RATIO to get final frequncy!
    //--------------------------------------------------------------------------
          
     for (int times=0; times<1; times++)   
      {    
           //----- IRQ pin select for LCO osc. output    ----------------------------
           Serial.println();               
           Serial.print("Perform setup for IRQ to LCO frequncy output: ");  
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
        //----- time to measure IRQ pin LCO frequency before change -----------------
        //------------------------------------
        //bit0..3 Internal Tuning Capacitors (from 0 to 120pF in steps of 8pF)
        //(000 default value). Get best match to 500 kHz resonance frequency
        //----------------------------------------------------------------------------
        //
        //bit5    Display TRCO on IRQ pin  = 0 (default value) 
        //        put it to "1" to enable frequency square wave OUT on INT pin of the board !
        //bit6    Display SRCO on IRQ pin  = 0 (default value)
        //        put it to "1" to enable frequency square wave OUT on INT pin of the board !
        //bit7    Display LCO on IRQ pin   = 0 (default value)
        //        put it to "1" to enable antenna resonance frequency square wave OUT on INT
        //        pin of the board !
        //-------------------------------------
        //TRCO -  Timer RCO Oscillators 1.1MHz
        //SRCO -  System RCO at 32.768kHz
        //LCO  -  Frequency of the Antenna
        //-------------------------------------
        //--------------------------------------------------------------------------- 
        //STEPS TO DEBUG HARDWARE THE RESONANCE FREQUENCY OF THE BOARD:
        //--------------------------------------------------------------
        //1)  Observing LCO 500 Khz center resonance frequency requires to connect an 
        //    oscilloscope probe to the IRQ pin of the Seed Grove AS3935
        //2)  Then place bit 7= 1 and bit 5=0, bit 6=0;
        //3)  Then start application after programming with this Sketch.
        //4)  Finally the frequency measured must be multiplied by the internal 
        //    DIVISION FACTOR as shown below:
        //--------------------------------------------------------------------------- 
        // Division Ratio REG0x03[7] REG0x03[6]
        //  16          0               0
        //  32          0               1
        //  64          1               0
        //  128         1               1
        //--------------------------------------------------------------------------- 
        //In this case DIV RATIO= 16, so if the measure is 31.9 kHz this is finally 
        //510.4 kHz 31.9 x 16), and is a 2.0 % variation and is OK because  the data 
        //sheet suggests to stay < 3.5 % to achieve best results with the board.
        //--------------------------------------------------------------------------- 
           Serial.println();
           Serial.print("Time wait for 10 seconds to measure before modification... ");  
           Serial.println(); 
           delay(10000);  //time= 10 seconds
           
          
           //----- SELF CALIBRATION          -----------------------------------------
        if(SELF_CALIBRATE)
          { 
            Serial.print("Performing self calibration: ");  
            Serial.println(); 
            Sensor.calibrateAuto();
            
          }
           //----- NO SELF CALIBRATION but modify Tuning capacitor   -----------------
        if(!SELF_CALIBRATE)
          {
            Serial.print("Modify calibration cap. value: ");  
            Serial.print(CAPACITOR_VALUE);  
            Serial.println(); 
            Sensor.tuneCapacitor(CAPACITOR_VALUE); 
          } 
       //------------------------------------------
       //READING SETTINGS.....
       //------------------------------------------
       delay(200);   
       Sensor.setIrqEvent(); 
       Serial.println();   
       Serial.println();   
       Serial.print("End procedure, setting now to EVENT trigger on IRQ pin !");  
       Serial.println(); 
       while(1)     //endless stop, end of procedure calibration
       delay(200);
            
     }  
 
    //----------------END void LOOP ------------------------------------------------------
} 
