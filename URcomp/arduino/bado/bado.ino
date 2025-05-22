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
const int TURN_SPEED = 220;      // Speed for turning (adjust for turning radius)
const int EDGE_CORRECTION_DELAY = 300; // Milliseconds to turn away from edge
const int SEARCH_DURATION = 1000; // Time to move forward during search (ms)
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
  // Read all sensors
  bool leftFront = digitalRead(irLeftFront) == EDGE_DETECTED;
  bool rightFront = digitalRead(irRightFront) == EDGE_DETECTED;
  bool leftRear = digitalRead(irLeftRear) == EDGE_DETECTED;
  bool rightRear = digitalRead(irRightRear) == EDGE_DETECTED;

  // If no edges detected, return immediately
  if (!(leftFront || rightFront || leftRear || rightRear)) {
    return false;
  }

  // Stop motors before any corrective action
  stopMotors();
  delay(50); // Brief pause to ensure stopped

  // Re-read sensors after stopping
  leftFront = digitalRead(irLeftFront) == EDGE_DETECTED;
  rightFront = digitalRead(irRightFront) == EDGE_DETECTED;
  leftRear = digitalRead(irLeftRear) == EDGE_DETECTED;
  rightRear = digitalRead(irRightRear) == EDGE_DETECTED;

  // Handle edge cases with sensor-based control
  // 1. Right rear and right front - turn left until edge clears
  if (rightRear && rightFront) {
    while (digitalRead(irRightRear) == EDGE_DETECTED || 
           digitalRead(irRightFront) == EDGE_DETECTED) {
      turnLeft(TURN_SPEED);
      delay(10);
    }
    moveForward(BASE_SPEED);
    // Move forward while monitoring front sensors
    while (digitalRead(irLeftFront) != EDGE_DETECTED && 
           digitalRead(irRightFront) != EDGE_DETECTED) {
      moveForward(BASE_SPEED);
      delay(10);
    }
    return true;
  } 
  // 2. Left rear and left front - turn right until edge clears
  else if (leftRear && leftFront) {
    while (digitalRead(irLeftRear) == EDGE_DETECTED || 
           digitalRead(irLeftFront) == EDGE_DETECTED) {
      turnRight(TURN_SPEED);
      delay(10);
    }
    moveForward(BASE_SPEED);
    // Move forward while monitoring front sensors
    while (digitalRead(irLeftFront) != EDGE_DETECTED && 
           digitalRead(irRightFront) != EDGE_DETECTED) {
      moveForward(BASE_SPEED);
      delay(10);
    }
    return true;
  }
  // 3. Both front sensors detected - move backward then turn
  else if (leftFront && rightFront) {
    // Move backward until front edges clear
    while (digitalRead(irLeftFront) == EDGE_DETECTED || 
           digitalRead(irRightFront) == EDGE_DETECTED) {
      moveBackward(BASE_SPEED);
      delay(10);
    }
    // Turn right until all edges clear
    while (digitalRead(irLeftFront) == EDGE_DETECTED || 
           digitalRead(irRightFront) == EDGE_DETECTED ||
           digitalRead(irLeftRear) == EDGE_DETECTED || 
           digitalRead(irRightRear) == EDGE_DETECTED) {
      turnRight(TURN_SPEED);
      delay(10);
    }
    return true;
  }
  // 4. Right front sensor detected - turn left until clear
  else if (rightFront) {
    while (digitalRead(irRightFront) == EDGE_DETECTED) {
    digitalWrite(motorA_IN1, LOW);
    digitalWrite(motorA_IN2, HIGH);
    analogWrite(motorA_Enable, 0);

    digitalWrite(motorB_IN3, HIGH);
    digitalWrite(motorB_IN4, LOW);
    analogWrite(motorB_Enable, 255);      
  delay(10);
    }
    return true;
  } 
  // 5. Left front sensor detected - turn right until clear
  else if (leftFront) {
    while (digitalRead(irLeftFront) == EDGE_DETECTED) {
  digitalWrite(motorA_IN1, HIGH);
  digitalWrite(motorA_IN2, LOW);
  analogWrite(motorA_Enable, 255);

  digitalWrite(motorB_IN3, LOW);
  digitalWrite(motorB_IN4, HIGH);
  analogWrite(motorB_Enable, 0);      
  delay(10);
    }
    return true;
  }
  // 6. Both rear sensors detected - move forward until clear
  else if (rightRear && leftRear) {
    
  unsigned long moveStart = millis();
    while (digitalRead(irRightFront) != EDGE_DETECTED && digitalRead(irLeftFront) != EDGE_DETECTED &&
            millis() - moveStart < 200) {
      moveForward(BASE_SPEED);
      delay(10);
    }
    return true;
  }
  // 7. Left rear sensor detected - turn right until clear
  else if (leftRear) {
    while (digitalRead(irLeftRear) == EDGE_DETECTED) {
      turnRight(TURN_SPEED);
      delay(10);
    }
    moveForward(BASE_SPEED);
    unsigned long moveStart = millis();
    // Move forward while monitoring front sensors
    while (digitalRead(irLeftFront) != EDGE_DETECTED &&  digitalRead(irRightFront) != EDGE_DETECTED && millis() - moveStart < 200) {
      moveForward(BASE_SPEED);
      delay(10);
    }
    return true;
  }
  // 8. Right rear sensor detected - turn left until clear
  else if (rightRear) {
    while (digitalRead(irRightRear) == EDGE_DETECTED) {
      turnLeft(TURN_SPEED);
      delay(10);
    }
    moveForward(BASE_SPEED);
    unsigned long moveStart = millis();
    // Move forward while monitoring front sensors
    while (digitalRead(irLeftFront) != EDGE_DETECTED && digitalRead(irRightFront) != EDGE_DETECTED && millis() - moveStart < 200) {
      moveForward(BASE_SPEED);
      delay(10);
    }
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
  delay(150);
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
  if (!timedTurn(turnRight, TURN_90_DELAY*2-45)) {
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