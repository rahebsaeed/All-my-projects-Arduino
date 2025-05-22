#include <Servo.h>

// Servo objects - Arm1 uses MG90S
Servo base, arm1, arm2, gripper;

// Servo configuration - Changed to match your pin numbers
const uint8_t servoPins[4] = {A0, A1, A2, A3}; // Base, Arm1(MG90S), Arm2, Gripper
#define timePosition 500

// MG90S specific adjustments
const int MG90S_MIN_US = 500;    // MG90S minimum pulse width (µs)
const int MG90S_MAX_US = 2500;   // MG90S maximum pulse width (µs)

// Current and target positions [base, arm1, arm2, gripper]
float currentPos[4] = {0, 180, 180, 80};
float targetPos[4] = {0, 180, 180, 80};

// Predefined positions [base, arm1, arm2, gripper]
const float positions[25][4] = {
  {0, 180, 180, 80},    // HOME
  {0, 30, 90, 80},      // P1
  {0, 30, 90, 30},      // P2
  {0, 60, 30, 30},      // P3
  {0, 100, 70, 30},     // P4
  {0, 180, 180, 30},    // P5 
  {180, 180, 180, 30},  // P6
  // First position for discharge
  {180, 90, 120, 30},   // P7 
  {180, 90, 120, 80},   // P8
  {180, 140, 140, 80},  // P9
  // Second position for discharge
  {180, 120, 160, 30},  // P10
  {180, 120, 160, 80},  // P11
  {180, 140, 120, 80},  // P12
  // Final position
  {180, 180, 180, 80},  // P13
  //for get from positions 
  {180, 180, 180, 80},  // P14
  //for get from position1
  {180, 90, 120, 80},  // P15
  {180, 90, 120, 30},  // P16
  {180, 140, 140, 30},  // P17
  //for get from position2
  {180, 120, 160, 80},  // P18
  {180, 120, 160, 30},  // P19
  {180, 140, 120, 30},  // P20
  {180, 180, 180, 30},  // P21
  {0, 180, 180, 30},  // P22
  {0, 30, 90, 30},  // P23
  {0, 30, 90, 80}  // P24
  };

// Movement control
const uint16_t moveDelay = 20; // ms between updates
unsigned long lastMoveTime = 0;
bool isMoving = false;
unsigned long moveStartTime = 0;
float moveDuration = 0;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  while (!Serial && millis() < 5000); // Wait for serial port or timeout
  
  Serial.println("\n\nRobot Arm Controller Initializing...");
  
  // Attach servos with custom pulse width for MG90S on Arm1
  if (!base.attach(servoPins[0])) Serial.println("Base servo attach failed");
  if (!arm1.attach(servoPins[1], MG90S_MIN_US, MG90S_MAX_US)) Serial.println("Arm1 (MG90S) attach failed");
  if (!arm2.attach(servoPins[2])) Serial.println("Arm2 servo attach failed");
  if (!gripper.attach(servoPins[3])) Serial.println("Gripper servo attach failed");
  
  // Initialize positions
  writeAllServos();
  delay(1000); // Allow servos to reach initial positions
  
  Serial.println("System Ready!");
}
int i =0;
void loop() {

loopSerial();

}

void loopSerial(){
  // Handle movement
  if (isMoving) {
    updateServos();
  }
  
  // Handle serial commands
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    processCommand(input);
  }  
}
void writeAllServos() {
  base.write(round(currentPos[0]));
  arm1.write(round(currentPos[1]));
  arm2.write(round(currentPos[2]));
  gripper.write(round(currentPos[3]));
}

void updateServos() {
  if (millis() - lastMoveTime < moveDelay) return;
  lastMoveTime = millis();

  float progress = (millis() - moveStartTime) / moveDuration;
  progress = constrain(progress, 0.0, 1.0);
  
  // Update all positions based on progress
  for (int i = 0; i < 4; i++) {
    currentPos[i] = currentPos[i] + (targetPos[i] - currentPos[i]) * progress;
  }
  
  writeAllServos();
  
  if (progress >= 1.0) {
    isMoving = false;
    Serial.println("\nMovement complete!");
    printStatus();
  }
}


void goToPosition(uint8_t posIndex) {
  if (posIndex >= 25) return;
  
  Serial.print("\nMoving to position ");
  Serial.println(posIndex);
  
  // Calculate movement duration based on longest distance
  float maxDistance = 0;
  for (uint8_t i = 0; i < 4; i++) {
    float distance = abs(positions[posIndex][i] - currentPos[i]);
    if (distance > maxDistance) maxDistance = distance;
  }
  
  moveDuration = maxDistance * 15; // Increased scale factor for smoother movement
  moveStartTime = millis();
  
  // Set targets
  for (uint8_t i = 0; i < 4; i++) {
    targetPos[i] = positions[posIndex][i];
    Serial.print("Servo "); Serial.print(i); 
    Serial.print(" moving from "); Serial.print(currentPos[i]);
    Serial.print(" to "); Serial.println(targetPos[i]);
  }
  
  isMoving = true;
  printStatus();
}

void processCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  
  if (cmd == "HOME") goToPosition(0);
  else if (cmd == "P1") goToPosition(1);
  else if (cmd == "P2") goToPosition(2);
  else if (cmd == "P3") goToPosition(3);
  else if (cmd == "P4") goToPosition(4);
  else if (cmd == "P5") goToPosition(5);
  else if (cmd == "P6") goToPosition(6);
  else if (cmd == "P7") goToPosition(7);
  else if (cmd == "P8") goToPosition(8);
  else if (cmd == "P9") goToPosition(9);
  else if (cmd == "P10") goToPosition(10);
  else if (cmd == "P11") goToPosition(11);
  else if (cmd == "P12") goToPosition(12);
  else if (cmd == "P13") goToPosition(13);
  else if (cmd == "P14") goToPosition(14);
  else if (cmd == "P15") goToPosition(15);
  else if (cmd == "P16") goToPosition(16);
  else if (cmd == "P17") goToPosition(17);
  else if (cmd == "P18") goToPosition(18);
  else if (cmd == "P19") goToPosition(19);
  else if (cmd == "P20") goToPosition(20);
  else if (cmd == "P21") goToPosition(21);
  else if (cmd == "P22") goToPosition(22);
  else if (cmd == "P23") goToPosition(23);
  else if (cmd == "P24") goToPosition(24);
  else if (cmd == "STOP") {
    isMoving = false;
    Serial.println("Movement stopped");
  }
  else if (cmd == "STATUS") {
    printStatus();
  }
  else {
    Serial.println("Unknown command");
  }
}



void printStatus() {
  Serial.println("\n=== CURRENT STATUS ===");
  Serial.println("Joint\tCurrent\tTarget");
  Serial.print("Base:\t");
  Serial.print(currentPos[0], 1);
  Serial.print("\t");
  Serial.println(targetPos[0], 1);
  
  Serial.print("Arm1:\t");
  Serial.print(currentPos[1], 1);
  Serial.print("\t");
  Serial.println(targetPos[1], 1);
  
  Serial.print("Arm2:\t");
  Serial.print(currentPos[2], 1);
  Serial.print("\t");
  Serial.println(targetPos[2], 1);
  
  Serial.print("Gripper:\t");
  Serial.print(currentPos[3], 1);
  Serial.print("\t");
  Serial.println(targetPos[3], 1);
  
  Serial.print("State:\t");
  Serial.println(isMoving ? "MOVING" : "READY");
}
