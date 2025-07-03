// =======================================================================
// ========== CODE FOR ARDUINO NANO (The Modem & Monitor) ==========
// =======================================================================
#include <SoftwareSerial.h>
#include "RAK811.h"

// --- Serial port for RAK811 ---
#define RAK811_RX_PIN 2
#define RAK811_TX_PIN 3
SoftwareSerial rakSerial(RAK811_RX_PIN, RAK811_TX_PIN); // RX, TX

// --- Serial port for ESP32-CAM ---
#define ESP32_RX_PIN 10
#define ESP32_TX_PIN 11
SoftwareSerial espSerial(ESP32_RX_PIN, ESP32_TX_PIN); // RX, TX

// --- RAK811 Library ---
// We pass 'rakSerial' for LoRa comms and 'Serial' for library debug output.
RAK811 RAKLoRa(rakSerial, Serial); 


void setup() {
  // Start serial for monitoring on your computer
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to connect
  Serial.println("Arduino Nano LoRa Gateway Initializing...");

  // Start serial to ESP32-CAM (MUST match ESP32 speed)
  espSerial.begin(115200);

  // Start serial to RAK811 (MUST match RAK811 speed)
  rakSerial.begin(115200);
  delay(1000);

  RAKLoRa.sendRawCommand("at+join");

  delay(2000);

  
  
  Serial.println("Successfully joined LoRaWAN network!");
 
  
  // --- Send initial command to ESP32-CAM ---
  Serial.println("Sending 'cam' command to ESP32-CAM...");
  espSerial.println("cam");
}

void loop() {

  
  // Check for data coming from the ESP32-CAM
  if (espSerial.available()) {
    String messageFromEsp = espSerial.readStringUntil('\n');
    messageFromEsp.trim();

    if (messageFromEsp.length() > 0) {
      if (messageFromEsp == "DONE") {
        Serial.println("-------------------------------------");
        Serial.println("Received 'DONE' from ESP32. Image transfer complete.");
        Serial.println("System will now wait. Reset to start again.");
      } 
      else if (messageFromEsp.startsWith("ERROR:")) {
          Serial.print("!!! Received an error from ESP32: ");
          Serial.println(messageFromEsp);
      } 
      else {
        // This is a hex chunk.
        Serial.println("-------------------------------------");
        Serial.println("Received hex chunk from ESP32:");
        Serial.println(messageFromEsp);

        String command = "at+send=lora:2:" + messageFromEsp;
        Serial.println("Sending command to RAK811...");

        if (RAKLoRa.sendRawCommand(command)) {
          Serial.println("RAK811 send successful.");
          Serial.println("Sending 'OK' back to ESP32 to request next chunk.");
          espSerial.println("OK"); 
        } else {
          Serial.println("!!! RAK811 send FAILED! Not sending 'OK'. ESP32 will wait.");
        }
      }
    }
  }
}