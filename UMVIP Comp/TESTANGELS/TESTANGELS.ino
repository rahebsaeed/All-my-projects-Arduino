#include <Servo.h>
#include <NewPing.h>
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3); // RX=2, TX=3 (personnalisables)
bool isMovingForward = false;  // State flag

#define TRIG_PIN 13  // Trig pin
#define ECHO_PIN 12  // Echo pin
#define MAX_DISTANCE 200  // Max distance to measure (in cm)

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
// Define servo objects and pins
Servo frontLeftHip, frontLeftKnee, frontRightHip, frontRightKnee;
Servo rearLeftHip, rearLeftKnee, rearRightHip, rearRightKnee;

const int FLHipPin = A2;
const int FLKneePin = 5;
const int FRHipPin = 6;
const int FRKneePin = 9;
const int RLHipPin = 10;
const int RLKneePin = 11;
const int RRHipPin = A0;
const int RRKneePin = A1;

// Individual initial angles for each joint
// LEFT
int FLH_initial = 135;  // Front Left Hip
int RLH_initial = 45;  // Rear Left Hip
int FLK_initial = 0;   // Front Left Knee
int RLK_initial = 180;   // Rear Left Knee
// RIGHT

int FRH_initial = 45;  // Front Right Hip
int RRH_initial = 135;  // Rear Right Hip
int RRK_initial = 0;   // Rear Right Knee
int FRK_initial = 180;   // Front Right Knee

// Movement parameters
int liftAngle = 30;
int stepSize = 40;
int turnAngle = 30;
int stepDelay = 40;

// Movement states
enum Movement { STOP, FORWARD, BACKWARD, LEFT, RIGHT, PHASE2, SPEED, STAIRS, TUNEL };
Movement currentMove = STOP;

void setup() {
  Serial.begin(9600);         // For Serial Monitor
  bluetooth.begin(9600);      // For Bluetooth
  delay(1000);
  // Attach servos to pins
  frontLeftHip.attach(FLHipPin);
  frontLeftKnee.attach(FLKneePin);
  frontRightHip.attach(FRHipPin);
  frontRightKnee.attach(FRKneePin);
  rearLeftHip.attach(RLHipPin);
  rearLeftKnee.attach(RLKneePin);
  rearRightHip.attach(RRHipPin);
  rearRightKnee.attach(RRKneePin);

  // Initialize servos to their custom starting angles
  resetToInitialPositions();
  
  Serial.println("Servos initialized with individual angles!");
  Serial.println("FLH:" + String(FLH_initial) + " FRH:" + String(FRH_initial));
  Serial.println("RLH:" + String(RLH_initial) + " RRH:" + String(RRH_initial));
}

void loop() {
  // Priority 1: Check Bluetooth commands
  if (bluetooth.available()) {
    char command = bluetooth.read();
    Serial.print("Received: ");  // Debug
    Serial.println(command);     // Debug
    
    switch (tolower(command)) {
      case 'd': 
        sendDistance();
        break;
      case 'f': currentMove = FORWARD;  bluetooth.println("[MOVING FORWARD]"); break;
      case 'b': currentMove = BACKWARD; bluetooth.println("[MOVING BACKWARD]"); break;
      case 'l': currentMove = LEFT;     bluetooth.println("[TURNING LEFT]"); break;
      case 'r': currentMove = RIGHT;    bluetooth.println("[TURNING RIGHT]"); break;
      case 't': currentMove = TUNEL;    bluetooth.println("[MOVING TUNEL]"); break;
      case 's': currentMove = STAIRS;    bluetooth.println("[STAIRS]"); break;
      case 'c': currentMove = SPEED;    bluetooth.println("[SPEED]"); break;
      case 'x': currentMove = PHASE2;    bluetooth.println("[PHASE2]"); break;
      case '0': currentMove = STOP;     bluetooth.println("[STOPPED]"); break;
      default:
        bluetooth.println("[UNKNOWN COMMAND]");
    }
  }

  // Priority 2: Execute movement
  executeMovement();

  delay(10);  // Minimal delay
}

void sendDistance() {
  int distance = sonar.ping_cm();
  if (distance > 0) {
    bluetooth.print("Distance: ");
    bluetooth.print(distance);
    bluetooth.println(" cm");
  } else {
    bluetooth.println("Error: No reading!");
  }
}

void executeMovement() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 100) return;  // Throttle to 10Hz
  
  switch (currentMove) {
    case FORWARD:  moveForward();  break;
    case BACKWARD: moveBackward(); break;
    case LEFT:     turnLeft();     break;
    case RIGHT:    turnRight();    break;
    case TUNEL:    navigateTunnel();    break;
    case STAIRS:    stairs();    break;
    case SPEED:    Speed();    break;
    case PHASE2:    phase2();    break;
    case STOP:     stopRobot();    break;
  }
  
  lastUpdate = millis();
}

void stopRobot(){}
// Helper function to set servo angle
void setServo(Servo &servo, int angle) {
  servo.write(angle);
}

// Reset all servos to their individual initial positions
void resetToInitialPositions() {
  frontLeftHip.write(FLH_initial);
  frontLeftKnee.write(FLK_initial);
  frontRightHip.write(FRH_initial);
  frontRightKnee.write(FRK_initial);
  rearLeftHip.write(RLH_initial);
  rearLeftKnee.write(RLK_initial);
  rearRightHip.write(RRH_initial);
  rearRightKnee.write(RRK_initial);
}
void resetToInitialPositionstairs() {
  frontLeftHip.write(FLH_initial);
  frontLeftKnee.write(FLK_initial);
  frontRightHip.write(FRH_initial);
  frontRightKnee.write(FRK_initial);
  rearLeftHip.write(90);
  rearLeftKnee.write(0);
  rearRightHip.write(90);
  rearRightKnee.write(180);
}
// Improved moveForward function using diagonal gait pattern
void moveForward() {
  // Lift and move diagonal pair 1 (Front Left + Rear Right)
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  frontLeftHip.write(FLH_initial + stepSize);
  rearRightHip.write(RRH_initial + stepSize/2);
  delay(stepDelay);
  
  frontLeftKnee.write(FLK_initial);
  rearRightKnee.write(RRK_initial);
  delay(stepDelay/2);
  
  // Push body forward with the other diagonal pair
  frontRightHip.write(FRH_initial - stepSize/2);
  rearLeftHip.write(RLH_initial - stepSize/2);
  delay(stepDelay);
  
  // Lift and move diagonal pair 2 (Front Right + Rear Left)
  frontRightKnee.write(FRK_initial - liftAngle);
  rearLeftKnee.write(RLK_initial - liftAngle);
  delay(stepDelay/2);
  
  frontRightHip.write(FRH_initial + stepSize);
  rearLeftHip.write(RLH_initial + stepSize/2);
  delay(stepDelay);
  
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay/2);
  
  // Push body forward with the other diagonal pair
  frontLeftHip.write(FLH_initial - stepSize/2);
  rearRightHip.write(RRH_initial - stepSize/2);
  delay(stepDelay);
  
  // Return to neutral position
  resetToInitialPositions();
  delay(stepDelay);
}

void moveBackward() {
  // Phase 1: Lift front right and rear left legs
  frontRightKnee.write(FRK_initial - liftAngle);
  rearLeftKnee.write(RLK_initial - liftAngle);
  delay(stepDelay);
  
  // Phase 2: Move lifted legs backward
  frontRightHip.write(FRH_initial - stepSize);
  rearLeftHip.write(RLH_initial - stepSize);
  delay(stepDelay);
  
  // Phase 3: Lower these legs
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay);
  
  // Phase 4: Shift weight backward
  frontLeftHip.write(FLH_initial + stepSize/2);
  rearRightHip.write(RRH_initial + stepSize/2);
  delay(stepDelay*2);
  
  // Phase 5: Lift front left and rear right legs
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay);
  
  // Phase 6: Move these legs backward
  frontLeftHip.write(FLH_initial - stepSize);
  rearRightHip.write(RRH_initial - stepSize);
  delay(stepDelay);
  
  // Phase 7: Lower these legs
  frontLeftKnee.write(FLK_initial);
  rearRightKnee.write(RRK_initial);
  delay(stepDelay);
  
  // Phase 8: Shift weight backward
  frontRightHip.write(FRH_initial + stepSize/2);
  rearLeftHip.write(RLH_initial + stepSize/2);
  delay(stepDelay*2);
  
  resetToInitialPositions();
}

void turnRight() {
  // Lift and move diagonal pair 1 (Front Left + Rear Right) backward
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  frontLeftHip.write(FLH_initial - stepSize);
  rearRightHip.write(RRH_initial - stepSize/2);
  delay(stepDelay);
  
  frontLeftKnee.write(FLK_initial);
  rearRightKnee.write(RRK_initial);
  delay(stepDelay/2);
  
  // Push body backward with the other diagonal pair
  frontRightHip.write(FRH_initial + stepSize/2);
  rearLeftHip.write(RLH_initial + stepSize/2);
  delay(stepDelay);
  
  // Lift and move diagonal pair 2 (Front Right + Rear Left) backward
  frontRightKnee.write(FRK_initial - liftAngle);
  rearLeftKnee.write(RLK_initial - liftAngle);
  delay(stepDelay/2);
  
  frontRightHip.write(FRH_initial - stepSize);
  rearLeftHip.write(RLH_initial - stepSize/2);
  delay(stepDelay);
  
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay/2);
  
  // Push body backward with the other diagonal pair
  frontLeftHip.write(FLH_initial + stepSize/2);
  rearRightHip.write(RRH_initial + stepSize/2);
  delay(stepDelay);
  
  // Return to neutral position
  resetToInitialPositions();
  delay(stepDelay);
}

void turnLeft() {
  // Lift and move diagonal pair 1 (Front Right + Rear Left) backward
  frontRightKnee.write(FRK_initial - liftAngle);
  rearLeftKnee.write(RLK_initial - liftAngle);
  delay(stepDelay/2);
  
  frontRightHip.write(FRH_initial - stepSize);
  rearLeftHip.write(RLH_initial - stepSize/2);
  delay(stepDelay);
  
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay/2);
  
  // Push body backward with the other diagonal pair
  frontLeftHip.write(FLH_initial + stepSize/2);
  rearRightHip.write(RRH_initial + stepSize/2);
  delay(stepDelay);
  
  // Lift and move diagonal pair 2 (Front Left + Rear Right) backward
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  frontLeftHip.write(FLH_initial - stepSize);
  rearRightHip.write(RRH_initial - stepSize/2);
  delay(stepDelay);
  
  frontLeftKnee.write(FLK_initial);
  rearRightKnee.write(RRK_initial);
  delay(stepDelay/2);
  
  // Push body backward with the other diagonal pair
  frontRightHip.write(FRH_initial + stepSize/2);
  rearLeftHip.write(RLH_initial + stepSize/2);
  delay(stepDelay);
  
  // Return to neutral position
  resetToInitialPositions();
  delay(stepDelay);
}
// Tunnel movement parameters
#define tunnelStepDelay 100  // Delay between movements in tunnel
#define tunnelLiftAngle 45   // How much to lift legs when moving

void setupTunnelPosition() {
  // Right side: Hips at 90°, Knees at 0°
  frontRightHip.write(90);
  rearRightHip.write(90);
  frontRightKnee.write(180);
  rearRightKnee.write(180);
  
  // Left side: Hips at 90°, Knees at 180°
  frontLeftHip.write(90);
  rearLeftHip.write(90);
  frontLeftKnee.write(0);
  rearLeftKnee.write(0);
  
  delay(100);  // Allow servos to reach position
}

void moveForwardInTunnel() {
  // Phase 1: Lift front left and rear right legs
  frontLeftKnee.write(0 + tunnelLiftAngle);
  rearRightKnee.write(180 - tunnelLiftAngle);
  delay(tunnelStepDelay);
  
  // Phase 2: Move body forward by adjusting opposite knees
  frontRightKnee.write(180 - tunnelLiftAngle);
  rearLeftKnee.write(0 + tunnelLiftAngle);
  delay(tunnelStepDelay*2);
  
  // Phase 3: Lower lifted legs
  frontLeftKnee.write(0);
  rearRightKnee.write(180);
  delay(tunnelStepDelay);
  
  // Phase 4: Lift front right and rear left legs
  frontRightKnee.write(180 - tunnelLiftAngle);
  rearLeftKnee.write(0 + tunnelLiftAngle);
  delay(tunnelStepDelay);
  
  // Phase 5: Move body forward by adjusting opposite knees
  frontLeftKnee.write(0 + tunnelLiftAngle);
  rearRightKnee.write(180 - tunnelLiftAngle);
  delay(tunnelStepDelay*2);
  
  // Phase 6: Lower lifted legs
  frontRightKnee.write(180);
  rearLeftKnee.write(0);
  delay(tunnelStepDelay);
}

void navigateTunnel() {
  Serial.println("Setting tunnel position...");
  setupTunnelPosition();
  
  Serial.println("Moving through tunnel...");
  for (int i = 0; i < 4; i++) {  // Take 4 steps forward
    moveForwardInTunnel();
  }
  
  Serial.println("Tunnel navigation complete");
}

void Speed() {  
  resetToInitialPositionstairs();
  delay(300); // Longer settling time

  frontLeftKnee.write(FLK_initial - liftAngle);
  // Shift weight diagonally to opposite legs
  frontRightHip.write(FRH_initial );
  rearLeftHip.write(RLH_initial );
  rearRightHip.write(RRH_initial );
 
  
  delay(300); // Longer settling time
  moveForward();
}
void stairs(){
  frontLeftHip.write(90);
  frontLeftKnee.write(60);
  frontRightHip.write(180);
  frontRightKnee.write(135);
  rearLeftHip.write(90);
  rearLeftKnee.write(0);
  rearRightHip.write(90);
  rearRightKnee.write(180);
  delay(300);
  frontRightKnee.write(45);
  ////
  frontLeftHip.write(0);
  frontLeftKnee.write(45);
  frontRightHip.write(90);
  frontRightKnee.write(135);
  rearLeftHip.write(90);
  rearLeftKnee.write(0);
  rearRightHip.write(90);
  rearRightKnee.write(180);
  delay(300);
  moveForward();
  ///
  frontLeftHip.write(90);
  frontLeftKnee.write(0);
  frontRightHip.write(90);
  frontRightKnee.write(180);
  rearLeftHip.write(90);
  rearLeftKnee.write(135);
  rearRightHip.write(90);
  rearRightKnee.write(45);
  delay(300);
}
void phase2(){
  frontLeftHip.write(FLH_initial);
  frontLeftKnee.write(FLK_initial);
  frontRightHip.write(FRH_initial);
  frontRightKnee.write(FRK_initial);
  rearLeftHip.write(90);
  rearLeftKnee.write(RLK_initial);
  rearRightHip.write(RRH_initial);
  rearRightKnee.write(RRK_initial);
  delay(200);
  frontRightHip.write(180);
  frontRightKnee.write(135);
  delay(200);

}