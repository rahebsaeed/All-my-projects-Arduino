#include <Arduino.h>

// Pin definitions for the ultrasonic sensors
#define TRIG_PIN 7         // Single trigger pin for all sensors
#define ECHO_F 8           // Front sensor echo pin
#define ECHO_R 11          // Right sensor echo pin
#define ECHO_L 12          // Left sensor echo pin


// Motor Driver Pins (L298N)
#define M_LEFT_IN1  2
#define M_LEFT_IN2  3
#define M_RIGHT_IN3 4
#define M_RIGHT_IN4 5
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10


#define BASE_SPEED 60
#define SLOWLY 40
#define MAX_SPEED 50
#define TURN_SPEED 80
#define turnTime  300
int currentSpeed;
#define WALL_DISTANCE 30  // space between walls

#define MAX_DISTANCE 400   // Maximum distance in cm
#define TIMEOUT 50000      // Increased timeout in microseconds



// Wall following thresholds
#define WALL_DETECTION_THRESHOLD 8  // cm (consider wall present)
#define DEADBAND 2                  // cm (reduced from 3cm)
#define TARGET_OFFSET 7      // 7cm from each wall

// Sensor sampling rate
#define SENSOR_SAMPLE_INTERVAL 20   // ms (faster than before)

unsigned long lastSensorRead = 0;


int s0 = A0;
int s1 = A1;
int s2 = A2;
int s3 = A3;
int outPin = 6;

void setup() {
    // Motor control pins
  pinMode(M_LEFT_IN1, OUTPUT);
  pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT);
  pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT);
  pinMode(M_RIGHT_ENB, OUTPUT);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(outPin, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
  // Set trigger pin as OUTPUT
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  // Set echo pins as INPUT
  pinMode(ECHO_F, INPUT);
  pinMode(ECHO_R, INPUT);
  pinMode(ECHO_L, INPUT);
  
  
  delay(1000);
}
int i =0;
float Distance_F, Distance_L, Distance_R;


void loop() {
distance1();
delay(20);
}


// Obstacle avoidance states
enum AvoidanceState {
  MOVING_FORWARD,
  SLOWING_DOWN,
  STOPPED,
  TURNING,
  COLOR_CHECK,
  SIDE_PATH_CHECK
};
AvoidanceState currentState = MOVING_FORWARD;

// Add this enum for turn direction tracking
enum TurnDirection { LEFT, RIGHT };
TurnDirection currentTurnDirection;

// Color detection variables
unsigned long lastColorCheckTime = 0;
const unsigned long COLOR_CHECK_INTERVAL = 300; // ms between color checks
unsigned long turnStartTime; // Changed to unsigned long
bool isTurningFromCrossroad = false; // Initialize
const unsigned long TURN_90_DEGREE_TIME = 800; // Made constant, adjust based on your robot

void distance1() {
  measureDistances();
  
  // Detect crossroad (openings on both sides)
  bool isCrossroad = ((Distance_R > WALL_DISTANCE && Distance_L < WALL_DISTANCE) || (Distance_R < WALL_DISTANCE && Distance_L > WALL_DISTANCE));
  
  if (currentState == MOVING_FORWARD && isCrossroad) {
    currentState = SIDE_PATH_CHECK;
    isTurningFromCrossroad = false; // Reset flag
    return;
  }
    if(Distance_R < 5 && Distance_R <15 && Distance_L > 5 || Distance_R > 5 && Distance_R < 15 && Distance_L < 5){
        currentState = STOPPED;
      }

  switch(currentState) {
    case MOVING_FORWARD:
      
      if (Distance_F < 20) {
        stopMotors();
        currentState = COLOR_CHECK;
      } 
      else if (Distance_F < WALL_DISTANCE ) {
        moveForward(SLOWLY);
        currentState = SLOWING_DOWN;
      }
      else {
        moveForward(MAX_SPEED);
      }
      break;
      
    case SLOWING_DOWN:
      if (Distance_F < 15 ) {
        stopMotors();
        currentState = COLOR_CHECK;
      }
      else if (Distance_F > WALL_DISTANCE ) {
        moveForward(MAX_SPEED);
        currentState = MOVING_FORWARD;
      }
      break;
      
    case STOPPED:
      // Decide which way to turn
      if (Distance_R > Distance_L) {
          measureDistances();
          turnRight(TURN_SPEED);
          delay(100);
        currentState = MOVING_FORWARD;

      } else {

        turnLeft(TURN_SPEED);
        delay(100);
        currentState = MOVING_FORWARD;
      }
      currentState = MOVING_FORWARD;
      break;
      
    case TURNING:
      measureDistances();

      
      break;
      
    case COLOR_CHECK:
      handleColorDetection();
      break;
      
    case SIDE_PATH_CHECK:
      handleColorDetection();
      break;
  }
}


bool handleColorDetection() {
  int redFreq = readColorFrequency(LOW, LOW);
  int blueFreq = readColorFrequency(LOW, HIGH);
  int greenFreq = readColorFrequency(HIGH, HIGH);
  
  // blue color - initiate 90-degree turn right
  if (blueFreq < redFreq && blueFreq < greenFreq) {
    turnRight(TURN_SPEED);
    delay(400);
    moveForward(MAX_SPEED);
    delay(300);
    isTurningFromCrossroad = true; // Set flag for special turn handling
    currentState = MOVING_FORWARD;
    return true;
  }
  // Green color - go straight
  else if (greenFreq < redFreq && greenFreq < blueFreq) {
    turnLeft(TURN_SPEED);;
    delay(400);
    moveForward(MAX_SPEED);
    delay(300);
    isTurningFromCrossroad = true; // Set flag for special turn handling
    currentState = MOVING_FORWARD;
    return true;
  }
  // Blue color - stop
  else if (redFreq >35 && redFreq < 60 && greenFreq >120 && greenFreq < 165 && blueFreq <140 && blueFreq >90) {
    stopMotors();
    delay(200);
    while(Distance_R < WALL_DISTANCE && Distance_L < WALL_DISTANCE){
      measureDistances();
      moveBackward(MAX_SPEED);
      delay(20);
    }
    stopMotors();
    delay(100);
    handleColorDetection();
    currentState = MOVING_FORWARD;
    return true;
  }else{
    currentState = STOPPED;
  }
  
  return false;
}

void checkColors() {
  // Simple wrapper for color checking
  handleColorDetection();
}

int readColorFrequency(int s2Val, int s3Val) {
  digitalWrite(s2, s2Val);
  digitalWrite(s3, s3Val);
  delay(10); // Reduced from 100ms for faster response
  return pulseIn(outPin, LOW);
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
  // Apply motor control IN REVERSE DIRECTION
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);    // Reverse polarity for backward movement
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void turnLeft(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void turnRight(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void moveBack(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void turnRight90() {
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
  analogWrite(M_LEFT_ENA, TURN_SPEED);    // Motor speed
  analogWrite(M_RIGHT_ENB, TURN_SPEED);
  delay(turnTime);          // Time for 90-degree turn
  stopMotors();
  delay(100);
}

// Function to turn left 90 degrees
void turnLeft90() {
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
  analogWrite(M_LEFT_ENA, TURN_SPEED);    // Motor speed
  analogWrite(M_RIGHT_ENB, TURN_SPEED);
  delay(turnTime);          // Time for 90-degree turn
  stopMotors();
}
void moveForward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}




void breakMotors(){
  moveBack(MAX_SPEED);
  delay(50);
  stopMotors();
  delay(50);
  moveBack(MAX_SPEED);
  delay(50);
  stopMotors();
  delay(50);

}
void smoothStop() {
  int currentSpeed = MAX_SPEED;
  measureDistances(); // Get current distance
  
  // Only start braking if we're within a certain range (e.g., 50cm)
  if(Distance_F <= 70) {
    while(Distance_F > 30 && currentSpeed > 0) {
      // Reduce speed proportionally to distance
      currentSpeed = map(Distance_F, 20, 50, 0, MAX_SPEED);
      currentSpeed = constrain(currentSpeed, 0, MAX_SPEED);
      
      // Apply the speed
      analogWrite(M_LEFT_ENA, currentSpeed);
      analogWrite(M_RIGHT_ENB, currentSpeed);
      digitalWrite(M_LEFT_IN1, HIGH);
      digitalWrite(M_LEFT_IN2, LOW);
      digitalWrite(M_RIGHT_IN3, HIGH);
      digitalWrite(M_RIGHT_IN4, LOW);
      
      // Get new distance reading
      measureDistances();
      delay(50); // Small delay to prevent sensor flooding
    }
    
    // Final stop
    stopMotors();
  }
}

void measureDistances() {
  long duration_front, duration_right, duration_left;

  // Measure front sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_front = pulseIn(ECHO_F, HIGH, TIMEOUT);
  
  // Small delay between sensors
  delay(10);

  // Measure right sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_right = pulseIn(ECHO_R, HIGH, TIMEOUT);
  
  delay(10);

  // Measure left sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_left = pulseIn(ECHO_L, HIGH, TIMEOUT);

  // Calculate distances
  Distance_F = (duration_front == 0) ? MAX_DISTANCE : (duration_front * 0.034 / 2.0);
  Distance_R = (duration_right == 0) ? MAX_DISTANCE : (duration_right * 0.034 / 2.0);
  Distance_L = (duration_left == 0) ? MAX_DISTANCE : (duration_left * 0.034 / 2.0);
  
  // Apply simple filtering for stability
  static float prev_front = MAX_DISTANCE;
  static float prev_right = MAX_DISTANCE;
  static float prev_left = MAX_DISTANCE;
  
  Distance_F = (Distance_F * 0.7) + (prev_front * 0.3);
  Distance_R = (Distance_R * 0.7) + (prev_right * 0.3);
  Distance_L = (Distance_L * 0.7) + (prev_left * 0.3);
  
  // Save current readings as previous for next iteration
  prev_front = Distance_F;
  prev_right = Distance_R;
  prev_left = Distance_L;
}