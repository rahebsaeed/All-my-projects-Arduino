#include <Arduino.h>

// Pin definitions for the infrared sensors
#define IR_RIGHT 7         
#define IR_LEFT 6         

// Motor Driver Pins (L298N)
#define M_LEFT_IN1  2
#define M_LEFT_IN2  3
#define M_RIGHT_IN3 4
#define M_RIGHT_IN4 5
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10

// PID Constants
#define BASE_SPEED 50      // Base motor speed (0-255)
#define KP 20              // Proportional gain
#define KD 15             // Derivative gain
#define MAX_SPEED_DIFF 50  // Maximum speed difference between motors

// Variables for PID control
int lastError = 0;
unsigned long lastTime = 0;

void setup() {
  // Motor control pins
  pinMode(M_LEFT_IN1, OUTPUT);
  pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT);
  pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT);
  pinMode(M_RIGHT_ENB, OUTPUT);

  // Sensor pins
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_LEFT, INPUT);

  Serial.begin(9600); // For debugging
  delay(1000);
}

void loop() {
  lineFollowingPID();
  delay(10);
}

void lineFollowingPID() {
  int rightSensor = digitalRead(IR_RIGHT);
  int leftSensor = digitalRead(IR_LEFT);
  
  // Calculate error (-2, -1, 0, 1, 2)
  int error = 0;
  if (leftSensor == HIGH && rightSensor == LOW) {
    error = -1; // Line to the left
  } 
  else if (leftSensor == LOW && rightSensor == HIGH) {
    error = 1; // Line to the right
  }
  // Both sensors on or off line means error = 0 (go straight)

  // Calculate time difference for derivative term
  unsigned long currentTime = millis();
  int deltaTime = currentTime - lastTime;
  lastTime = currentTime;
  
  // Calculate PID terms
  int proportional = KP * error;
  int derivative = KD * (error - lastError) / deltaTime;
  lastError = error;
  
  int pidOutput = proportional + derivative;
  
  // Constrain the PID output to prevent extreme values
  pidOutput = constrain(pidOutput, -MAX_SPEED_DIFF, MAX_SPEED_DIFF);
  
  // Apply PID output to motor speeds
  int leftSpeed = BASE_SPEED - pidOutput;
  int rightSpeed = BASE_SPEED + pidOutput;
  
  // Ensure speeds are within valid range (0-255)
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  
  // Move motors with adjusted speeds
  moveForward(leftSpeed, rightSpeed);
}

// --- Motor Control Functions ---
void stopMotors() {
  analogWrite(M_LEFT_ENA, 0);
  analogWrite(M_RIGHT_ENB, 0);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void moveBackward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void moveForward(int leftSpeed, int rightSpeed) {
  // Set directions (forward)
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
  
  // Set speeds
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
}