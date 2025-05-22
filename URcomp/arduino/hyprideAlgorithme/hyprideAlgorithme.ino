//-----------------------------------------------------------------------------
// Pin Assignments
//-----------------------------------------------------------------------------

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
const int motorB_Enable = 10; // ENB pin

//-----------------------------------------------------------------------------
// Constants & Thresholds (ADJUST THESE!)
//-----------------------------------------------------------------------------

//  NO LONGER NEEDED. Digital Sensors don't have a threshold
// Now you only care about LOW or HIGH
const int ULTRASONIC_THRESHOLD_CLOSE = 30; // cm - Distance to trigger aggressive push
const int ULTRASONIC_THRESHOLD_FAR = 60;   // cm - Distance to trigger charging

const int BASE_SPEED = 200;      // Base motor speed for forward movement
const int PUSH_SPEED = 255;      // Maximum speed for pushing (adjust as needed)
const int TURN_SPEED = 200;      // Speed for turning (adjust for turning radius)
const int EDGE_CORRECTION_DELAY = 200; // Milliseconds to turn away from edge

//-----------------------------------------------------------------------------
// Search Strategy Configuration
//-----------------------------------------------------------------------------

// Search Strategy Enumeration
enum SearchStrategy {
  RANDOM_SEARCH,
  EXPANDING_SPIRAL,
  WALL_FOLLOW,
  AGGRESSIVE_SWEEP
};

SearchStrategy currentStrategy = RANDOM_SEARCH;  // Initial search strategy

// Search Strategy Selection Parameters
const unsigned long TIME_WITHOUT_SIGHTING_THRESHOLD = 10000; // 10 seconds
const unsigned long WALL_FOLLOW_TIME = 15000;  //Time the robot to start huddling the wall
unsigned long lastOpponentSighting = 0; //Time robot lost track of oppnent

// Random Search Parameters
const int RANDOM_TURN_DURATION_MIN = 200;  // Minimum random turn duration (ms)
const int RANDOM_TURN_DURATION_MAX = 800;  // Maximum random turn duration (ms)
const int RANDOM_FORWARD_DURATION_MIN = 300; // Min forward duration
const int RANDOM_FORWARD_DURATION_MAX = 1000; // Max forward duration

// Expanding Spiral Parameters
const int SPIRAL_INCREMENT_DELAY = 500;    // ms between spiral adjustments
unsigned long spiralTimer = 0;              // Timer for spiral increment
int spiralSpeedIncrement = 10;            // Amount to increment speed each time

bool isTurning = false; // flag if robot is in the middle of a turn or not

//-----------------------------------------------------------------------------
// Attack Strategy Configuration
//-----------------------------------------------------------------------------
const int JOSTLING_INTERVAL = 500;  // milliseconds between jostles
unsigned long lastPushTime = 0;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void setup();
void loop();
void checkEdge();
long readUltrasonicSensor();
void determineAction();
void selectSearchStrategy();
void moveForward(int speed);
void turnLeft(int speed);
void turnRight(int speed);
void stopMotors();
void push(int speed);

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

  // IR Sensor Pins as INPUT_PULLUP (assuming LOW when border is detected)
  pinMode(irLeftFront, INPUT_PULLUP);
  pinMode(irRightFront, INPUT_PULLUP);
  pinMode(irLeftRear, INPUT_PULLUP);
  pinMode(irRightRear, INPUT_PULLUP);

  // Ultrasonic Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial.println("Sumo Robot Ready!");

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

  digitalWrite(motorB_IN3, HIGH);
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

//-----------------------------------------------------------------------------
// Revised checkEdge() function for corner IR sensor placement
//-----------------------------------------------------------------------------
void checkEdge() {
  bool leftFrontOnWhite = digitalRead(irLeftFront) == LOW; // changed to digitalRead
  bool rightFrontOnWhite = digitalRead(irRightFront) == LOW; // changed to digitalRead
  bool leftRearOnWhite = digitalRead(irLeftRear) == LOW; // changed to digitalRead
  bool rightRearOnWhite = digitalRead(irRightRear) == LOW; // changed to digitalRead

  Serial.print("IR Values: LF="); Serial.print(leftFrontOnWhite);  // Printing boolean values
  Serial.print(" RF="); Serial.print(rightFrontOnWhite);
  Serial.print(" LR="); Serial.print(leftRearOnWhite);
  Serial.print(" RR="); Serial.println(rightRearOnWhite);

  if (leftFrontOnWhite || rightFrontOnWhite || leftRearOnWhite || rightRearOnWhite) {  // White Detected
    Serial.println("Edge Detected! Correcting...");
    stopMotors();
    delay(50);

    // Revised Correction Logic (Based on Corner Detection)
    // These are examples - you will want to fine-tune based on your robot's response

    if (leftFrontOnWhite) {
      Serial.println("Left Front Edge Detected: Slight Right Turn");
      turnRight(TURN_SPEED / 2);  // Less aggressive turn
      delay(EDGE_CORRECTION_DELAY);
    }
    if (rightFrontOnWhite) {
      Serial.println("Right Front Edge Detected: Slight Left Turn");
      turnLeft(TURN_SPEED / 2);   // Less aggressive turn
      delay(EDGE_CORRECTION_DELAY);
    }
    if (leftRearOnWhite) {
      Serial.println("Left Rear Edge Detected: Forward and Right Turn");
      moveForward(BASE_SPEED);  // a slight forward movement
      turnRight(TURN_SPEED);  // More aggressive turn
      delay(EDGE_CORRECTION_DELAY);
    }
    if (rightRearOnWhite) {
      Serial.println("Right Rear Edge Detected: Forward and Left Turn");
      moveForward(BASE_SPEED);  // a slight forward movement
      turnLeft(TURN_SPEED);  // More aggressive turn
      delay(EDGE_CORRECTION_DELAY);
    }

    stopMotors();
    delay(50);
    moveForward(BASE_SPEED); // Resume forward motion
  }
}
// Function to read Ultrasonic Sensor
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
// Select Search Strategy
//-----------------------------------------------------------------------------

void selectSearchStrategy() {
  if (millis() - lastOpponentSighting > TIME_WITHOUT_SIGHTING_THRESHOLD) {
    Serial.println("No opponent sighted for a while, switching to more aggressive search (AGGRESSIVE_SWEEP).");
    currentStrategy = AGGRESSIVE_SWEEP;
  } else if (millis() - lastOpponentSighting > WALL_FOLLOW_TIME) {
    Serial.println("Still cant find him Huddling the Wall (WALL_FOLLOW).");
    currentStrategy = WALL_FOLLOW;
  } else {
    // For now, let's just alternate between RANDOM and SPIRAL every 5 seconds

    if (currentStrategy == RANDOM_SEARCH) {
      currentStrategy = EXPANDING_SPIRAL;
      Serial.println("Switching to EXPANDING_SPIRAL.");
    } else {
      currentStrategy = RANDOM_SEARCH;
      Serial.println("Switching to RANDOM_SEARCH.");
    }

  }
}
//-----------------------------------------------------------------------------
// Decision-Making Function (Hybrid Search Strategy)
//-----------------------------------------------------------------------------

void determineAction() {
  checkEdge(); // CHECK FOR EDGE FIRST!  CRITICAL.

  long distance = readUltrasonicSensor();
  Serial.print("Ultrasonic Distance: "); Serial.print(distance); Serial.println(" cm");

  //Declare variables
  bool leftFrontOnWhite = digitalRead(irLeftFront) == LOW; // changed to digitalRead
  bool rightFrontOnWhite = digitalRead(irRightFront) == LOW; // changed to digitalRead
  bool leftRearOnWhite = digitalRead(irLeftRear) == LOW; // changed to digitalRead
  bool rightRearOnWhite = digitalRead(irRightRear) == LOW; // changed to digitalRead

  if (distance < ULTRASONIC_THRESHOLD_CLOSE) {
    Serial.println("Opponent Very Close! Pushing!");
    // Adaptive Pushing
    if (millis() - lastPushTime < JOSTLING_INTERVAL) {
      // We've been pushing for a short time, now jostle forward
      moveForward(PUSH_SPEED);  // Forward push
      lastPushTime = millis();

    } else if (millis() - lastPushTime > JOSTLING_INTERVAL && millis() - lastPushTime < JOSTLING_INTERVAL * 2) {
      // Back up slightly
      moveForward(-PUSH_SPEED);  // Negative speed for backward motion
      lastPushTime = millis();
    } else {
      moveForward(PUSH_SPEED);
      lastPushTime = millis();
    }
    lastOpponentSighting = millis(); // reset the timer

  } else if (distance < ULTRASONIC_THRESHOLD_FAR) {
    Serial.println("Opponent Detected! Charging!");
    moveForward(BASE_SPEED);
    lastOpponentSighting = millis();  // reset the timer
    isTurning = false;

  } else {
    Serial.println("Searching for Opponent...");
    // Select a Search Strategy Every 5 Seconds
    if (millis() % 5000 == 0) { // select strategy every 5 seconds
      selectSearchStrategy();
    }
    switch (currentStrategy) {
      case RANDOM_SEARCH:
        // Randomized Search (Existing Code)
        if (isTurning == false) {

          Serial.println("Performing Randomized Search...");

          // Choose random turn direction
          int turnDirection = random(0, 2); // 0 = Left, 1 = Right

          // Choose random turn duration
          int turnDuration = random(RANDOM_TURN_DURATION_MIN, RANDOM_TURN_DURATION_MAX);

          // Choose random forward duration
          int forwardDuration = random(RANDOM_FORWARD_DURATION_MIN, RANDOM_FORWARD_DURATION_MAX);

          //Check if White Line Is detected and correct first

          if (leftFrontOnWhite || rightFrontOnWhite || leftRearOnWhite || rightRearOnWhite) {
            checkEdge();
          } else {
            if (turnDirection == 0) {
              Serial.print("Randomized Turn Left "); Serial.print(turnDuration);
              turnLeft(TURN_SPEED);
            } else {
              Serial.print("Randomized Turn Right "); Serial.print(turnDuration);
              turnRight(TURN_SPEED);
            }
            isTurning = true; // mark as turning
            delay(turnDuration);
            stopMotors();

            delay(50);
            moveForward(BASE_SPEED);
            delay(forwardDuration); // go forward a short amount of time
            stopMotors();
            isTurning = false;
          }

        }
        break;

      case EXPANDING_SPIRAL:
        // Expanding Spiral Search (Existing Code)
        Serial.println("Performing Expanding Spiral Search...");
        //Check if White Line Is detected and correct first

        if (leftFrontOnWhite || rightFrontOnWhite || leftRearOnWhite || rightRearOnWhite) {
          checkEdge();
        } else {
          moveForward(BASE_SPEED);
          turnRight(50);
        }

        if (millis() - spiralTimer > SPIRAL_INCREMENT_DELAY) {
          Serial.print("Increasing Spiral Speed ");
          spiralSpeedIncrement = (spiralSpeedIncrement + 5) % 100; // prevent it from going too high
          spiralTimer = millis();
        }
        break;

      case WALL_FOLLOW:
        Serial.println("Performing Wall Following!");

        //Check if White Line Is detected and correct first

        if (leftFrontOnWhite || rightFrontOnWhite || leftRearOnWhite || rightRearOnWhite) {
          checkEdge();
        } else {

          //If No Line is Detected Then Start Touching the Wall Randomly
          if (random(0, 2) == 0) {
            turnLeft(BASE_SPEED); //Random Start to Move left Or Right First
          } else {
            turnRight(BASE_SPEED);
          }
        }
        break;

      case AGGRESSIVE_SWEEP:
        Serial.println("Performing Aggressive Sweep!");

        // fast turn to the right.
        turnRight(TURN_SPEED + 50);

        break;

      default:
        // Default: Simple Turn Right
        turnRight(50);
    }
  }
}