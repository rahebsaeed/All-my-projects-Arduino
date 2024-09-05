
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
//MONITOR AND PLOT EVENTS detected by  GROVE-AS3935-Lightning sensor
//--------------------------------------------------------------------------------- 
//This example demonstrates how to  use this sensor in a very easy way to export
//trough I2C the events  captured by the sensor. The events can be shown in plai 
//text form or in a graphical plotter mode.  
//To choose how to show results select ONE ONLY  OF THE  "RESULT_MODE" in 
//the "#define RESULT_MODE"  define assignment. 
//
//-------------------------------------------------------------------------------- 


#include <GROVEAS3935.h>


//--------------------------------------------------------------------------------
//I2C ADDRESS : Default value is 3    
//              modify in the  #define Device_adress ...  
//--------------------------------------------------------------------------------
 

GROVEAS3935   Sensor;  //scope declaration of the functions and variables Public 
                       //in the library to Sensor

#define Device_adress 3   //IMPORTANT, after running the example GROVE-AS3935-I2C-test 
                          //and I2C is working correctly, assign the HW programmed I2C 
                          //Device address, by default is = 3.
                          
#define RESULT_MODE   0   //0= LITERAL mode, plain text (SELECT SERIAL MONITOR on Arduino menu)
                          //1= PLOTTER graph mode (SELECT SERIAL PLOTTER on Arduino menu)
 
int dataRead =0; 
int device;
unsigned char LITERAL_mode;
unsigned char GRAPH_mode;

void setup()
{ 
  //---------------------------------------------------------------
  //Initialize HW  I2C address of board with address found valid !
  //---------------------------------------------------------------
  device = Device_adress;
  Sensor.initializeDevice(device);
  //---------------------------------------------------------------
  Serial.begin(9600); 
  Wire.begin();
  Wire.setClock(100000); // set I2C  speed 

  if (!RESULT_MODE)
  {
  LITERAL_mode =1;
  GRAPH_mode =0;
  }
  
  if (RESULT_MODE)
  {
  LITERAL_mode =0;
  GRAPH_mode =1;
  }
}

 
void loop()
{   //**************************************************************************
    //  This loop runs endless polling and reading during time the registers
    //**************************************************************************
        int i=0;
        int INT_struck ; //event interrupt register
        int N_lightning; //number of lightnings detected
        int Av_distance; //average distance extimation
        int NOISE;       //interrupt for high noise level
        int Disturber;   //interrupt for disturber detected
        int L_struck;    //interrupt for lightning detected
        int Distance;    //estimated hit distance
        int Energy_LSB;  //energy single event LOW BYTE
        int Energy_MSB;  //energy single event HIGH BYTE
        int Energy_MMSB; //energy single event AV.Energy content
        int Calculate_Intensity;  //this calculates global energy by
                                  //weighting LSB, MSB, MMSB reg.
                                  //Can be modified in the library.
        int marker;      //marker of X position of data plotting
        if(true)
        { 
             //--------------------------------------------------------------------------
             //WAITING CYCLE - ENDLESS - LOOKING FOR LIGHTNINGS
             //variables from AS3935 during operative cycle
             //--------------------------------------------------------------------------
             //
             //-------------------------------------------------
             //-------------------------------------------------
             //GRAPH SCALING MIN-MAX: MODIFY AS YOU LIKE!!!
             //-------------------------------------------------
             int  MAX_scale =30;        //setting max Y of plotting
             int  min_scale =0;         //setting min Y of plotting
             int  c=0;                  //generic use variable
             //-------------------------------------------------
             //PRINT OUT METHOD:
             //---------------------------------------------------------------------------
             //#define RESULT_MODE 0 //LITERAL mode, text (SERIAL MONITOR on Arduino menu)
             //#define RESULT_MODE 1 //PLOTTER graph mode (SERIAL PLOTTER on Arduino menu)
             //---------------------------------------------------------------------------
             //*****************************************
             //SET UP THE PLOTTER X-Y READINGS (X= time)
             //*****************************************
            Serial.print("MAX"); //max line marker
            Serial.print(" ");
            Serial.print("min");//min line marker 
            Serial.print(" ");
            //*****************************************
            //SETUP LABELS NAME
            //*****************************************
            Serial.print("NOISE");
            Serial.print(" ");
            Serial.print("Disturber");
            Serial.print(" ");
            Serial.print("Distance");
            Serial.print(" ");
            Serial.print("Energy_MSB/20");  //SCALED FACTOR /20
            Serial.print(" ");
            Serial.print("Energy_MMSB");
            Serial.print(" ");
            Serial.print("Tot.Energy/100");
            Serial.print(" ");
            Serial.print("Xpos");
            Serial.print(" ");
            Serial.println();
            //**************************************************
            //**************************************************
            // STARTING ENDLESS OBSERVATION CYCLE FOR EVENTS
            //**************************************************
            //**************************************************
             while (1)
             {
               if (LITERAL_mode ==1)
              { 
               Serial.println();
               Serial.print(" *** WAITING FOR LIGHTNING - WATCHING ");         // Print the character
               Serial.println();
              } 
         
               //------------------------------------------------------------------
               //CHECK FOR INCOMING INT MESSAGE ON REG 0X03 bit= INT_L ---> "1000"
               //------------------------------------------------------------------
              c=0;
              i= 0x03;
              dataRead= Sensor.readFromAS3935 (device, i); //read data from internal registers trough adr_I2C
              
              //------------------------
              //CLEAN the variables
              //------------------------
              NOISE =0 ;
              Disturber =0;
              L_struck =0 ;
              Distance =0 ;
              Energy_LSB =0 ;
              Energy_MSB =0 ;
              Energy_MMSB =0 ;
              Calculate_Intensity=0;
              marker=3;            //kept raised to be visible
              //------------------------
              c = dataRead ;       // Receive a byte as character 
              
              INT_struck = c;
              NOISE  =INT_struck & (0b00000001);
              Disturber  =INT_struck & (0b00000100);
              L_struck  =(INT_struck & (0b00001000))/8;  //this is a flag bit so can be 0 or 1.
              if (LITERAL_mode ==1)
              { 
                Serial.print("L_struck (event striking): ");
                Serial.print(L_struck);
                Serial.println();              
              }
           //------------------------------------------------------------------------------------------------
           //DEBUG ALL RESULTS careless about "L_STRUCK"!!!
           //------------------------------------------------------------------------------------------------
           //L_struck =1;//  --->  this show all messages, testing purpose-DEBUG of AS3935 system remove "//"
           //
           //-------------------------------------------------------------------------------------------------
          if ((GRAPH_mode == 1) & (L_struck ==0))
            {
            //------------------------------------------------
            //GRAPH- PLOTTER  REFRESHING   LAYOUT
            //------------------------------------------------
          
            Serial.print(MAX_scale);
            Serial.print(" ");
            
            Serial.print(min_scale);
            Serial.print(" ");
            //------------------------------------------------
            Serial.print(NOISE,DEC);
            Serial.print(" ");
            Serial.print(Disturber,DEC);
            Serial.print(" ");
            Serial.print(Distance,DEC);
            Serial.print(" ");
            Energy_MSB = Energy_MSB/20;   //JUST A SCALING FACTOR TO PLOT !
            Serial.print(Energy_MSB,DEC);
            Serial.print(" ");
            Serial.print(Energy_MMSB,DEC);
            Serial.print(" ");
            Calculate_Intensity = Calculate_Intensity/100; //JUST A SCALING FACTOR TO PLOT !
            Serial.print(Calculate_Intensity,DEC);        //just a figure of merit on relative
            Serial.print(" ");
            Serial.print(marker,DEC);
            Serial.print(" ");
            Serial.println();
            } 
           //------------------------------------------------------------------------------
           if (L_struck > 0)
            {
          
           //----------------------------------------------------------------
           //distance estimation - other lightning data - energy calculation
           //----------------------------------------------------------------
           //estimated DISTANCE:
           //--------------------
            c=0;
            i= 0x07;
            dataRead= Sensor.readFromAS3935 (device, i); //read data from internal registers trough adr_I2C

            c = dataRead ;//Wire.read();    // Receive a byte as character   
            Distance = c;
            Distance  = Distance & (0b00111111);
            //--------------------------------------------
            //ENERGY content registers  LSB - MSB - MAIN
            //--------------------------------------------
            i= 0x04;
            dataRead= Sensor.readFromAS3935 (device, i); //read data from internal registers trough adr_I2C
            c=0;
            c = dataRead ;//Wire.read();    // Receive a byte as character 
            Energy_LSB = c;
            //-------------------------
            //ENERGY MSB 
            //-------------------------
              i= 0x05;
              dataRead= Sensor.readFromAS3935 (device, i); //read data from internal registers trough adr_I2C
              c=0;
              c = dataRead ;//Wire.read();    // Receive a byte as character     
               Energy_MSB = c;
    
            //-------------------------
            //ENERGY MMSB 
            //-------------------------

            i= 0x06;
            dataRead= Sensor.readFromAS3935 (device, i); //read data from internal registers trough adr_I2C
            c=0;     
            c = dataRead ;//Wire.read();    // Receive a byte as character 
      
            Energy_MMSB = c;
            Energy_MMSB  = Energy_MMSB & (0b00011111);
            Calculate_Intensity= Sensor.calculateIntensity ();//just a figure of merit on relative
                                                              //intensity of events for comparison. 
            
            //-------------------------------------------------------------------------
            //select type of readout FULL NUMERICAL DATA or GRAPH DATA
            //-------------------------------------------------------------------------
            if (GRAPH_mode == 1)
            {
            //------------------------------------------------
            //LEGIBILE FORM OF EVENT READINGS
            //------------------------------------------------
            Serial.print(MAX_scale);
            Serial.print(" ");
            
            Serial.print(min_scale);
            Serial.print(" ");
            //------------------------------------------------
            Serial.print(NOISE,DEC);
            Serial.print(" ");
            Serial.print(Disturber,DEC);
            Serial.print(" ");
            Serial.print(Distance,DEC);
            Serial.print(" ");
            //Serial.println(Energy_LSB,DEC);
            //Serial.print(" ");
            Energy_MSB = Energy_MSB/20;   //JUST A SCALING FACTOR TO PLOT !
            Serial.print(Energy_MSB,DEC);
            Serial.print(" ");
            Serial.print(Energy_MMSB,DEC);
            Serial.print(" ");
            Calculate_Intensity = Calculate_Intensity/100;   //JUST A SCALING FACTOR TO PLOT !
            Serial.print(Calculate_Intensity,DEC); //just a figure of merit on relative
            Serial.print(" ");                     //intensity of events for comparison. 
            Serial.print(marker,DEC);
            Serial.print(" ");
            Serial.println();
            } 
            if (LITERAL_mode == 1)
            {
            //------------------------------------------------
            //LEGIBILE FORM OF EVENT READINGS
            //------------------------------------------------
            Serial.println();
            Serial.println("*****************************************************");
            Serial.println();
            Serial.print("Noise LEVEL detected: ");
            Serial.println(NOISE,DEC);
            Serial.print("Disturber detected: ");
            Serial.println(Disturber,DEC);
            Serial.print("Lightining detected: ");
            Serial.println( L_struck,DEC);
            Serial.println();
            Serial.print("Estimated hit distance: ");
            Serial.println(Distance,DEC);
            Serial.print("LSB single event energy: ");
            Serial.println(Energy_LSB,DEC);
            Serial.print("MSB single event energy: ");
            Serial.println(Energy_MSB,DEC);
            Serial.print("MMSB single event energy: ");
            Serial.println(Energy_MMSB,DEC);
            Serial.print("Total event energy: ");//just a figure of merit on relative
                                                 //intensity of events for comparison. 
            Serial.println(Calculate_Intensity,DEC);
            } 
        //---------------------------------------------------------------------------------
     
       
      }         
     delay(1000);  // <------------- SCANNING INTERVAL .... MODIFY AS NEEDED !!!
             
    }

   
    //----------------END void LOOP ----------------------------------------------------------------------------
  } 
 }
