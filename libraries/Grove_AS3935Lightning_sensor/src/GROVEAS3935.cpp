
/*
Grove-AS3935Lightning sensor *.CPP file for Library.
This file furnishes the essential functions to work
with Arduino and to understand how to work with this
sensor.
The functions can be used to make proper software and
work with the Grove-AS3935Lightning sensor.
.
Author: Davide Cogliati
Revision: 1.0 12-15-2022
*/

#include "GROVEAS3935.h"  //include the header file for Grove-AS3935 Lightning sensor
//#include <Wire.h>       //<---THE Wire Library is Arduino and is called in the *.h file

 
void GROVEAS3935::initializeDevice(int device)
{
   
  _device = device;
}           
          

void GROVEAS3935::gainAS3935 (unsigned int gain)
{       //------------------------------------------------------------------
        //read content of internal register 0x00
        //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
      
        
        Wire.beginTransmission(_device);
        Wire.write(0x00);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device, ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
            }
        //------------------------------------------------------------------
        //REG 0X00 write bits 1,2,3,4,5: values range from 0 to 31
        //------------------------------------------------------------------
        gain = gain & (0b00011111); ;//masking for valid values only
        gain = gain *2;             ;//shift to left 1 bit     
        c = c & (0b11000001); 
        c = c | gain;               ;//add gain to reg 0x00
        Wire.beginTransmission(_device);
        Wire.write(0x00); 
        Wire.write(c);
        Wire.endTransmission(true);  
}

void GROVEAS3935::setDefault(void)
{
        //------------------------------------------------------------------
        //REG 0X3C write 0x96
        //------------------------------------------------------------------
        
        Wire.beginTransmission(_device);
        Wire.write(0x3C); 
        Wire.write(SET_DEFAULT);
        Wire.endTransmission(true);  
            
}
void GROVEAS3935::calibrateAuto(void)
{       
        //------------------------------------------------------------------
        //REG 0X3D write 0x96
        //------------------------------------------------------------------
        Wire.beginTransmission(_device);
        Wire.write(0x3D); 
        Wire.write(CALIBRATE_AUTO);
        Wire.endTransmission(true);  
 }   



   
void GROVEAS3935::writeToAS3935 (int dataWrite, int device, int adr_I2C)
{           //-------------------------------------------------
            //write to internal register in random way 1 BYTE
            //-------------------------------------------------
            unsigned int c;
            int ndata;
            ndata=1;
            
            Wire.beginTransmission(_device);
            Wire.write(adr_I2C);
            Wire.write(dataWrite);
            Wire.endTransmission(true);  
}
int GROVEAS3935::readFromAS3935 (int adr_Device, int adr_I2C)
{
   //------------------------------------------------------------------
   //read content of internal register in random way 1 BYTE
   //------------------------------------------------------------------
        unsigned int dataRead;
        
        int ndata;
        ndata=1;
     
        
        Wire.beginTransmission(_device);
        Wire.write(adr_I2C);            //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device, ndata);//request bytes from register 0x00
        Wire.endTransmission();         //release the line
        delay(100);
       while(Wire.available())
            {
              dataRead = Wire.read();   //Receive register content
             
            }
          return dataRead;    
        
}




unsigned int GROVEAS3935::calculateIntensity (void)
{   //----------------------------------------------------------------------------------------------
    //this function adds in weighted manner the LSB, MSB, MMSB content
    //The device constructor issues that these numbers have no physical significance
    //in the sense they are not absolute with a measurement unit such as V/m as electric field
    //They offer a way to compare data collected on the field in an environment
    //----------------------------------------------------------------------------------------------
    //------------------------------------------------------------------
    //REG 0X04, 0x05  and 0x06 bits 0,1,2,3,4
    //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x04);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();          // Receive register content
              strikeLSB = c;
            }
        delay(200);
        Wire.beginTransmission(_device);
        Wire.write(0x05);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();          // Receive register content
              strikeMSB = c;
            }
        delay(200);
        Wire.beginTransmission(_device);
        Wire.write(0x06);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c = c & (0b00011111);
              strikeMMSB = c;
            }
         //------------------------------------------------------------------------
         //division /10 is made in order to work with numbers not to big
         //this function is for qualitative comparison of intensity
         //and can be modified as desired to obtain a good figure of merit
         //------------------------------------------------------------------------
         strikeEval = 255*((255*strikeMMSB)/10) + (255*strikeMSB)/10 + strikeLSB/10;
         return strikeEval;   
   
}


unsigned int GROVEAS3935::frontDistance(void)
{
   //------------------------------------------------------------------
   //REG 0X07  is read bits: 0,1,2,3,4,5
   //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x07);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c = c & (0b00111111);   // AND to cancel information not necessary
               
              return c;
            }

}
unsigned int GROVEAS3935::getNoise(void)
{
   //------------------------------------------------------------------
   //REG 0X03  is read bits: 0,1,2,3
   //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
       
        Wire.beginTransmission(_device);
        Wire.write(0x03);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c = c & (0b00000001);   // AND to cancel information not necessary
             // c= unsigned int(c);   // cast to unsigned integer
              return c;               // returns 1 if noise exceeds the threshold, 0 if less than treshold
            }
}   

void GROVEAS3935::tuneCapacitor(unsigned int tuningCap)
{

  //-------------------------------------------------------------
  //fix the sensor tuning cap. , accepted values: 0 up to 15
  //from 0 to 120pF in steps of 8pF
  //-------------------------------------------------------------
  //------------------------------------------------------------------
  //REG 0X08  modify bit 3,2,1,0
  //------------------------------------------------------------------
        unsigned int c;  
        int ndata;
        ndata=1;
       
        Wire.beginTransmission(_device);
        Wire.write(0x08);                 //address to be read
        Wire.endTransmission(false);      //do not release the line!
        Wire.requestFrom(_device, ndata); // request bytes from register 0x00
        Wire.endTransmission();           //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11110000);    // AND to cancel information to be modified
              tuningCap = tuningCap & (0b00001111);  //prevents outrange values, only 0 to 15
              c = c | tuningCap;      // OR to add information
      
            //tune capacitor adding -----------
            Wire.beginTransmission(_device);
            Wire.write(0x08);
            Wire.write(c);
            Wire.endTransmission(true); 
            }


}


   
void GROVEAS3935::noiseFloorLevel(unsigned int noiseLevel)
{

  //-------------------------------------------------------------
  //fix the sensor noise floor level, accepted values: 0 up to 7
  //-------------------------------------------------------------
  //------------------------------------------------------------------
  //REG 0X01  modify bit 6,5,4
  //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
      
        Wire.beginTransmission(_device);
        Wire.write(0x01);                 //address to be read
        Wire.endTransmission(false);      //do not release the line!
        Wire.requestFrom(_device, ndata); // request bytes from register 0x00
        Wire.endTransmission();           //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();                 // Receive register content
              c= c & (0b10001111);             // AND to cancel information to be modified
              noiseLevel = noiseLevel & (0b00000111);  //prevents outrange values, only 0 to 7
              noiseLevel = noiseLevel * 16;   //shift left
              c= c | noiseLevel;              // OR to add information
      
            //noise level floor -------------
            Wire.beginTransmission(_device);
            Wire.write(0x01);
            Wire.write(c);
            Wire.endTransmission(true); 
            }
}
void GROVEAS3935::watchDogLevel(unsigned int watchDog)
{

  //-------------------------------------------------------------
  //fix the sensor watchdog level, accepted values: 0 up to 15
  //-------------------------------------------------------------
  //------------------------------------------------------------------
  //REG 0X01  modify bit 6,5,4
  //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        
        Wire.beginTransmission(_device);
        Wire.write(0x01);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();                    // Receive register content
              c= c & (0b11110000);                // AND to cancel information to be modified
              watchDog = watchDog & (0b00001111); //prevents outrange values, only 0 to 7
              c= c | watchDog;                    // OR to add information
      
            //watchdog level ------------------
            Wire.beginTransmission(_device);
            Wire.write(0x01);
            Wire.write(c);
            Wire.endTransmission(true); 
            }
}
void GROVEAS3935::lightningNum(unsigned int numberLightning)
{

  //---------------------------------------------------------------------------------
  //fix the sensor number of lightning to trigger alarm, accepted values: 0 up to 3
  //---------------------------------------------------------------------------------
  //------------------------------------------------------------------
  //REG 0X02  modify bit 5,4
  //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x02);                 //address to be read
        Wire.endTransmission(false);      //do not release the line!
        Wire.requestFrom(_device, ndata); // request bytes from register 0x00
        Wire.endTransmission();           //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11111100);    // AND to cancel information to be modified
              numberLightning = numberLightning & (0b00000011);  //prevents outrange values, only 0 to 3
              numberLightning = numberLightning * 16;  //shift left
              c= c | numberLightning;                  // OR to add information
      
            //number of lightning to trigger ------- 
            Wire.beginTransmission(_device);
            Wire.write(0x02);
            Wire.write(c);
            Wire.endTransmission(true); 
            }
}

void GROVEAS3935::rejectionSpike(unsigned int spikeReject)
{

  //---------------------------------------------------------------------------------
  //fix the sensor level for spike rejection, accepted values: 0 up to 15
  //---------------------------------------------------------------------------------
  //------------------------------------------------------------------
  //REG 0X02  modify bit 3,2,1,0
  //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x02);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11110000);    // AND to cancel information to be modified
              spikeReject = spikeReject & (0b00001111);  //prevents outrange values, only 0 to 15
              c= c | spikeReject;                        // OR to add information
      
            //spike rejection level ----------
            Wire.beginTransmission(_device);
            Wire.write(0x02);
            Wire.write(c);
            Wire.endTransmission(true); 
            }
}


void GROVEAS3935::maskON(void)
{
        //------------------------------------------------------------------
        //REG 0X03  modify bit 5
        //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x03);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11011111);    // AND to cancel information to be modified
              c= c | MASK_ON;         // OR to add information
      
            //SET_OUTDOOR MODE -------------
            Wire.beginTransmission(_device);
            Wire.write(0x03);
            Wire.write(c);
            Wire.endTransmission(true);
            }
              
}    
      
     
void GROVEAS3935::maskOFF(void)
{
     
       //------------------------------------------------------------------
        //REG 0X03  modify bit 5
        //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x03);                 //address to be read
        Wire.endTransmission(false);      //do not release the line!
        Wire.requestFrom(_device,ndata);  // request bytes from register 0x00
        Wire.endTransmission();           //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11011111);    // AND to cancel information to be modified
              c= c | MASK_OFF;        // OR to add information
      
            //SET_OUTDOOR MODE -------------
            Wire.beginTransmission(_device);
            Wire.write(0x03);
            Wire.write(c);
            Wire.endTransmission(true);  
            }    
      
}
void GROVEAS3935::setOutdoor(void)
{
        //------------------------------------------------------------------
        //REG 0X00  modify bits 1,2,3,4,5
        //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x00);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11000001);    // AND to cancel information to be modified
              c= c | SET_INDOOR;      // OR to add information
      
        //SET_OUTDOOR MODE -------------
        Wire.beginTransmission(_device);
        Wire.write(0x00);
        Wire.write(c);
        Wire.endTransmission(true);  
        } 
           
}

void GROVEAS3935::setIndoor(void)
{
    //------------------------------------------------------------------
    //REG 0X00  modify bits 1,2,3,4,5
    //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x00);                 //address to be read
        Wire.endTransmission(false);      //do not release the line!
        Wire.requestFrom(_device,ndata);  // request bytes from register 0x00
        Wire.endTransmission();           //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11000001);    // AND to cancel information to be modified
              c= c | SET_OUTDOOR;     // OR to add information
      
        //SET_OUTDOOR MODE ------------
        Wire.beginTransmission(_device);
        Wire.write(0x00);
        Wire.write(c);
        Wire.endTransmission(true);  
        }    
     
}
void GROVEAS3935::powerDwn(void)
{
     //------------------------------------------------------------------
     //REG 0X00  modify bit 0
     //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x00);                 //address to be read
        Wire.endTransmission(false);      //do not release the line!
        Wire.requestFrom(_device,ndata);  // request bytes from register 0x00
        Wire.endTransmission();           //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11000001);    // AND to cancel information to be modified
              c= c | POWER_DOWN;      // OR to add information
      
        //SET power down MODE -----------
        Wire.beginTransmission(_device);
        Wire.write(0x00);
        Wire.write(c);
        Wire.endTransmission(true);  
        }    
      
}
void GROVEAS3935::powerUpn(void)
{
      //------------------------------------------------------------------
      //REG 0X00  modify bit 0
      //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1;
        
        Wire.beginTransmission(_device);
        Wire.write(0x00); //address to be read
        Wire.endTransmission(false);   //do not release the line!
        Wire.requestFrom(_device,ndata);// request bytes from register 0x00
        Wire.endTransmission();        //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b11000001);    // AND to cancel information to be modified
              c= c | POWER_UP;        // OR to add information
      
        //SET power up MODE -------------
        Wire.beginTransmission(_device);
        Wire.write(0x00);
        Wire.write(c);
        Wire.endTransmission(true);  
        }    
      
}
void GROVEAS3935::clearStatOn(void)
{
      //to clear build up statistics toggle the bit REG0x02[6] (high-low-high)
      //------------------------------------------------------------------
      //REG 0X02  modify bit 0
      //------------------------------------------------------------------
        unsigned int c;
        int ndata;
        ndata=1; 
        
        Wire.beginTransmission(_device);
        Wire.write(0x02);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,ndata);//request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();         // Receive register content
              c = c & (0b10111111);    // AND to cancel information to be modified
              c = c | CLEAR_STAT_OFF;  // OR to add information
      
        //SET to 1
        Wire.beginTransmission(_device);
        Wire.write(0x02);
        Wire.write(c);
        Wire.endTransmission(true);  
        delay(200);
        //SET to 0
        c = c &  CLEAR_STAT_ON;
        Wire.beginTransmission(_device);
        Wire.write(0x02);
        Wire.write(c);
        Wire.endTransmission(true);  
        delay(200);
        //SET to 1
        c = c | CLEAR_STAT_OFF;
        Wire.beginTransmission(_device);
        Wire.write(0x02);
        Wire.write(c);
        Wire.endTransmission(true);  
        delay(200);
        
        }  
          
}
void GROVEAS3935::clearStatOff(void)
{
      //------------------------------------------------------------------
      //REG 0X02  modify bit 0
      //------------------------------------------------------------------
        unsigned int c;
        
        Wire.beginTransmission(_device);
        Wire.write(0x02);              //address to be read
        Wire.endTransmission(false);   //do not release the line!
        Wire.requestFrom(_device,1);   // request bytes from register 0x00
        Wire.endTransmission();        //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();         // Receive register content
              c = c & (0b10111111);    // AND to cancel information to be modified
              c = c | CLEAR_STAT_OFF;  // OR to add information
      
        //--------------------------------
        Wire.beginTransmission(_device);
        Wire.write(0x02);
        Wire.write(c);
        Wire.endTransmission(true);  
        delay(200);
        }
}

void GROVEAS3935::setIrqfreq(void)
{
      //------------------------------------------------------------------
      //REG 0X08  modify bit 7,6,5
      //------------------------------------------------------------------
        unsigned int c;
       
       
        Wire.beginTransmission(_device);
        Wire.write(0x08);               //address to be read
        Wire.endTransmission(false);    //do not release the line!
        Wire.requestFrom(_device,1);    // request bytes from register 0x00
        Wire.endTransmission();         //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();          // Receive register content
              c = c & (0b00011111);     // AND to cancel information to be modified
              c = c | IRQ_LCO;          // OR to add information
      
        //-----------------------------
        Wire.beginTransmission(_device);
        Wire.write(0x08);
        Wire.write(c);
        Wire.endTransmission(true);  
        delay(200);
         }
         
}
void GROVEAS3935::setIrqEvent(void)
{
      //------------------------------------------------------------------
      //REG 0X08  modify bit 7,6,5
      //------------------------------------------------------------------
        unsigned int c;
        
           
        Wire.beginTransmission(_device);
        Wire.write(0x08);              //address to be read
        Wire.endTransmission(false);   //do not release the line!
        Wire.requestFrom(_device,1);   //request bytes from register 0x00
        Wire.endTransmission();        //release the line
        c=0;
        while(Wire.available())
            {
              c = Wire.read();        // Receive register content
              c= c & (0b00011111);    // AND to cancel information to be modified
              c= c | IRQ_LIGHTNING;   // OR to add information
      
        //-----------------------------
        Wire.beginTransmission(_device);
        Wire.write(0x08);
        Wire.write(c);
        Wire.endTransmission(true);  
        delay(200);
        }
}
        
     
   
 
