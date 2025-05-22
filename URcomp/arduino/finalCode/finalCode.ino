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

const int BASE_SPEED = 200;      // Base motor speed for forward movement
const int PUSH_SPEED = 255;      // Maximum speed for pushing (adjust as needed)
const int TURN_SPEED = 220;      // Speed for turning (adjust for turning radius)
const int EDGE_CORRECTION_DELAY = 300; // Milliseconds to turn away from edge
const int SEARCH_DURATION = 1500; // Time to move forward during search (ms)
const int TURN_90_DELAY = 500;   // Time to turn 90 degrees (adjust based on your robot)
const int TURN_60_DELAY = 330;   // Time to turn 60 degrees (500ms * 60/90)
const int delay360 = 2000;   // Time to turn 360 degrees (adjust based on your robot)
const int ULTRASONIC_CHECK_INTERVAL = 30; // ms between ultrasonic checks during turns

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
  delay(10); // Short delay to prevent excessive sensor readings
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
// Updated Edge Detection Function
//-----------------------------------------------------------------------------

bool handleEdgeDetection() {
  // First read all sensors
  bool leftFront = digitalRead(irLeftFront) == EDGE_DETECTED;
  bool rightFront = digitalRead(irRightFront) == EDGE_DETECTED;
  bool leftRear = digitalRead(irLeftRear) == EDGE_DETECTED;
  bool rightRear = digitalRead(irRightRear) == EDGE_DETECTED;

  // If no edges detected, return immediately
  if (!(leftFront || rightFront || leftRear || rightRear)) {
    return false;
  }



  // Re-read sensors after stopping to confirm
  leftFront = digitalRead(irLeftFront) == EDGE_DETECTED;
  rightFront = digitalRead(irRightFront) == EDGE_DETECTED;
  leftRear = digitalRead(irLeftRear) == EDGE_DETECTED;
  rightRear = digitalRead(irRightRear) == EDGE_DETECTED;

  // Then handle the specific edge cases
  // 1. Right rear and right front - turn left and move forward
  if (rightRear && rightFront) {
    turnLeft(TURN_SPEED);
    delay(EDGE_CORRECTION_DELAY);
    moveForward(BASE_SPEED);
    delay(200);
    return true;
  } 
  // 2. Left rear and left front - turn right and move forward
  else if (leftRear && leftFront) {
    turnRight(TURN_SPEED);
    delay(EDGE_CORRECTION_DELAY);
    moveForward(BASE_SPEED);
    delay(200);
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
    moveBackward(BASE_SPEED);
    delay(EDGE_CORRECTION_DELAY);
    turnLeft(TURN_SPEED);
    delay(EDGE_CORRECTION_DELAY);
    return true;
  } 
  // 5. Left front sensor detected - turn right while moving forward
  else if (leftFront) {
    moveBackward(BASE_SPEED);
    delay(EDGE_CORRECTION_DELAY);    
    turnRight(TURN_SPEED);
    delay(EDGE_CORRECTION_DELAY);
    return true;
  }
  else if (rightRear && leftRear) {
    moveForward(BASE_SPEED);
    delay(200);
    return true;
  }
  // 6. Any rear sensor(s) detected - just move forward
  else if (leftRear) {
    turnRight(TURN_SPEED);
    delay(EDGE_CORRECTION_DELAY);
    moveForward(BASE_SPEED);
    delay(200);
    return true;
  }
  else if (rightRear) {
    turnLeft(TURN_SPEED);
    delay(EDGE_CORRECTION_DELAY);
    moveForward(BASE_SPEED);
    delay(200);
    return true;
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
        return false; // Indicate obstacle detected
      }
    }
    
    // Continue turning
    turnFunction(230);
    delay(10);
  }
  return true;
}

void Break(){
  stopMotors();
  delay(10);
  moveBackward(PUSH_SPEED);
  delay(200);
  stopMotors();

}
//-----------------------------------------------------------------------------
// Enhanced Timed Move Forward Function
//-----------------------------------------------------------------------------

bool timedMoveForward(int duration) {
  unsigned long startTime = millis();
  unsigned long lastEdgeCheck = 0;
  const int EDGE_CHECK_INTERVAL = 20; // Check edges every 20ms
  
  while (millis() - startTime < duration) {
    // Check edges more frequently
    if (millis() - lastEdgeCheck >= EDGE_CHECK_INTERVAL) {
      lastEdgeCheck = millis();
      
      // Immediate stop if any edge is detected
      if (digitalRead(irLeftFront) == EDGE_DETECTED || 
          digitalRead(irRightFront) == EDGE_DETECTED ||
          digitalRead(irLeftRear) == EDGE_DETECTED || 
          digitalRead(irRightRear) == EDGE_DETECTED) {
        Break();
        handleEdgeDetection(); // Process the edge case
        return true;
      }
    }
    
    // Check ultrasonic sensor
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
// Aggressive Attack Function
//-----------------------------------------------------------------------------

void aggressiveAttack() {
  unsigned long attackStart = millis();
  while (millis() - attackStart < 1000) { // Attack for 1 second
    // Immediate front edge detection check
    if (digitalRead(irLeftFront) == EDGE_DETECTED || 
        digitalRead(irRightFront) == EDGE_DETECTED) {
      Break();
      return;
    }
    
    // Check all edges
    if (handleEdgeDetection()) {
      Break();
      return;
    }
    
    // Push at maximum speed
    push(PUSH_SPEED);
    delay(10);
  }
}

//-----------------------------------------------------------------------------
// Search Pattern Function (360 turn + 60 turn + forward)
//-----------------------------------------------------------------------------

void searchPattern() {
  // First turn 360 degrees with obstacle checking
  // bool opponentDetected = !timedTurn(turnRight, delay360);
  
  // if (opponentDetected) {
  //   aggressiveAttack();
  //   return;
  // }
  
  // If no opponent found during 360 turn, do 60 degree turn
  if (!timedTurn(turnRight, TURN_90_DELAY)) {
    aggressiveAttack();
    return;
  }
  
  // Then move forward for SEARCH_DURATION with checking
  timedMoveForward(SEARCH_DURATION);
}

//-----------------------------------------------------------------------------
// Decision-Making Function
//-----------------------------------------------------------------------------

void determineAction() {
  // First check for edges - highest priority
  if (handleEdgeDetection()) {
    return;
  }

  long distance = readUltrasonicSensor();

  if (distance < ULTRASONIC_THRESHOLD_CLOSE) {
    aggressiveAttack();
  } 
  else if (distance < ULTRASONIC_THRESHOLD_FAR) {
    // Approach mode with edge checking
    if (!timedMoveForward(500)) {
      return;
    }
  } 
  else {
    // Search mode with new pattern
    searchPattern();
  }
}