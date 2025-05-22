#include <NewPing.h>

// Ultrasonic Sensor Pins
const int trigFront = 7;
const int echoFront = 8;
const int trigLeft = 13;
const int echoLeft = A4;
const int trigRight = 11;
const int echoRight = 12;

// Motor Control Pins
const int IN1 = 2;
const int IN2 = 3;
const int ENA = 9; // Left motor speed control
const int IN3 = 4;
const int IN4 = 5;
const int ENB = 10; // Right motor speed control

// Parameters
const int pathWidth = 30; // Maze path width in cm
const float robotWidth = 15.5; // Robot width in cm
const float targetDistance = (pathWidth - robotWidth) / 2; // Ideal distance to walls = 7.5 cm
const float deadZone = 0.5; // Dead zone to prevent oscillations
const float immediateCorrectionThreshold = 5.0; // Threshold for immediate correction

// Dual PID Control Parameters
float Kp_position = 12;  // Position (distance) correction gain
float Kp_heading = 3;    // Heading correction gain
float lastError = 0;

NewPing sonarFront(trigFront, echoFront, 200);
NewPing sonarLeft(trigLeft, echoLeft, 200);
NewPing sonarRight(trigRight, echoRight, 200);

int getDistance(NewPing &sensor) {
  int distance = sensor.ping_cm();
  return (distance == 0) ? 200 : distance;
}

void moveForwardWithCorrection() {
  static float lastLeftDist = targetDistance;
  static float lastRightDist = targetDistance;
  
  // Get distances
  int leftDist = getDistance(sonarLeft);
  int rightDist = getDistance(sonarRight);
  
  // Calculate position error (distance from ideal position)
  float positionError = (leftDist - rightDist) / 2;
  
  // Calculate heading error (change in distances over time)
  float headingError = ((leftDist - lastLeftDist) - (rightDist - lastRightDist));
  
  // Store current distances for next iteration
  lastLeftDist = leftDist;
  lastRightDist = rightDist;
  int baseSpeed = 255;  

  // Calculate total correction
  float correction = ((baseSpeed/10) * positionError) + ((baseSpeed/4) * headingError);
  
  // Calculate motor speeds

  int leftSpeed = constrain(baseSpeed - correction, 50, 255);
  int rightSpeed = constrain(baseSpeed + correction, 50, 255);
  
  // Apply speeds to motors
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void setup() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
}

void loop() {
  if (getDistance(sonarFront) > 40) {
    moveForwardWithCorrection();
  } else {
    stopMotors();
  }
  delay(10);
}