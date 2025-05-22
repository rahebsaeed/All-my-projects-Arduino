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
#define SLOWLY 70
#define MAX_SPEED 100
#define TURN_SPEED 100
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

int readColorFrequency(int s2Val, int s3Val) {
  digitalWrite(s2, s2Val);
  digitalWrite(s3, s3Val);
  delay(100);
  return pulseIn(outPin, LOW);
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
void distance() {
  measureDistances();

  if (Distance_F < 10) {
    if (i == 1) {
      smoothStop();
    }
    stopMotors();
    delay(90);
    
    // Only turn if there's a wall in front
      if (Distance_R > Distance_L) {
        turnRight90();
      } else {
        turnLeft90();
      }
  } 
  else if (Distance_F < 50) {
    if (i == 1) {
      smoothStop();
    }
    i = 0;
    moveForward(SLOWLY);
  } 
  else {
    moveForward(MAX_SPEED);
    i = 1;
  }
}

// Obstacle avoidance states
enum AvoidanceState {
  MOVING_FORWARD,
  SLOWING_DOWN,
  STOPPED,
  TURNING,
  COLORCHECK
};
AvoidanceState currentState = MOVING_FORWARD;

void distance1() {
  measureDistances(); // Always get fresh measurements
  
  switch(currentState) {
    case MOVING_FORWARD:
      if (Distance_F < WALL_DISTANCE * 2) {
        smoothStop();
        currentState = COLORCHECK;
      } 
      else if (Distance_F < WALL_DISTANCE * 3) {
        moveForward(SLOWLY);
        currentState = SLOWING_DOWN;
      }else if(Distance_R > WALL_DISTANCE || Distance_L > WALL_DISTANCE){
        stopMotors();
        delay(100);
        currentState = STOPPED;
      }
      else {
        moveForward(MAX_SPEED);
      }
      break;
      
    case SLOWING_DOWN:
      if (Distance_F < WALL_DISTANCE * 1.5) {
        smoothStop();
        currentState = STOPPED;
      }
      else if (Distance_F > WALL_DISTANCE * 3) {
        moveForward(MAX_SPEED);
        currentState = MOVING_FORWARD;
      }
      break;
      
    case STOPPED:
      // Decide which way to turn
      if (Distance_R > Distance_L) {
        turnRight(TURN_SPEED);
      } else {
        turnLeft(TURN_SPEED);
      }
      currentState = TURNING;
      break;
      
    case TURNING:
      measureDistances();
      // Keep turning until we have clearance
      if (Distance_F > WALL_DISTANCE * 2 && Distance_R > 10 && Distance_L > 10) {
        stopMotors();
        delay(100); // Brief pause after turn
        currentState = MOVING_FORWARD;
      }
      // Emergency backup if stuck
      else if (Distance_F < WALL_DISTANCE && (Distance_R < 10 || Distance_L < 10)) {
        moveBackward(SLOWLY);
        delay(300);
        stopMotors();
      }
      break;
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