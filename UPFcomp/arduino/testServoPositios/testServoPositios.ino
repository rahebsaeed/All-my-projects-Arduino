#include <Servo.h>

// Define servo objects
Servo arm1;
Servo arm2;
Servo gripper;
Servo base;

// Current positions
int arm1Pos = 180;
int arm2Pos = 180;
int gripperPos = 0;
int basePos = 0;

// Target positions
int arm1Target = 180;
int arm2Target = 180;
int gripperTarget = 0;
int baseTarget = 0;

// Pin definitions
const int arm1Pin = D4;
const int arm2Pin = D6;
const int gripperPin = D7;
const int basePin = D3;

// Movement control
const int moveDelay = 20; // ms between steps
const int stepSize = 1;   // degrees per step
unsigned long previousMillis = 0;

void setup() {
  // Attach servos to pins
  arm1.attach(arm1Pin);
  arm2.attach(arm2Pin);
  gripper.attach(gripperPin);
  base.attach(basePin);
  
  // Set initial positions
  arm1.write(arm1Pos);
  arm2.write(arm2Pos);
  gripper.write(gripperPos);
  base.write(basePos);
  
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection
  
  Serial.println("Robot Arm Control System");
  Serial.println("Initial positions: Base=0°, Arm1=90°, Arm2=90°, Gripper=0°");
  Serial.println("Enter commands in format: B=angle, A1=angle, A2=angle, G=angle");
  Serial.println("Example: A1=45 (will move only Arm1, others maintain position)");
  Serial.println("Full status is always shown after movement");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Smooth movement handler (non-blocking)
  if (currentMillis - previousMillis >= moveDelay) {
    previousMillis = currentMillis;
    
    // Move Base
    if (basePos != baseTarget) {
      basePos += (baseTarget > basePos) ? stepSize : -stepSize;
      base.write(basePos);
    }
    
    // Move Arm1
    if (arm1Pos != arm1Target) {
      arm1Pos += (arm1Target > arm1Pos) ? stepSize : -stepSize;
      arm1.write(arm1Pos);
    }
    
    // Move Arm2
    if (arm2Pos != arm2Target) {
      arm2Pos += (arm2Target > arm2Pos) ? stepSize : -stepSize;
      arm2.write(arm2Pos);
    }
    
    // Move Gripper
    if (gripperPos != gripperTarget) {
      gripperPos += (gripperTarget > gripperPos) ? stepSize : -stepSize;
      gripper.write(gripperPos);
    }
  }
  
  // Serial command handling
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    processCommand(input);
    printStatus(); // Always show status after command
  }
}

void processCommand(String command) {
  // First update all targets to current positions
  // This ensures unchanged servos stay in place
  baseTarget = basePos;
  arm1Target = arm1Pos;
  arm2Target = arm2Pos;
  gripperTarget = gripperPos;
  
  // Now process the command to update specific targets
  int commaIndex1 = command.indexOf(',');
  int commaIndex2 = command.lastIndexOf(',');
  
  String part1, part2, part3, part4;
  
  if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex1 != commaIndex2) {
    // Multiple commands
    int commaCount = 0;
    for (int i = 0; i < command.length(); i++) {
      if (command.charAt(i) == ',') commaCount++;
    }
    
    if (commaCount >= 3) {
      // Four commands
      part1 = command.substring(0, commaIndex1);
      int commaIndex3 = command.indexOf(',', commaIndex1 + 1);
      part2 = command.substring(commaIndex1 + 1, commaIndex3);
      part3 = command.substring(commaIndex3 + 1, commaIndex2);
      part4 = command.substring(commaIndex2 + 1);
      
      processSingleCommand(part1);
      processSingleCommand(part2);
      processSingleCommand(part3);
      processSingleCommand(part4);
    } else {
      // Three commands
      part1 = command.substring(0, commaIndex1);
      part2 = command.substring(commaIndex1 + 1, commaIndex2);
      part3 = command.substring(commaIndex2 + 1);
      
      processSingleCommand(part1);
      processSingleCommand(part2);
      processSingleCommand(part3);
    }
  } else if (commaIndex1 != -1) {
    // Two commands
    part1 = command.substring(0, commaIndex1);
    part2 = command.substring(commaIndex1 + 1);
    
    processSingleCommand(part1);
    processSingleCommand(part2);
  } else {
    // Single command
    processSingleCommand(command);
  }
}

void processSingleCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  
  if (cmd.startsWith("B=")) {
    int angle = cmd.substring(2).toInt();
    if (angle >= 0 && angle <= 180) {
      baseTarget = angle;
      Serial.print("Base moving to ");
      Serial.print(baseTarget);
      Serial.println("°");
    } else {
      Serial.println("Invalid angle for Base (0-180)");
    }
  }
  else if (cmd.startsWith("A1=")) {
    int angle = cmd.substring(3).toInt();
    if (angle >= 0 && angle <= 180) {
      arm1Target = angle;
      Serial.print("Arm1 moving to ");
      Serial.print(arm1Target);
      Serial.println("°");
    } else {
      Serial.println("Invalid angle for Arm1 (0-180)");
    }
  } 
  else if (cmd.startsWith("A2=")) {
    int angle = cmd.substring(3).toInt();
    if (angle >= 0 && angle <= 180) {
      arm2Target = angle;
      Serial.print("Arm2 moving to ");
      Serial.print(arm2Target);
      Serial.println("°");
    } else {
      Serial.println("Invalid angle for Arm2 (0-180)");
    }
  } 
  else if (cmd.startsWith("G=")) {
    int angle = cmd.substring(2).toInt();
    if (angle >= 0 && angle <= 180) {
      gripperTarget = angle;
      Serial.print("Gripper moving to ");
      Serial.print(gripperTarget);
      Serial.println("°");
    } else {
      Serial.println("Invalid angle for Gripper (0-180)");
    }
  } 
  else if (cmd == "HELP" || cmd == "?") {
    printHelp();
  } 
  else if (cmd == "STATUS") {
    printStatus();
  }
  else if (cmd == "STOP") {
    // Stop all movement by setting targets to current positions
    baseTarget = basePos;
    arm1Target = arm1Pos;
    arm2Target = arm2Pos;
    gripperTarget = gripperPos;
    Serial.println("All movement stopped");
  }
  else if (cmd == "HOME") {
    // Return to home position
    baseTarget = 0;
    arm1Target = 90;
    arm2Target = 90;
    gripperTarget = 0;
    Serial.println("Returning to home position (0,90,90,0)");
  }
  else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    Serial.println("Type HELP for available commands");
  }
}

void printHelp() {
  Serial.println("\nAvailable commands:");
  Serial.println("B=angle   - Set Base target position (0-180)");
  Serial.println("A1=angle  - Set Arm1 target position (0-180)");
  Serial.println("A2=angle  - Set Arm2 target position (0-180)");
  Serial.println("G=angle   - Set Gripper target position (0-180)");
  Serial.println("HOME      - Return to home position (0,90,90,0)");
  Serial.println("STOP      - Stop all movement immediately");
  Serial.println("STATUS    - Show current positions and targets");
  Serial.println("HELP or ? - Show this help message");
  Serial.println("\nMultiple commands can be separated by commas");
  Serial.println("Example: A1=45, A2=135 (others maintain position)");
}

void printStatus() {
  Serial.println("\nCurrent Status:");
  Serial.println("Servo\tCurrent\tTarget");
  Serial.print("Base:\t");
  Serial.print(basePos);
  Serial.print("°\t");
  Serial.print(baseTarget);
  Serial.println("°");
  
  Serial.print("Arm1:\t");
  Serial.print(arm1Pos);
  Serial.print("°\t");
  Serial.print(arm1Target);
  Serial.println("°");
  
  Serial.print("Arm2:\t");
  Serial.print(arm2Pos);
  Serial.print("°\t");
  Serial.print(arm2Target);
  Serial.println("°");
  
  Serial.print("Gripper:\t");
  Serial.print(gripperPos);
  Serial.print("°\t");
  Serial.print(gripperTarget);
  Serial.println("°");
  
  // Calculate if movement is in progress
  if (basePos != baseTarget || arm1Pos != arm1Target || 
      arm2Pos != arm2Target || gripperPos != gripperTarget) {
    Serial.println("Movement in progress...");
  } else {
    Serial.println("All servos at target positions");
  }
}