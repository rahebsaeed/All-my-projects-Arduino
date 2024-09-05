// This example looks for a file named "logo.bmp"
//  On the SD card, and renders it to the screen
//All copyrights for Raheb Aref Mahyoub Saeed

#include <Esplora.h>
#include <SD.h>
#include <SPI.h>
#include <TFT.h>            // Arduino TFT library

#define SD_CS    8  // Chip select line for SD card in Esplora

PImage logo;

void setup() {
  // initialize the screen
  EsploraTFT.begin();
  // initialize the SD card
  SD.begin(SD_CS);
  // set the background the black
  EsploraTFT.background(0, 0, 0);

  // load the image into the named instance of PImage
  logo = EsploraTFT.loadImage("arduino.bmp");

  // if it is a valid image file, turn the Esplora's LED green
   if (logo.isValid()) { 
       Esplora.writeGreen(255);
  }
  else{
  // if it is not valid, turn the LED red
    Esplora.writeRed(255);
  }

  // draw the image on the screen starting at the top left corner
  EsploraTFT.image(logo, 0, 0);
}

void loop() {

}