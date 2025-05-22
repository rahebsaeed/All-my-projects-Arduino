//-----------------------------------------------------------------------------
// Pin Assignments
//-----------------------------------------------------------------------------

// IR Sensor Pins (Digital - changed from Analog)
const int irLeftFront = 12;    
const int irRightFront = 13;   
const int irLeftRear = 6;     
const int irRightRear = 11; 

// Ultrasonic Sensor Pins
const int trigPin = 8;
const int echoPin = 7;

// L298N Motor Driver Pins
const int motorA_IN1 = 2;
const int motorA_IN2 = 3;
const int motorB_IN3 = 4;
const int motorB_IN4 = 5;
const int motorA_Enable = 9;  // ENA pin
const int motorB_Enable = 10;  // ENB pin 

//-----------------------------------------------------------------------------
// Constants & Thresholds (ADJUST THESE!)
//-----------------------------------------------------------------------------

const bool EDGE_DETECTED = LOW;  // For digital sensors (LOW when white detected)
const int ULTRASONIC_THRESHOLD_CLOSE = 30; // cm - Distance to trigger aggressive push
const int ULTRASONIC_THRESHOLD_FAR = 60;   // cm - Distance to trigger charging

const int BASE_SPEED = 255;      // Base motor speed for forward movement
const int PUSH_SPEED = 255;      // Maximum speed for pushing (adjust as needed)
const int TURN_SPEED = 255;      // Speed for turning (adjust for turning radius)
const int EDGE_CORRECTION_DELAY = 300; // Milliseconds to turn away from edge

//-----------------------------------------------------------------------------
// Setup Function
//-----------------------------------------------------------------------------

void setup() {
  // Motor Driver Pins as OUTPUT
  pinMode(motorA_IN1, OUTPUT);
  pinMode(motorA_IN2, OUTPUT);
  pinMode(motorB_IN3, OUTPUT);
  pinMode(motorB_IN4, OUTPUT);
  pinMode(motorA_Enable, OUTPUT);
  pinMode(motorB_Enable, OUTPUT);

  // IR Sensor Pins as INPUT (digital)
  pinMode(irLeftFront, INPUT);
  pinMode(irRightFront, INPUT);
  pinMode(irLeftRear, INPUT);
  pinMode(irRightRear, INPUT);

  // Ultrasonic Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  // Initial Motor Stop
  stopMotors();
}

//-----------------------------------------------------------------------------
// Loop Function
//-----------------------------------------------------------------------------

void loop() {
  determineAction();
  // moveForward(200);
  delay(10); // Short delay to prevent excessive sensor readings
}

//-----------------------------------------------------------------------------
// Motor Control Functions
//-----------------------------------------------------------------------------

void moveForward(int speed) {
  digitalWrite(motorA_IN1, LOW);
  digitalWrite(motorA_IN2, HIGH);
  analogWrite(motorA_Enable, speed); // PWM for speed control

  digitalWrite(motorB_IN3, LOW);
  digitalWrite(motorB_IN4, HIGH);
  analogWrite(motorB_Enable, speed);
}

void turnLeft(int speed) {
  digitalWrite(motorA_IN1, LOW);
  digitalWrite(motorA_IN2, HIGH);
  analogWrite(motorA_Enable, speed);

  digitalWrite(motorB_IN3, HIGH);
  digitalWrite(motorB_IN4, LOW);
  analogWrite(motorB_Enable, speed);
}

void turnRight(int speed) {
  digitalWrite(motorA_IN1, HIGH);
  digitalWrite(motorA_IN2, LOW);
  analogWrite(motorA_Enable, speed);

  digitalWrite(motorB_IN3, LOW);
  digitalWrite(motorB_IN4, HIGH);
  analogWrite(motorB_Enable, speed);
}

void stopMotors() {
  digitalWrite(motorA_IN1, LOW);
  digitalWrite(motorA_IN2, LOW);
  analogWrite(motorA_Enable, 0);

  digitalWrite(motorB_IN3, LOW);
  digitalWrite(motorB_IN4, LOW);
  analogWrite(motorB_Enable, 0);
}

void push(int speed) {
  moveForward(speed);  // For now, just use forward. You could make it more aggressive.
}

//-----------------------------------------------------------------------------
// Sensor Reading Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Improved checkEdge() Function
//-----------------------------------------------------------------------------

void checkEdge() {
  bool leftFront = digitalRead(irLeftFront);
  bool rightFront = digitalRead(irRightFront);
  bool leftRear = digitalRead(irLeftRear);
  bool rightRear = digitalRead(irRightRear);

  // Debug output


  if (leftFront == EDGE_DETECTED || rightFront == EDGE_DETECTED || 
      leftRear == EDGE_DETECTED || rightRear == EDGE_DETECTED) {
    
    stopMotors();
    delay(50);

    // Front sensors have priority
    if (leftFront == EDGE_DETECTED && rightFront == EDGE_DETECTED) {
      // Both front sensors on edge - reverse and turn
      digitalWrite(motorA_IN1, HIGH);
      digitalWrite(motorA_IN2, LOW);
      digitalWrite(motorB_IN3, HIGH);
      digitalWrite(motorB_IN4, LOW);
      analogWrite(motorA_Enable, BASE_SPEED);
      analogWrite(motorB_Enable, BASE_SPEED);
      delay(EDGE_CORRECTION_DELAY);
      turnRight(TURN_SPEED);
      delay(EDGE_CORRECTION_DELAY);
    } 
    else if (leftFront == EDGE_DETECTED) {
      turnRight(TURN_SPEED);
      delay(EDGE_CORRECTION_DELAY);
    }
    else if (rightFront == EDGE_DETECTED) {
      turnLeft(TURN_SPEED);
      delay(EDGE_CORRECTION_DELAY);
    }
    // Handle rear sensors if front ones are clear
    else if (leftRear == EDGE_DETECTED) {
      moveForward(BASE_SPEED);
      delay(100);
      turnRight(TURN_SPEED);
      delay(EDGE_CORRECTION_DELAY);
    }
    else if (rightRear == EDGE_DETECTED) {
      moveForward(BASE_SPEED);
      delay(100);
      turnLeft(TURN_SPEED);
      delay(EDGE_CORRECTION_DELAY);
    }

    stopMotors();
    delay(50);
    moveForward(BASE_SPEED);
  }
}

long readUltrasonicSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

//-----------------------------------------------------------------------------
// Decision-Making Function (Simple Turn Right Search)
//-----------------------------------------------------------------------------

void determineAction() {
  checkEdge(); // CHECK FOR EDGE FIRST! CRITICAL.

  long distance = readUltrasonicSensor();

  if (distance < ULTRASONIC_THRESHOLD_CLOSE) {
    push(PUSH_SPEED);
  } else if (distance < ULTRASONIC_THRESHOLD_FAR) {
    moveForward(BASE_SPEED);
  } else {
    // Simple Search: Rotate slowly
    turnRight(200);
  }
}