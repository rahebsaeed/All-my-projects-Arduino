#include "RAK811.h"
#include "SoftwareSerial.h"

#define WORK_MODE LoRaP2P
#define TXpin 11
#define RXpin 10
#define DebugSerial Serial  // Use Serial as the DebugSerial

SoftwareSerial ATSerial(RXpin, TXpin);  // Declare a virtual serial port

RAK811 RAKLoRa(ATSerial, DebugSerial);

char buffer[] = "Welcome LoRa"; // ASCII text
char hexBuffer[100]; // To hold the hex representation

String asciiToHex(String ascii) {
    String hex = "";
    for (int i = 0; i < ascii.length(); i++) {
        char c = ascii[i];
        // Convert each character to a 2-digit hexadecimal string
        if (c < 16) {
            hex += "0";  // Add leading zero for single-digit hex
        }
        hex += String(c, HEX); // Convert to HEX and append
    }
    hex.toUpperCase(); // Optional: Convert to uppercase
    return hex;
}

void setup() {
  DebugSerial.begin(115200);  // Use DebugSerial for additional debugging
  while(Serial.available()) {
    Serial.read();
  }

  ATSerial.begin(115200);  // Baudrate for communication with RAK811
  while(ATSerial.available()) {
    ATSerial.read();
  }

  RAKLoRa.rk_getVersion();
  DebugSerial.println(RAKLoRa.rk_recvData());

  DebugSerial.println(F("Start init LoRaP2P parameters..."));

  if (!RAKLoRa.rk_initP2P("869525000", 7, 1, 1, 8, 20)) {
    DebugSerial.println(F("Init error, please reset module."));
    while (1);
  } else {
    DebugSerial.println(F("Init OK"));
  }

  // Convert ASCII buffer to Hex and store it in the global hexBuffer
  String hexString = asciiToHex(String(buffer));
  hexString.toCharArray(hexBuffer, hexString.length() + 1); // Convert String to char array
}

void loop() {
  DebugSerial.println(F("Start send data..."));
  
  if (RAKLoRa.rk_sendP2PData(hexBuffer)) {
    DebugSerial.print("Data Sent: ");
    DebugSerial.println(hexBuffer);
    
  } else {
    DebugSerial.println("Send failed.");
  }
  delay(30000);  // Send every 30 seconds for testing

}

