#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- SERVO CONFIGURATION ---
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int frontLeftHip     = 3;
const int frontLeftKnee    = 5;
const int frontRightHip    = 6;
const int frontRightKnee   = 9;
const int rearLeftHip      = 10;
const int rearLeftKnee     = 11;
const int rearRightHip     = A0;
const int rearRightKnee    = A2;

// Servo calibration values (adjust these)
const int servoMin = 150; // Minimum pulse length
const int servoMax = 600; // Maximum pulse length

// --- ROBOT DIMENSIONS (MEASURE CAREFULLY) ---
const float thighLength = 61.20;    // Length of upper leg (mm)
const float shinLength  = 86.60;    // Length of lower leg (mm)
const float bodyHeight  = 150.0;    // Target height of the body from the ground (mm)
const float initialHipAngle = 146.67; // the degree of hip from surface of body

// --- BASE STANCE COORDINATES (ADJUST FOR STABILITY) ---
// These are the initial X, Y positions of the feet relative to the body center.
// X is forward/backward, Y is left/right.
float frontLeftX   =  50.0;
float frontLeftY   =  70.0;
float frontRightX  =  50.0;
float frontRightY  = -70.0;
float rearLeftX    = -50.0;
float rearLeftY    =  70.0;
float rearRightX   = -50.0;
float rearRightY   = -70.0;

// --- WALKING PARAMETERS ---
const float stepSize = 20.0;      // How far the feet move in each step (mm)
const float stepHeight = 15.0;    // How high the feet lift off the ground (mm)
const float walkSpeed = 500;     // Delay between steps (milliseconds)

// --- GLOBAL VARIABLES ---
char command = ' ';                // Stores the serial command

// --- FUNCTIONS ---

// Function to convert angle in degrees to pulse width for servo
int angleToPulse(float angle) {
  float pulse = (angle / 180.0) * (servoMax - servoMin) + servoMin;
  return (int)pulse; // Ensure we return an integer
}

// Function to set the servo angle using PCA9685
void setServoAngle(int servoChannel, float angle) {
  int pulse = angleToPulse(angle);
  pwm.setPWM(servoChannel, 0, pulse);
}

// Function to calculate inverse kinematics
void calculateIK(float targetX, float targetY, float bodyHeight, float &hipAngle, float &kneeAngle) {
  // Calculate distance from hip to target position
  float distance = sqrt(pow(targetX, 2) + pow((bodyHeight+targetY), 2));

  // Use law of cosines to calculate knee angle
  float cosKneeAngle = (pow(thighLength, 2) + pow(shinLength, 2) - pow(distance, 2)) / (2 * thighLength * shinLength);

  // Check if target is reachable by leg
  if (cosKneeAngle < -1.0 || cosKneeAngle > 1.0) {
    Serial.println("Target out of reach!");
    hipAngle  = 0.0;
    kneeAngle = 0.0;
    return;
  }

  kneeAngle = degrees(acos(cosKneeAngle));

  // Use law of cosines to calculate hip angle
  float cosHipAngle = (pow(thighLength, 2) + pow(distance, 2) - pow(shinLength, 2)) / (2 * thighLength * distance);

  // Check if target is reachable by leg
  if (cosHipAngle < -1.0 || cosHipAngle > 1.0) {
      Serial.println("Target out of reach!");
      hipAngle  = 0.0;
      kneeAngle = 0.0;
      return;
  }
  
  hipAngle = degrees(acos(cosHipAngle));
  hipAngle=hipAngle+initialHipAngle;
}
// Function to set the leg position. This is what you would call to move the legs.
void setLegPosition(int hipServo, int kneeServo, float targetX, float targetY)
{
    float hipAngle;
    float kneeAngle;

    calculateIK(targetX, targetY, bodyHeight, hipAngle, kneeAngle);
    Serial.print("Hip Angle :  "); Serial.println(hipAngle); Serial.print("knee Angle :  ");Serial.println(kneeAngle);
    if(hipAngle != 0 || kneeAngle != 0){
        setServoAngle(hipServo, hipAngle);
        setServoAngle(kneeServo, kneeAngle);
    }
}

// Function to perform a walking step
void walkingStep(char direction) {
  float stepModifier = (direction == 'F') ? stepSize : -stepSize;

  // Move front legs forward/backward
    setLegPosition(frontLeftHip, frontLeftKnee, frontLeftX + stepModifier, frontLeftY);
    setLegPosition(frontRightHip, frontRightKnee, frontRightX + stepModifier, frontRightY);

  delay(walkSpeed);

  // Move rear legs forward/backward
  setLegPosition(rearLeftHip, rearLeftKnee, rearLeftX - stepModifier, rearLeftY);
    setLegPosition(rearRightHip, rearRightKnee, rearRightX - stepModifier, rearRightY);
    

  delay(walkSpeed);

    setLegPosition(frontLeftHip, frontLeftKnee, frontLeftX, frontLeftY);
    setLegPosition(frontRightHip, frontRightKnee, frontRightX , frontRightY);

  delay(walkSpeed);

  // Move rear legs forward/backward
  setLegPosition(rearLeftHip, rearLeftKnee, rearLeftX, rearLeftY);
    setLegPosition(rearRightHip, rearRightKnee, rearRightX, rearRightY);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Robot Starting...");

  pwm.begin();
  pwm.setPWMFreq(60); // Standard servo frequency

  // Give servos time to initialize
  delay(1000);

  // Initial leg positioning (set to base stance)
    setLegPosition(frontLeftHip, frontLeftKnee, frontLeftX, frontLeftY);
    setLegPosition(frontRightHip, frontRightKnee, frontRightX, frontRightY);
    setLegPosition(rearLeftHip, rearLeftKnee, rearLeftX, rearLeftY);
    setLegPosition(rearRightHip, rearRightKnee, rearRightX, rearRightY);

  Serial.println("Initialization Complete.");
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    command = Serial.read();
    command = toupper(command); // Convert to uppercase for easier checking

    if (command == 'F' || command == 'B') {
      walkingStep(command);  // Perform a walking step forward or backward
    } else {
      Serial.println("Invalid command. Use 'F' for forward or 'B' for backward.");
    }
  }
}