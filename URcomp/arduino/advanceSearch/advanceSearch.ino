//-----------------------------------------------------------------------------
// Pin Assignments
//-----------------------------------------------------------------------------

// IR Sensor Pins (Digital - Use Digital Pins directly, not A0-A3 as analog)
const int irLeftFront = 12;    
const int irRightFront = 13;   
const int irLeftRear = 6;     
const int irRightRear = 11; 
 

// Ultrasonic Sensor Pins
const int trigPin = 8;
const int echoPin = 7;  // This pin assignment will have to change from 7.

// L298N Motor Driver Pins
const int motorA_IN1 = 2;
const int motorA_IN2 = 3;
const int motorB_IN3 = 4; //This pin assignment will have to change from 4.
const int motorB_IN4 = 5; //This pin assignment will have to change from 5.
const int motorA_Enable = 9;  // ENA pin
const int motorB_Enable = 10; // ENB pin

//-----------------------------------------------------------------------------
// Constants & Thresholds (ADJUST THESE!)
//-----------------------------------------------------------------------------

//const int IR_THRESHOLD = 500;  //  NO LONGER NEEDED. Digital Sensors don't have a threshold
// Now you only care about LOW or HIGH
const int ULTRASONIC_THRESHOLD_CLOSE = 30; // cm - Distance to trigger aggressive push
const int ULTRASONIC_THRESHOLD_FAR = 60;   // cm - Distance to trigger charging

const int BASE_SPEED = 200;      // Base motor speed for forward movement
const int PUSH_SPEED = 255;      // Maximum speed for pushing (adjust as needed)
const int TURN_SPEED = 200;      // Speed for turning (adjust for turning radius)
const int EDGE_CORRECTION_DELAY = 200; // Milliseconds to turn away from edge

//-----------------------------------------------------------------------------
// Search Strategy Configuration (RANDOMIZED SEARCH ONLY)
//-----------------------------------------------------------------------------

const int RANDOM_TURN_DURATION_MIN = 200;  // Minimum random turn duration (ms)
const int RANDOM_TURN_DURATION_MAX = 800;  // Maximum random turn duration (ms)
const int RANDOM_FORWARD_DURATION_MIN = 300; // Min forward duration
const int RANDOM_FORWARD_DURATION_MAX = 1000; // Max forward duration

bool isTurning = false; // flag if robot is in the middle of a turn or not

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void setup();
void loop();
void checkEdge();
long readUltrasonicSensor();
void determineAction();
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
  pinMode(irLeftFront, INPUT);
  pinMode(irRightFront, INPUT);
  pinMode(irLeftRear, INPUT);
  pinMode(irRightRear, INPUT);

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
  // Same as moveForward, but potentially higher speed
   moveForward(speed);  // For now, just use forward.  You could make it more aggressive.
}

//-----------------------------------------------------------------------------
// Sensor Reading Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Revised checkEdge() function for corner IR sensor placement
//-----------------------------------------------------------------------------
void checkEdge() {
  bool leftFrontOnWhite = digitalRead(irLeftFront) == LOW; // Changed to digitalRead
  bool rightFrontOnWhite = digitalRead(irRightFront) == LOW; // Changed to digitalRead
  bool leftRearOnWhite = digitalRead(irLeftRear) == LOW; // Changed to digitalRead
  bool rightRearOnWhite = digitalRead(irRightRear) == LOW; // Changed to digitalRead


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
      turnRight(TURN_SPEED);  // Less aggressive turn
      delay(EDGE_CORRECTION_DELAY);
    }
    if (rightFrontOnWhite) {
      Serial.println("Right Front Edge Detected: Slight Left Turn");
      turnLeft(TURN_SPEED);   // Less aggressive turn
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
// Decision-Making Function (Randomized Search ONLY)
//-----------------------------------------------------------------------------

void determineAction() {
  checkEdge(); // CHECK FOR EDGE FIRST!  CRITICAL.

  long distance = readUltrasonicSensor();
  Serial.print("Ultrasonic Distance: "); Serial.print(distance); Serial.println(" cm");

  if (distance < ULTRASONIC_THRESHOLD_CLOSE) {
    Serial.println("Opponent Very Close! Pushing!");
    push(PUSH_SPEED);
    isTurning = false;

  } else if (distance < ULTRASONIC_THRESHOLD_FAR) {
    Serial.println("Opponent Detected! Charging!");
    moveForward(BASE_SPEED);
    isTurning = false;

  } else {
    Serial.println("Searching for Opponent...");

       if (isTurning == false) {

          Serial.println("Performing Randomized Search...");

          // Choose random turn direction
          int turnDirection = random(0, 2); // 0 = Left, 1 = Right

          // Choose random turn duration
          int turnDuration = random(RANDOM_TURN_DURATION_MIN, RANDOM_TURN_DURATION_MAX);

          // Choose random forward duration
          int forwardDuration = random(RANDOM_FORWARD_DURATION_MIN, RANDOM_FORWARD_DURATION_MAX);

        //Check if White Line Is detected and correct first
        bool leftFrontOnWhite = digitalRead(irLeftFront) == LOW; // Changed to digitalRead
        bool rightFrontOnWhite = digitalRead(irRightFront) == LOW; // Changed to digitalRead
        bool leftRearOnWhite = digitalRead(irLeftRear) == LOW; // Changed to digitalRead
        bool rightRearOnWhite = digitalRead(irRightRear) == LOW; // Changed to digitalRead

        if (leftFrontOnWhite || rightFrontOnWhite || leftRearOnWhite || rightRearOnWhite){
           checkEdge();
          }
          else{
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

  }
}