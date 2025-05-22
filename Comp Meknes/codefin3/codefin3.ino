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
#define MAX_SPEED 70
#define TURN_SPEED 80
#define turnTime  300
int currentSpeed;
#define WALL_DISTANCE 30  // space between walls
#define OPENING_THRESHOLD 90  // space between walls
#define TURN_90_DEGREE_TIME 300  // space between walls

#define MAX_DISTANCE 400   // Maximum distance in cm
#define TIMEOUT 50000      // Increased timeout in microseconds



// Wall following thresholds
#define WALL_DETECTION_THRESHOLD 8  // cm (consider wall present)
#define DEADBAND 2                  // cm (reduced from 3cm)
#define TARGET_OFFSET 7      // 7cm from each wall
#define CORRECTION_THRESHOLD 10
#define REVERSE_DURATION 100

// Sensor sampling rate
#define SENSOR_SAMPLE_INTERVAL 20   // ms (faster than before)

unsigned long lastSensorRead = 0;


// 1. FIRST DECLARE ALL ENUMS
enum TurnDirection { LEFT, RIGHT };

enum AvoidanceState {
  MOVING_FORWARD,
  SLOWING_DOWN,
  STOPPED,
  TURNING,
  COLOR_CHECK,
  REVERSING
};

enum ColorScenario {
  NO_SCENARIO,
  FRONT_OBSTACLE,
  RIGHT_OPENING,
  LEFT_OPENING
};

// 2. THEN DECLARE FUNCTION PROTOTYPES
void gentleAdjust(TurnDirection direction);
bool handleColorDetection();
void distance1();
int readColorFrequency(int s2Val, int s3Val);
void measureDistances();

// 3. THEN DECLARE GLOBAL VARIABLES
AvoidanceState currentState = MOVING_FORWARD;
ColorScenario colorScenario = NO_SCENARIO;
unsigned long turnStartTime;
unsigned long reverseStartTime;
bool isTurningFromColor = false;
float Distance_F, Distance_L, Distance_R;
int i = 0;

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


void loop() {
distance1();
delay(20);
}

void tstColors(){
  int redFreq = readColorFrequency(LOW, LOW);     // Rouge
  int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
  int greenFreq = readColorFrequency(HIGH, HIGH); // Vert

  String colorName = "Aucune couleur détectée";

  // Seuil : si tous sont au-dessus de 200, alors rien n’est détecté
    // Plus la fréquence est basse, plus la couleur est dominante
    if (redFreq >35 && redFreq < 60 && greenFreq >120 && greenFreq < 165 && blueFreq <140 && blueFreq >90 ) {
      turnRight(TURN_SPEED);      
      colorName = "Rouge";

    }
    else if (greenFreq < redFreq && greenFreq < blueFreq) {
      moveForward(BASE_SPEED);
      i=1;
      colorName = "Vert";
    }
    else if (blueFreq < redFreq && blueFreq < greenFreq) {
      colorName = "Bleu";
      if(i == 1){
        breakMotors();
      }
      i=0;
      stopMotors();

    }


  delay(20);
}



// 4. NOW IMPLEMENT THE FUNCTIONS
void gentleAdjust(TurnDirection direction) {
  if (direction == LEFT) {
    // Adjust left (right motor faster)
    analogWrite(M_LEFT_ENA, MAX_SPEED * 0.8);
    analogWrite(M_RIGHT_ENB, MAX_SPEED);
  } else {
    // Adjust right (left motor faster)
    analogWrite(M_LEFT_ENA, MAX_SPEED);
    analogWrite(M_RIGHT_ENB, MAX_SPEED * 0.8);
  }
  
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
  
  delay(100);
  moveForward(MAX_SPEED);
}

bool handleColorDetection() {
  if (colorScenario == NO_SCENARIO) return false;

  int redFreq = readColorFrequency(LOW, LOW);
  int blueFreq = readColorFrequency(LOW, HIGH);
  int greenFreq = readColorFrequency(HIGH, HIGH);

  if (blueFreq < redFreq && blueFreq < greenFreq) {
    turnRight(TURN_SPEED);
    turnStartTime = millis();
    isTurningFromColor = true;
    currentState = TURNING;
    return true;
  }
  else if (greenFreq < redFreq && greenFreq < blueFreq) {
    turnLeft(TURN_SPEED);
    turnStartTime = millis();
    isTurningFromColor = true;
    currentState = TURNING;
    return true;
  }
  else if (redFreq >10 && redFreq < 45 && greenFreq >80 && greenFreq < 130 && blueFreq <110 && blueFreq >60) {
    moveBackward(SLOWLY);
    reverseStartTime = millis();
    currentState = REVERSING;
    return true;
  }

  return false;
}


void distance1() {
  measureDistances();
  
  // First check for emergency stop conditions
  if (Distance_F < WALL_DISTANCE/2) {  // Immediate stop for very close obstacles
    smoothStop();
    currentState = COLOR_CHECK;
    colorScenario = FRONT_OBSTACLE;
    return;
  }

  // Scenario 1: Course correction while moving forward
  if (currentState == MOVING_FORWARD) {
    // First check if we need to stop for front obstacle
    if (Distance_F < WALL_DISTANCE) {
      smoothStop();
      currentState = COLOR_CHECK;
      colorScenario = FRONT_OBSTACLE;
      return;
    }
    
    // Then do course correction if needed
    int diff = Distance_R - Distance_L;
    if (abs(diff) > CORRECTION_THRESHOLD) {
      if (diff > 0) {
        gentleAdjust(LEFT);  // Right sensor farther, adjust left
      } else {
        gentleAdjust(RIGHT); // Left sensor farther, adjust right
      }
      return;
    }
  }

  // Check for color detection scenarios
  colorScenario = NO_SCENARIO;
  
  // Scenario 2: Front obstacle detected
  if (Distance_F < WALL_DISTANCE) {
    colorScenario = FRONT_OBSTACLE;
  }
  // Scenario 3: Right opening with left wall
  else if (Distance_R < WALL_DISTANCE && Distance_L > OPENING_THRESHOLD) {
    colorScenario = LEFT_OPENING;
  }
  // Scenario 3: Left opening with right wall
  else if (Distance_L < WALL_DISTANCE && Distance_R > OPENING_THRESHOLD) {
    colorScenario = RIGHT_OPENING;
  }

  // Handle state transitions
  switch(currentState) {
    case MOVING_FORWARD:
      if (colorScenario != NO_SCENARIO) {
        smoothStop();
        currentState = COLOR_CHECK;
      } else if (Distance_F < WALL_DISTANCE * 1.5) {
        moveForward(SLOWLY);
        currentState = SLOWING_DOWN;
      }
      break;

    case SLOWING_DOWN:
      if (Distance_F < WALL_DISTANCE) {
        smoothStop();
        colorScenario = FRONT_OBSTACLE;
        currentState = COLOR_CHECK;
      } else if (Distance_F > WALL_DISTANCE * 2) {
        moveForward(MAX_SPEED);
        currentState = MOVING_FORWARD;
      }
      break;

    case COLOR_CHECK:
      if (!handleColorDetection()) {
        currentState = STOPPED;
      }
      break;

    case STOPPED:
      // Default turn if no color detected
      if (Distance_R > Distance_L) {
        turnRight(TURN_SPEED);
      } else {
        turnLeft(TURN_SPEED);
      }
      currentState = TURNING;
      turnStartTime = millis();
      break;

    case TURNING:
      measureDistances();
      // Complete turn if from color command
      if (isTurningFromColor && millis() - turnStartTime >= TURN_90_DEGREE_TIME) {
        stopMotors();
        isTurningFromColor = false;
        currentState = MOVING_FORWARD;
      }
      // Normal turning until front is clear
      else if (Distance_F > OPENING_THRESHOLD) {
        stopMotors();
        currentState = MOVING_FORWARD;
      }
      // Timeout safety
      else if (millis() - turnStartTime > 1000) {
        stopMotors();
        currentState = STOPPED;
      }
      break;

    case REVERSING:
      if (millis() - reverseStartTime >= REVERSE_DURATION) {
        stopMotors();
        // Check if we've reached scenario 3 condition
        if ((Distance_R < WALL_DISTANCE && Distance_L > OPENING_THRESHOLD) ||
            (Distance_L < WALL_DISTANCE && Distance_R > OPENING_THRESHOLD)) {
          currentState = COLOR_CHECK;
        } else {
          // Continue reversing
          moveBackward(SLOWLY);
          reverseStartTime = millis();
        }
      }
      break;
  }
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