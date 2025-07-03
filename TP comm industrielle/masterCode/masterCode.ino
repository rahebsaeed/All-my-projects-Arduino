#include <WiFi.h>
#include <ModbusIP_ESP8266.h>

// Network credentials
const char* ssid = "derjhgcv";
const char* password = "12341234";

// Slave device IP (change to your slave ESP32's IP)
IPAddress slaveIP(192, 168, 1, 100); 

// Register addresses (must match slave)
const int COIL_ADDR = 1;          // 0x00001
const int HOLDING_REG_ADDR = 10;  // 4x00010

ModbusIP mb;  // Modbus master instance

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("Master IP: ");
  Serial.println(WiFi.localIP());

  // Set Modbus TCP timeout
  mb.setTimeout(2000);
  
  Serial.println("\nModbus TCP Master Ready");
  Serial.println("Send commands via Serial Monitor:");
  Serial.println("read coil 1");
  Serial.println("read hreg 10");
  Serial.println("write coil 1 0/1");
  Serial.println("write hreg 10 value");
}

void loop() {
  mb.task();
  handleSerialCommands();
  delay(10);
}

void handleSerialCommands() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd.equals("help")) {
      printHelp();
    }
    else if (cmd.startsWith("read coil ")) {
      int addr = cmd.substring(10).toInt();
      readCoil(addr);
    }
    else if (cmd.startsWith("read hreg ")) {
      int addr = cmd.substring(10).toInt();
      readHoldingRegister(addr);
    }
    else if (cmd.startsWith("write coil ")) {
      int spacePos = cmd.indexOf(' ', 11);
      int addr = cmd.substring(11, spacePos).toInt();
      bool val = cmd.substring(spacePos+1).toInt() > 0;
      writeCoil(addr, val);
    }
    else if (cmd.startsWith("write hreg ")) {
      int spacePos = cmd.indexOf(' ', 11);
      int addr = cmd.substring(11, spacePos).toInt();
      uint16_t val = cmd.substring(spacePos+1).toInt();
      writeHoldingRegister(addr, val);
    }
  }
}

void readCoil(int addr) {
  bool coilValue = mb.readCoil(slaveIP, addr);
  if (mb.isOk()) {
    Serial.printf("Coil %d: %s\n", addr, coilValue ? "ON" : "OFF");
  } else {
    Serial.printf("Error reading coil %d\n", addr);
  }
}

void readHoldingRegister(int addr) {
  uint16_t regValue = mb.readHreg(slaveIP, addr);
  if (mb.isOk()) {
    Serial.printf("Holding Reg %d: %d\n", addr, regValue);
  } else {
    Serial.printf("Error reading holding reg %d\n", addr);
  }
}

void writeCoil(int addr, bool value) {
  mb.writeCoil(slaveIP, addr, value);
  if (mb.isOk()) {
    Serial.printf("Coil %d set to: %s\n", addr, value ? "ON" : "OFF");
  } else {
    Serial.printf("Error writing coil %d\n", addr);
  }
}

void writeHoldingRegister(int addr, uint16_t value) {
  mb.writeHreg(slaveIP, addr, value);
  if (mb.isOk()) {
    Serial.printf("Holding Reg %d set to: %d\n", addr, value);
  } else {
    Serial.printf("Error writing holding reg %d\n", addr);
  }
}

void printHelp() {
  Serial.println("\nAvailable Commands:");
  Serial.println("read coil [addr]  - Read coil value");
 