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
const int ULTRASONIC_THRESHOLD_FAR = 40;   // cm - Distance to trigger charging

const int BASE_SPEED = 220;      // Base motor speed for forward movement
const int PUSH_SPEED = 255;      // Maximum speed for pushing (adjust as needed)
const int TURN_SPEED = 180;      // Speed for turning (adjust for turning radius)
const int EDGE_CORRECTION_DELAY = 300; // Milliseconds to turn away from edge
const int SEARCH_DURATION = 2000; // Time to move forward during search (ms)
const int TURN_90_DELAY = 500;   // Time to turn 90 degrees (adjust based on your robot)
const int delay360 = 2000;   // Time to turn 360 degrees (adjust based on your robot)
const int ULTRASONIC_CHECK_INTERVAL = 100; // ms between ultrasonic checks during turns

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
  delay(400);
}

//-----------------------------------------------------------------------------
// Loop Function
//-----------------------------------------------------------------------------
int i =0;
void loop() {
  if(i== 0){
  moveForward(PUSH_SPEED);
  if(digitalRead(irLeftFront) == EDGE_DETECTED || digitalRead(irRightFront) == EDGE_DETECTED){
    Break();
    i =1;
  }
  }
}
void Break(){
  stopMotors();
  delay(10);
  moveBackward(PUSH_SPEED);
  delay(200);
  stopMotors();

}
//-----------------------------------------------------------------------------
// Motor Control Functions
//-----------------------------------------------------------------------------

void moveForward(int speed) {
  digitalWrite(motorA_IN1, LOW);
  digitalWrite(motorA_IN2, HIGH);
  analogWrite(motorA_Enable, speed);

  digitalWrite(motorB_IN3, LOW);
  digitalWrite(motorB_IN4, HIGH);
  analogWrite(motorB_Enable, speed);
}

void moveBackward(int speed) {
  digitalWrite(motorA_IN1, HIGH);
  digitalWrite(motorA_IN2, LOW);
  analogWrite(motorA_Enable, speed);

  digitalWrite(motorB_IN3, HIGH);
  digitalWrite(motorB_IN4, LOW);
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
  moveForward(speed);
}

//-----------------------------------------------------------------------------
// Sensor Reading Functions
//-----------------------------------------------------------------------------

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
// Revised Attack Behavior in Edge Detection Function
//-----------------------------------------------------------------------------

bool handleEdgeDetection() {
  bool leftFront = digitalRead(irLeftFront) == EDGE_DETECTED;
  bool rightFront = digitalRead(irRightFront) == EDGE_DETECTED;
  bool leftRear = digitalRead(irLeftRear) == EDGE_DETECTED;
  bool rightRear = digitalRead(irRightRear) == EDGE_DETECTED;

  if (leftFront || rightFront || leftRear || rightRear) {
    stopMotors(); // FIRST STOP IMMEDIATELY when any edge is detected
    
    // Then handle the specific edge cases
    // 1. Right rear and right front - turn left and move forward
    if (rightRear && rightFront) {
      turnLeft(TURN_SPEED);
      delay(EDGE_CORRECTION_DELAY);
      moveForward(BASE_SPEED);
      return true;
    } 
    // 2. Left rear and left front - turn right and move forward
    else if (leftRear && leftFront) {
      turnRight(TURN_SPEED);
      delay(EDGE_CORRECTION_DELAY);
      moveForward(BASE_SPEED);
      return true;
    }
    // 3. Both front sensors detected - move backward and turn 90 degrees
    else if (leftFront && rightFront) {
      moveBackward(BASE_SPEED);
      delay(EDGE_CORRECTION_DELAY);
      turnRight(TURN_SPEED);
      delay(TURN_90_DELAY);
      return true;
    }
    // 4. Right front sensor detected - turn left while moving forward
    else if (rightFront) {
      digitalWrite(motorA_IN1, LOW);
      digitalWrite(motorA_IN2, HIGH);
      analogWrite(motorA_Enable, TURN_SPEED);
      digitalWrite(motorB_IN3, LOW);
      digitalWrite(motorB_IN4, LOW);
      analogWrite(motorB_Enable, 0);
      delay(200);
      return true;
    } 
    // 5. Left front sensor detected - turn right while moving forward
    else if (leftFront) {
      digitalWrite(motorA_IN1, LOW);
      digitalWrite(motorA_IN2, LOW);
      analogWrite(motorA_Enable, 0);
      digitalWrite(motorB_IN3, LOW);
      digitalWrite(motorB_IN4, HIGH);
      analogWrite(motorB_Enable, TURN_SPEED);
      delay(200);
      return true;
    }
    // 6. Any rear sensor(s) detected - just move forward
    else if (leftRear || rightRear) {
      moveForward(BASE_SPEED);
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
// Timed Turn Function with Obstacle Check
//-----------------------------------------------------------------------------

bool timedTurn(void (*turnFunction)(int), int duration) {
  unsigned long startTime = millis();
  
  while (millis() - startTime < duration) {
    // Check for edges first
    if (handleEdgeDetection()) {
      return true;
    }
    
    // Check ultrasonic sensor periodically
    if ((millis() - startTime) % ULTRASONIC_CHECK_INTERVAL == 0) {
      long distance = readUltrasonicSensor();
      if (distance < ULTRASONIC_THRESHOLD_FAR) {
        stopMotors();
        return false; // Indicate obstacle detected
      }
    }
    
    // Continue turning
    turnFunction(TURN_SPEED);
    delay(10);
  }
  return true;
}

//-----------------------------------------------------------------------------
// Timed Move Forward Function with Edge and Obstacle Check
//-----------------------------------------------------------------------------

bool timedMoveForward(int duration) {
  unsigned long startTime = millis();
  
  while (millis() - startTime < duration) {
    if (handleEdgeDetection()) {
      return true;
    }
    
    long distance = readUltrasonicSensor();
    if (distance < ULTRASONIC_THRESHOLD_FAR) {
      stopMotors();
      return false;
    }
    
    moveForward(BASE_SPEED);
    delay(10);
  }
  return true;
}

//-----------------------------------------------------------------------------
// Search Pattern Function (360 turn and move forward)
//-----------------------------------------------------------------------------

void searchPattern() {
  // First turn 360 degrees with obstacle checking
  if (!timedTurn(turnRight, delay360)) {
    return; // Obstacle detected during turn
  }
  
  // Then move forward for SEARCH_DURATION with checking
  timedMoveForward(SEARCH_DURATION);
}

//-----------------------------------------------------------------------------
// Decision-Making Function - REVISED ATTACK BEHAVIOR
//-----------------------------------------------------------------------------

void determineAction() {
  // First check for edges - this has highest priority
  if (handleEdgeDetection()) {
    return;
  }

  long distance = readUltrasonicSensor();

  if (distance < ULTRASONIC_THRESHOLD_CLOSE) {
    // AGGRESSIVE ATTACK MODE
    unsigned long attackStart = millis();
    while (millis() - attackStart < 1000) { // Attack for 1 second (adjust as needed)
      // Check for edges continuously during attack
      if (handleEdgeDetection()) {
        return; // Edge detected, stop attacking
      }
      
      // Push at maximum speed
      push(PUSH_SPEED);
      
      // Optional: Add small delay to prevent sensor flooding
      delay(10);
    }
  } 
  else if (distance < ULTRASONIC_THRESHOLD_FAR) {
    // Approach mode with edge checking
    if (!timedMoveForward(500)) { // Shorter approach duration
      return;
    }
  } 
  else {
    // Search mode
    searchPattern();
  }
}
