/*
 * Modbus RTU Slave - Robust Receiver
 * Enhanced with better timing and error handling
 */

#include <ModbusRTUSlave.h>
#include <SoftwareSerial.h>

// Modbus Configuration
#define SLAVE_ID 1
#define BAUD_RATE 9600
#define NUM_REGISTERS 10

// RS485 Pins
#define RS485_RX_PIN 4
#define RS485_TX_PIN 5
#define RS485_CTRL_PIN 6

SoftwareSerial modbusSerial(RS485_RX_PIN, RS485_TX_PIN);
ModbusRTUSlave modbus(modbusSerial, RS485_CTRL_PIN);

uint16_t holdingRegisters[NUM_REGISTERS];
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 1000;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("=== Robust Modbus Slave ===");

  // Initialize registers
  for (uint8_t i = 0; i < NUM_REGISTERS; i++) {
    holdingRegisters[i] = 0;
  }

  // Modbus initialization
  modbusSerial.begin(BAUD_RATE);
  modbus.configureHoldingRegisters(holdingRegisters, NUM_REGISTERS);
  modbus.begin(SLAVE_ID, BAUD_RATE);
  
  Serial.println("Slave ready. Waiting for commands...");
}

void loop() {
  // Process Modbus requests
  modbus.poll();

  // Periodic status update
  if (millis() - lastPrintTime >= PRINT_INTERVAL) {
    lastPrintTime = millis();
    
    Serial.print("\nCurrent Register Values: ");
    for (uint8_t i = 0; i < NUM_REGISTERS; i++) {
      Serial.print(holdingRegisters[i]);
      if (i < NUM_REGISTERS - 1) Serial.print(", ");
    }
    Serial.println();
  }
}