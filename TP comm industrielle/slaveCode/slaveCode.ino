#include <WiFi.h>
#include <ModbusIP_ESP8266.h>

// Register Definitions
const int COIL_ADDR = 1;          // 0x00001
const int DISCRETE_INPUT_ADDR = 2; // 1x00002
const int HOLDING_REG_ADDR = 10;   // 4x00010
const int INPUT_REG_ADDR = 100;    // 3x00100

ModbusIP mb;

// Callback function prototypes
uint16_t cbCoil(TRegister* reg, uint16_t val);
uint16_t cbHoldingReg(TRegister* reg, uint16_t val);
Modbus::ResultCode cbRequest(Modbus::FunctionCode fc, Modbus::RequestData request);

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin("derjhgcv", "12341234");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());

  // Initialize Modbus Registers
  mb.server();
  mb.addCoil(COIL_ADDR);            
  mb.addIsts(DISCRETE_INPUT_ADDR);   
  mb.addHreg(HOLDING_REG_ADDR);      
  mb.addIreg(INPUT_REG_ADDR);        

  // Set Initial Values
  mb.Coil(COIL_ADDR, false);
  mb.Ists(DISCRETE_INPUT_ADDR, false);
  mb.Hreg(HOLDING_REG_ADDR, 0);
  mb.Ireg(INPUT_REG_ADDR, 0);

  // Setup callbacks
  mb.onSetCoil(COIL_ADDR, cbCoil);
  mb.onSetHreg(HOLDING_REG_ADDR, cbHoldingReg);
  mb.onRequest(cbRequest);

  Serial.println("\nModbus TCP Slave Ready");
  printHelp();
}

// Correct request callback for v4.1.0
Modbus::ResultCode cbRequest(Modbus::FunctionCode fc, Modbus::RequestData request) {
  // Simplified request handling - shows function code only
  Serial.printf("[Master] Function Code: %d\n", (uint8_t)fc);
  return Modbus::EX_SUCCESS;
}

// Callback for Coil writes
uint16_t cbCoil(TRegister* reg, uint16_t val) {
  Serial.printf("[Master] Write Coil %d=%s\n", reg->address.address, val ? "ON" : "OFF");
  return val;
}

// Callback for Holding Register writes
uint16_t cbHoldingReg(TRegister* reg, uint16_t val) {
  Serial.printf("[Master] Write HReg %d=%d\n", reg->address.address, val);
  return val;
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
      Serial.printf("Coil %d=%s\n", addr, mb.Coil(addr) ? "ON" : "OFF");
    }
    else if (cmd.startsWith("read hreg ")) {
      int addr = cmd.substring(10).toInt();
      Serial.printf("HReg %d=%d\n", addr, mb.Hreg(addr));
    }
    else if (cmd.startsWith("write coil ")) {
      int spacePos = cmd.indexOf(' ', 11);
      int addr = cmd.substring(11, spacePos).toInt();
      bool val = cmd.substring(spacePos+1).toInt() > 0;
      mb.Coil(addr, val);
      Serial.printf("Set Coil %d=%s\n", addr, val ? "ON" : "OFF");
    }
    else if (cmd.startsWith("write hreg ")) {
      int spacePos = cmd.indexOf(' ', 11);
      int addr = cmd.substring(11, spacePos).toInt();
      uint16_t val = cmd.substring(spacePos+1).toInt();
      mb.Hreg(addr, val);
      Serial.printf("Set HReg %d=%d\n", addr, val);
    }
  }
}

void printHelp() {
  Serial.println("\nAvailable Commands:");
  Serial.println("read coil [addr]  - Read coil value");
  Serial.println("read hreg [addr]  - Read holding register");
  Serial.println("write coil [addr] [0/1] - Set coil value");
  Serial.println("write hreg [addr] [value] - Set holding register");
  Serial.println("help - Show this menu");
}