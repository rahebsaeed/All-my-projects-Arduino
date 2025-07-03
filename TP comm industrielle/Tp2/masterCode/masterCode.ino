/*
 * Modbus RTU Master - Robust Automated Sender
 * Works with ModbusMaster v2.0.1
 */

#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// Modbus Configuration
#define SLAVE_ID 1
#define BAUD_RATE 9600
#define SEND_INTERVAL 2000  // 2 seconds

// RS485 Pins
#define RS485_RX_PIN 4
#define RS485_TX_PIN 5
#define RS485_CTRL_PIN 6

SoftwareSerial modbusSerial(RS485_RX_PIN, RS485_TX_PIN);
ModbusMaster node;

uint16_t sendData = 0;
unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("=== Robust Modbus Master ===");

  // RS485 Control
  pinMode(RS485_CTRL_PIN, OUTPUT);
  digitalWrite(RS485_CTRL_PIN, LOW);  // Receive mode

  // Modbus initialization
  modbusSerial.begin(BAUD_RATE);
  node.begin(SLAVE_ID, modbusSerial);
  
  // Callbacks for proper half-duplex switching
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  
  Serial.println("Master ready. Starting communication...");
}

void loop() {
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = millis();
    sendData++;
    
    Serial.print("\nAttempting to send: ");
    Serial.println(sendData);
    
    uint8_t result = node.writeSingleRegister(0, sendData);
    
    if (result == node.ku8MBSuccess) {
      Serial.print("Success! Sent value: ");
      Serial.println(sendData);
      
      // Optional: Read back to verify
      delay(50);  // Increased delay before reading back
      uint8_t readResult = node.readHoldingRegisters(0, 1);
      
      if (readResult == node.ku8MBSuccess) {
        Serial.print("Verified value: ");
        Serial.println(node.getResponseBuffer(0));
      } else {
        Serial.print("Verification failed: ");
        printModbusError(readResult);
      }
    } else {
      Serial.print("Send failed: ");
      printModbusError(result);
    }
  }
}

void printModbusError(uint8_t error) {
  switch(error) {
    case node.ku8MBSuccess:           Serial.println("Success"); break;
    case node.ku8MBInvalidSlaveID:    Serial.println("Invalid slave ID"); break;
    case node.ku8MBInvalidFunction:   Serial.println("Invalid function"); break;
    case node.ku8MBResponseTimedOut:  Serial.println("Response timeout"); break;
    case node.ku8MBInvalidCRC:        Serial.println("Invalid CRC"); break;
    default: Serial.print("Unknown error: "); Serial.println(error); break;
  }
}

void preTransmission() {
  digitalWrite(RS485_CTRL_PIN, HIGH);  // Enable transmit
  delayMicroseconds(100);  // Allow line to stabilize
}

void postTransmission() {
  delayMicroseconds(100);  // Ensure transmission completes
  digitalWrite(RS485_CTRL_PIN, LOW);  // Back to receive mode
}