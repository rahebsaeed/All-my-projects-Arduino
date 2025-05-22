#include "RAK811.h"
#include "SoftwareSerial.h"

#define WORK_MODE LoRaP2P
#define TXpin 11
#define RXpin 10
#define DebugSerial Serial  // Use Serial as the DebugSerial

SoftwareSerial ATSerial(RXpin, TXpin);  // Declare a virtual serial port

RAK811 RAKLoRa(ATSerial, DebugSerial);

void setup() {
  DebugSerial.begin(115200);  // Use DebugSerial for additional debugging
  while (Serial.available()) {
    Serial.read();
  }

  ATSerial.begin(115200);  // Baudrate for communication with RAK811
  while (ATSerial.available()) {
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
}

void loop() {
  String rawData = RAKLoRa.rk_recvP2PData();
  if (rawData != NULL && rawData.length() > 0) {
    DebugSerial.println("Raw Received Data: " + rawData);

    String hex = extractHexData(rawData);  // Extract valid hex
    if (hex.length() == 0) {
      DebugSerial.println(F("No valid hex data to decode."));
      return;
    }

    DebugSerial.println("Filtered Hex Data: " + hex);

    String receivedData = hexToStr(hex);  // Convert hex to ASCII string
    DebugSerial.println("Decoded String: " + receivedData);
    delay(2000);
  } else {
    DebugSerial.println("No data received.");
    delay(2000);
  }
}

// Extract valid hex data from the raw string
String extractHexData(String rawData) {
  String hex = "";
  for (int i = 0; i < rawData.length(); i++) {
    char c = rawData[i];
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
      hex += c;  // Append valid hex characters
    }
  }

  // Ensure even length
  if (hex.length() % 2 != 0) {
    DebugSerial.println(F("Error: Hex data length is not even. Discarding data."));
    return "";
  }

  return hex;
}

// Convert hex string to ASCII text
String hexToStr(String hex) {
  String str = "";
  for (int i = 0; i < hex.length(); i += 2) {
    String byteStr = hex.substring(i, i + 2);  // Extract two characters
    int decimalValue = strtol(byteStr.c_str(), NULL, 16);  // Convert to decimal
    str += (char)decimalValue;  // Convert decimal to char and append
  }
  return str;
}
