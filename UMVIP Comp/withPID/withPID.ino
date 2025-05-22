#include <Servo.h>
#include <PID_v1.h>

// Define servo objects and pins
Servo frontLeftHip, frontLeftKnee, frontRightHip, frontRightKnee;
Servo rearLeftHip, rearLeftKnee, rearRightHip, rearRightKnee;

const int FLHipPin = 3;
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


// ******************** PID ********************
// Define PID variables for each joint.  Start with just one for testing
double FLH_Input, FLH_Output, FLH_Setpoint;
double Kp=2, Ki=0.1, Kd=0.1;  // Tune these!
PID FLH_PID(&FLH_Input, &FLH_Output, &FLH_Setpoint, Kp, Ki, Kd, DIRECT);  // Define the PID object

// Function to read the current angle (Placeholder - needs actual sensor)
// Replace this with code that reads the actual angle from a sensor
// For now, it just returns the last written angle.  This is a *MUST* fix.
int readJointAngle(Servo &servo) {
  //  This is a major simplification.  You *NEED* an angle sensor to do this right.
  //  Consider using a potentiometer attached to the servo shaft.

  // This returns the last set position. It won't reflect disturbances.
  //  This is only suitable for testing *without* external disturbances.
  return servo.read();
}

// ******************** END PID ********************


void setup() {
  Serial.begin(9600);
  
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

  // ******************** PID Setup ********************
  // Set the initial setpoint for the Front Left Hip
  FLH_Setpoint = FLH_initial;
  FLH_Input = readJointAngle(frontLeftHip); // get a starting point
  FLH_PID.SetMode(AUTOMATIC);
  FLH_PID.SetOutputLimits(0, 180);  // Limit the output to the servo range

  // ******************** END PID Setup ********************
}

void loop() {
  // Example: Walk forward
  moveForward();
  delay(100);
}

// Helper function to set servo angle - NOW USING PID
void setServo(Servo &servo, int angle) {
  //OLD - direct servo writing
  //servo.write(angle);

  // ******************** PID Control ********************
  // This is an example for *ONE* servo (frontLeftHip).  You will need
  // to duplicate and adapt this for each servo you want to control with PID.

    // Update the input with the current joint angle (READ FROM SENSOR!)
    FLH_Input = readJointAngle(frontLeftHip);

    // Set the desired setpoint (this might come from your movement calculations)
    FLH_Setpoint = angle;  // 'angle' is the desired angle passed to the function

    // Compute the PID output
    FLH_PID.Compute();

    // Write the PID output to the servo
    servo.write(FLH_Output); // frontLeftHip.write(FLH_Output);
  // ******************** END PID Control ********************
}

// Reset all servos to their individual initial positions
void resetToInitialPositions() {
  setServo(frontLeftHip, FLH_initial);
  frontLeftKnee.write(FLK_initial); //Knee stays as direct servo since no PID
  frontRightHip.write(FRH_initial);
  frontRightKnee.write(FRK_initial);
  rearLeftHip.write(RLH_initial);
  rearLeftKnee.write(RLK_initial);
  rearRightHip.write(RRH_initial);
  rearRightKnee.write(RRK_initial);
}

// Improved moveForward function using diagonal gait pattern
void moveForward() {
  // Lift and move diagonal pair 1 (Front Left + Rear Right)
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServo(frontLeftHip, FLH_initial + stepSize);
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
  
  setServo(frontRightHip, FRH_initial + stepSize);
  rearLeftHip.write(RLH_initial + stepSize/2);
  delay(stepDelay);
  
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay/2);
  
  // Push body forward with the other diagonal pair
  setServo(frontLeftHip, FLH_initial - stepSize/2);
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
   setServo(frontRightHip, FRH_initial - stepSize);
  rearLeftHip.write(RLH_initial - stepSize);
  delay(stepDelay);
  
  // Phase 3: Lower these legs
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay);
  
  // Phase 4: Shift weight backward
 setServo(frontLeftHip, FLH_initial + stepSize/2);
  rearRightHip.write(RRH_initial + stepSize/2);
  delay(stepDelay*2);
  
  // Phase 5: Lift front left and rear right legs
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay);
  
  // Phase 6: Move these legs backward
   setServo(frontLeftHip, FLH_initial - stepSize);
  rearRightHip.write(RRH_initial - stepSize);
  delay(stepDelay);
  
  // Phase 7: Lower these legs
  frontLeftKnee.write(FLK_initial);
  rearRightKnee.write(RRK_initial);
  delay(stepDelay);
  
  // Phase 8: Shift weight backward
  setServo(frontRightHip, FRH_initial + stepSize/2);
  rearLeftHip.write(RLH_initial + stepSize/2);
  delay(stepDelay*2);
  
  resetToInitialPositions();
}

void turnRight() {
  // Lift and move diagonal pair 1 (Front Left + Rear Right) backward
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay/2);

  setServo(frontLeftHip, FLH_initial - stepSize);
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

  setServo(frontRightHip, FRH_initial - stepSize);
  rearLeftHip.write(RLH_initial - stepSize/2);
  delay(stepDelay);
  
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay/2);
  
  // Push body backward with the other diagonal pair
  setServo(frontLeftHip, FLH_initial + stepSize/2);
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

  setServo(frontRightHip, FRH_initial - stepSize);
  rearLeftHip.write(RLH_initial - stepSize/2);
  delay(stepDelay);
  
  frontRightKnee.write(FRK_initial);
  rearLeftKnee.write(RLK_initial);
  delay(stepDelay/2);
  
  // Push body backward with the other diagonal pair
  setServo(frontLeftHip, FLH_initial + stepSize/2);
  rearRightHip.write(RRH_initial + stepSize/2);
  delay(stepDelay);
  
  // Lift and move diagonal pair 2 (Front Left + Rear Right) backward
  frontLeftKnee.write(FLK_initial - liftAngle);
  rearRightKnee.write(RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServo(frontLeftHip, FLH_initial - stepSize);
  rearRightHip.write(RRH_initial - stepSize/2);
  delay(stepDelay);
  
  frontLeftKnee.write(FLK_initial);
  rearRightKnee.write(RRK_initial);
  delay(stepDelay/2);
  
  // Push body backward with the other diagonal pair
  setServo(frontRightHip, FRH_initial + stepSize/2);
  rearLeftHip.write(RLH_initial + stepSize/2);
  delay(stepDelay);
  
  // Return to neutral position
  resetToInitialPositions();
  delay(stepDelay);
}
// Tunnel movement parameters
#define tunnelStepDelay 200  // Delay between movements in tunnel
#define tunnelLiftAngle 45   // How much to lift legs when moving

void setupTunnelPosition() {
  // Right side: Hips at 90°, Knees at 0°
  setServo(frontRightHip, 90);
   setServo(rearRightHip, 90);
  frontRightKnee.write(180);
  rearRightKnee.write(180);
  
  // Left side: Hips at 90°, Knees at 180°
  setServo(frontLeftHip, 90);
   setServo(rearLeftHip, 90);
  frontLeftKnee.write(0);
  rearLeftKnee.write(0);
  
  delay(200);  // Allow servos to reach position
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
  rearRightKnee.write(190);
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