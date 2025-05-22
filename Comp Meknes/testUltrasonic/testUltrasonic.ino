#include <NewPing.h>
#define TRIG_F 7
#define ECHO_F 8
#define TRIG_R 11
#define ECHO_R 12
#define TRIG_L 13
#define ECHO_L A4

// Motor Driver Pins (L298N)
#define M_LEFT_IN1  2
#define M_LEFT_IN2  3
#define M_RIGHT_IN3 4
#define M_RIGHT_IN4 5
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10


#define BASE_SPEED 60
#define SLOWLY 60
#define MAX_SPEED 80
#define TURN_SPEED 120
#define turnTime  300
#define MAX_CORRECTION 100  // Maximum correction value (adjust as needed)
int currentSpeed;
#define CENTER_TOLERANCE 1.0  

// PID Constants - More aggressive tuning
float Kp = 2.0;   // More responsive to immediate error
float Ki = 0.05;  // Small integral to prevent drift
float Kd = 0.8;   // Higher derivative to dampen oscillations

float error = 0, lastError = 0;
float P = 0, I = 0, D = 0;
float PIDvalue = 0;

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
#define INF_F A5 

NewPing sonarFront(TRIG_F, ECHO_F, 200);
NewPing sonarRight(TRIG_R, ECHO_R, 200);
NewPing sonarLeft(TRIG_L, ECHO_L, 200);

void setup() {
    // Motor control pins
  pinMode(M_LEFT_IN1, OUTPUT);
  pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT);
  pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT);
  pinMode(M_RIGHT_ENB, OUTPUT);
  pinMode(INF_F, INPUT);

  Serial.begin(9600);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(outPin, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
}
int i =0;
int Distance_F, Distance_L, Distance_R;

int readColorFrequency(int s2Val, int s3Val) {
  digitalWrite(s2, s2Val);
  digitalWrite(s3, s3Val);
  delay(100);
  return pulseIn(outPin, LOW);
}

void loop() {
distance();
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

  Serial.print("R: ");
  Serial.print(redFreq);
  Serial.print("  G: ");
  Serial.print(greenFreq);
  Serial.print("  B: ");
  Serial.print(blueFreq);
  Serial.print("  → ");
  Serial.println(colorName);

  delay(20);
}
void distance() {
  Distance_F = sonarFront.ping_cm();
  Distance_L = sonarLeft.ping_cm();
  Distance_R = sonarRight.ping_cm();

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
  else if (Distance_F < 30) {
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

float calculatePID(int currentSpeed) {
  
  if (millis() - lastSensorRead < SENSOR_SAMPLE_INTERVAL) {
    return PIDvalue;
  }
  lastSensorRead = millis();

  int leftDistance = sonarLeft.ping_cm();
  int rightDistance = sonarRight.ping_cm();
  
  // Only calculate error if we detect walls on at least one side
  if (leftDistance < WALL_DETECTION_THRESHOLD || rightDistance < WALL_DETECTION_THRESHOLD) {
    // Calculate normalized error (-1.0 to 1.0 range)
    error = (leftDistance - rightDistance) / (2.0 * TARGET_OFFSET);
    error = constrain(error, -1.0, 1.0);
    
    // Apply deadband (reduced to 0.5cm equivalent)
    if (abs(leftDistance - rightDistance) > 0.5) {
      P = error;
      I = I + (error * 0.1);  // Reduced integral effect
      D = (error - lastError) * 10;  // More responsive derivative
      lastError = error;
      
      // Calculate PID with speed-adaptive scaling
      PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
      
      // Scale correction based on current speed
      float speedFactor = map(currentSpeed, BASE_SPEED, MAX_SPEED, 0.5, 1.5);
      PIDvalue *= speedFactor * MAX_CORRECTION;
      
      PIDvalue = constrain(PIDvalue, -MAX_CORRECTION, MAX_CORRECTION);
    } else {
      PIDvalue = 0;
    }
  } else {
    // No walls detected - reset PID to prevent integral windup
    resetPID();
    PIDvalue = 0;
  }
  
  return PIDvalue;
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

void moveForward2(int speed) {
  float correction = calculatePID(speed);
  
  // More aggressive correction application
  int leftSpeed = speed - correction;
  int rightSpeed = speed + correction;
  
  // Dynamic speed constraints
  leftSpeed = constrain(leftSpeed, 30, 255);
  rightSpeed = constrain(rightSpeed, 30, 255);
  
  // Ensure minimum speed to keep moving
  leftSpeed = max(leftSpeed, 30);
  rightSpeed = max(rightSpeed, 30);
  
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
  

}
void moveForward1(int speed) {
  currentSpeed = constrain(speed, BASE_SPEED, MAX_SPEED);
  float correction = calculatePID(currentSpeed);
  
  // Apply speed-constrained correction
  int leftSpeed = currentSpeed - correction;
  int rightSpeed = currentSpeed + correction;
  
  // Dynamic speed constraints with better handling
  leftSpeed = constrain(leftSpeed, BASE_SPEED-20, MAX_SPEED); // Allow some under-speed
  rightSpeed = constrain(rightSpeed, BASE_SPEED-20, MAX_SPEED);
  
  // Ensure minimum effective speed
  leftSpeed = max(leftSpeed, BASE_SPEED-20);
  rightSpeed = max(rightSpeed, BASE_SPEED-20);
  
  // Apply motor control
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);


}

void centerRobotBeforeTurn() {
  int leftDist = sonarLeft.ping_cm();
  int rightDist = sonarRight.ping_cm();
  
  // Only attempt centering if BOTH walls are detected
  if (leftDist < WALL_DETECTION_THRESHOLD && rightDist < WALL_DETECTION_THRESHOLD) {
    float error = leftDist - rightDist;
    
    // Adjust position until centered (within tolerance)
    while (abs(error) > CENTER_TOLERANCE) {
      if (error > 0) { // Too close to left wall
        // Small right adjustment
        analogWrite(M_LEFT_ENA, 50);
        analogWrite(M_RIGHT_ENB, 50);
        digitalWrite(M_LEFT_IN1, LOW);
        digitalWrite(M_LEFT_IN2, HIGH);  // Left motor reverse
        digitalWrite(M_RIGHT_IN3, HIGH); // Right motor forward
        digitalWrite(M_RIGHT_IN4, LOW);
      } 
      else { // Too close to right wall
        // Small left adjustment
        analogWrite(M_LEFT_ENA, 50);
        analogWrite(M_RIGHT_ENB, 50);
        digitalWrite(M_LEFT_IN1, HIGH);  // Left motor forward
        digitalWrite(M_LEFT_IN2, LOW);
        digitalWrite(M_RIGHT_IN3, LOW);  // Right motor reverse
        digitalWrite(M_RIGHT_IN4, HIGH);
      }
      
      delay(50);  // Small adjustment time
      stopMotors();
      
      // Re-check distances
      leftDist = sonarLeft.ping_cm();
      rightDist = sonarRight.ping_cm();
      error = leftDist - rightDist;
    }
  }
  stopMotors();
  delay(100);  // Stabilize before turning
}

void turn180() {
  int leftDist = sonarLeft.ping_cm();
  int rightDist = sonarRight.ping_cm();

  // Only center if both walls detected
  if (leftDist < WALL_DETECTION_THRESHOLD && rightDist < WALL_DETECTION_THRESHOLD) {
    centerRobotBeforeTurn();
  }

  // Perform 180° turn
  analogWrite(M_LEFT_ENA, TURN_SPEED);
  analogWrite(M_RIGHT_ENB, TURN_SPEED);
  
  // Left motors backward, right motors forward (clockwise turn)
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
  
  delay(turnTime * 2);  // 180° turn (adjust as needed)
  stopMotors();
}

void moveBackward(int speed) {
  float correction = calculatePID(speed);
  
  // Apply correction to motor speeds (inverted for backward)
  int leftSpeed = speed + correction;
  int rightSpeed = speed - correction;
  
  // Constrain speeds to valid range
  leftSpeed = constrain(leftSpeed, 0, MAX_SPEED);
  rightSpeed = constrain(rightSpeed, 0, MAX_SPEED);
  
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void resetPID() {
  error = 0;
  lastError = 0;
  P = 0;
  I = 0;
  D = 0;
  PIDvalue = 0;
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
  Distance_F = sonarFront.ping_cm(); // Get current distance
  
  // Only start braking if we're within a certain range (e.g., 50cm)
  if(Distance_F <= 50) {
    while(Distance_F > 20 && currentSpeed > 0) {
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
      Distance_F = sonarFront.ping_cm();
      delay(50); // Small delay to prevent sensor flooding
    }
    
    // Final stop
    stopMotors();
  }
}