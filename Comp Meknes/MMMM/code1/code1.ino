#include <NewPing.h>
#include <Adafruit_TCS34725.h>

/* =========== PIN DECLARATIONS ============ */
// Motor Control Pins
const int IN1 = 2;
const int IN2 = 3;
const int ENA = 9;
const int IN3 = 4;
const int IN4 = 5;
const int ENB = 10;

// Ultrasonic Sensor Pins
const int trigFront = 26;
const int echoFront = 27;
const int trigLeft = 30;
const int echoLeft = 31;
const int trigRight = 22;
const int echoRight = 23;

/* =========== CONSTANTS ============ */
// Movement Timing
const int turnTime = 400;       // Time for 90° turn (ms)
const int uTurnTime = 800;      // Time for 180° turn (ms)
const int stopTime = 300;       // Pause after actions (ms)
const int speed = 100;          // Base speed
const int speedTurn = 120;      // Turning speed
int Distance_F;

// PID Constants - these will need tuning
float Kp = 0.8;   // Proportional gain
float Ki = 0.01;  // Integral gain
float Kd = 0.3;   // Derivative gain

float error = 0;
float lastError = 0;
float P = 0, I = 0, D = 0;
float PIDvalue = 0;

/* =========== SENSOR OBJECTS ============ */
NewPing sonarFront(trigFront, echoFront, 200);
NewPing sonarLeft(trigLeft, echoLeft, 200);
NewPing sonarRight(trigRight, echoRight, 200);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

/* =========== GLOBAL VARIABLES ============ */

enum Color { NONE, GREEN, RED, BLUE };


void moveBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedTurn);
  analogWrite(ENB, speedTurn);
  delay(turnTime);
  stopMotors();
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speedTurn);
  analogWrite(ENB, speedTurn);
  delay(turnTime);
  stopMotors();
}

void Uturn() {
  delay(uTurnTime);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(stopTime);
}

/* =========== SENSOR FUNCTIONS ============ */
int getDistance(NewPing &sensor) {
  return sensor.ping_cm(); // Get distance in cm
}

Color detectColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  // Only detect colors when on a spot (based on light intensity)
  if (c > 50000) {  // Adjust this threshold for 30x30cm spot detection
    if (g > r*1.5 && g > b*1.5) return GREEN;
    if (r > g*1.5 && r > b*1.5) return RED;
    if (b > g*1.5 && b > r*1.5) return BLUE;
  }
  return NONE;
}



void handleColorCommand(Color c) {
  // 1. Immediate stop
  stopMotors();
  
  
  // 3. Execute color command after centering
  switch(c) {
    case GREEN:
      Serial.println("Executing GREEN command - Right turn");
      turnRight();
      break;
      
    case RED:
      Serial.println("Executing RED command - Left turn");
      turnLeft();
      break;
      
    case BLUE:
      Serial.println("Executing BLUE command - U-turn");
      Uturn();
      break;
  }
  
  // 4. Pause before continuing
  delay(1000);
}

void smoothStop() {
  int currentSpeed = 100;
  Distance_F = getDistance(sonarFront); // Get current distance
  
  // Only start braking if we're within a certain range (e.g., 50cm)
  if(Distance_F <= 50) {
    while(Distance_F > 20 && currentSpeed > 0) {
      // Reduce speed proportionally to distance
      currentSpeed = map(Distance_F, 20, 50, 0, 100);
      currentSpeed = constrain(currentSpeed, 0, 100);
      
      // Apply the speed
      analogWrite(ENA, currentSpeed);
      analogWrite(ENB, currentSpeed);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      
      // Get new distance reading
      Distance_F = getDistance(sonarFront);
      delay(50); // Small delay to prevent sensor flooding
    }
    
    // Final stop
    stopMotors();
  }
}

void resetPID() {
  error = 0;
  lastError = 0;
  P = 0;
  I = 0;
  D = 0;
  PIDvalue = 0;
}

float calculatePID() {
  int leftDistance = getDistance(sonarLeft);
  int rightDistance = getDistance(sonarRight);
  
  // Calculate error (difference between sides)
  error = leftDistance - rightDistance;
  
  // Only correct if difference is significant (>3cm)
  if(abs(error) > 3) {
    P = error;
    I = I + error;
    D = error - lastError;
    lastError = error;
    
    PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
    PIDvalue = constrain(PIDvalue, -50, 50); // Limit correction
  } else {
    PIDvalue = 0; // No correction needed
  }
  
  return PIDvalue;
}
/* =========== MOVEMENT FUNCTIONS ============ */
void moveForward(int speed) {
  float correction = calculatePID();
  
  // Apply correction to motor speeds
  int leftSpeed = speed - correction;
  int rightSpeed = speed + correction;
  
  // Constrain speeds to valid range
  leftSpeed = constrain(leftSpeed, 0, 100);
  rightSpeed = constrain(rightSpeed, 0, 100);
  
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward(int speed) {
  float correction = calculatePID();
  
  // Apply correction to motor speeds (inverted for backward)
  int leftSpeed = speed + correction;
  int rightSpeed = speed - correction;
  
  // Constrain speeds to valid range
  leftSpeed = constrain(leftSpeed, 0, 100);
  rightSpeed = constrain(rightSpeed, 0, 100);
  
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


/* =========== MAIN PROGRAM ============ */
void setup() {
  Serial.begin(9600);
  
  // Initialize motor pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  
  // Initialize color sensor
  if (!tcs.begin()) {
    Serial.println("Color sensor error!");
    while(1);
  }
}

void loop() {

  // Check front distance
  int frontDist = sonarRight.ping_cm();
  Serial.println(frontDist);
  if(frontDist < 5 && frontDist != 0) {
    stopMotors();
  } else if(frontDist < 30) {
    smoothStop();
  }else{
    moveForward(100);
  }

}