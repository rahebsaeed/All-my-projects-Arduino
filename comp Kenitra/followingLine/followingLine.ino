#include <Arduino.h>

// Pin definitions for the infrared sensors
#define IR_RIGHT 11        
#define IR_LEFT 12        

// Motor Driver Pins (L298N)
#define M_LEFT_IN1  4
#define M_LEFT_IN2  5
#define M_RIGHT_IN3 6
#define M_RIGHT_IN4 7
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10

// Constants
#define BASE_SPEED 50      // Base motor speed (0-255)
#define TURN_SPEED 90      // Speed during turns (0-255)

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
  lineFollowing();
}

// Add these global variables at the top of your program
enum TurnDirection { NONE, LEFT_TURN, RIGHT_TURN };
TurnDirection lastTurn = NONE;

void lineFollowing() {
  int rightSensor = digitalRead(IR_RIGHT);
  int leftSensor = digitalRead(IR_LEFT);

  // Line following logic
  if (leftSensor == LOW && rightSensor == LOW) {
    // Both sensors on the line - move forward
    moveForward(BASE_SPEED);
    lastTurn = NONE;  // Reset turn memory when on line
  } 
  else if (leftSensor == HIGH && rightSensor == LOW) {
    // Line detected only by right sensor - turn right
    turnRight(TURN_SPEED);
    lastTurn = RIGHT_TURN;
  } 
  else if (leftSensor == LOW && rightSensor == HIGH) {
    // Line detected only by left sensor - turn left
    turnLeft(TURN_SPEED);
    lastTurn = LEFT_TURN;
  }
  else {
    // Both sensors off the line - use last known turn direction
    if (lastTurn == RIGHT_TURN) {
      // Continue turning right until line is found
      turnRight(TURN_SPEED);
    } 
    else if (lastTurn == LEFT_TURN) {
      // Continue turning left until line is found
      turnLeft(TURN_SPEED);
    }
    else {
      // No previous turn direction - stop or search
      stopMotors();
      // Optional: add search pattern here
    }
  }
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

void turnLeft(int speed) {
  // Left motor backward, right motor forward
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void turnRight(int speed) {
  // Left motor forward, right motor backward
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void moveForward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}