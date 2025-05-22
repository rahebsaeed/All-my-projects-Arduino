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
#define TURN_SPEED 140
#define turnTime  300
#define MAX_CORRECTION 100 

#define TARGET_OFFSET 10      

// PID Constants - Tune these values for your robot
#define KP  8   // Proportional gain (reduced from original)
#define KI 0.01   // Integral gain
#define KD 0.3   // Derivative gain
#define WALL_DISTANCE 5  // Desired distance from wall (cm)

int s0 = A0;
int s1 = A1;
int s2 = A2;
int s3 = A3;
int outPin = 6;

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

  Serial.begin(9600);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(outPin, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
}
int i =0, j=1;
int Distance_F, Distance_L, Distance_R;

int readColorFrequency(int s2Val, int s3Val) {
  digitalWrite(s2, s2Val);
  digitalWrite(s3, s3Val);
  delay(100);
  return pulseIn(outPin, LOW);
}

void loop() {
colorWithUS();
}

void colorWithUS(){
  // Read Color
  int redFreq = readColorFrequency(LOW, LOW);     // Rouge
  int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
  int greenFreq = readColorFrequency(HIGH, HIGH); // Vert
    // Read all sensors
  Distance_F = sonarFront.ping_cm();
  Distance_L = sonarLeft.ping_cm();
  Distance_R = sonarRight.ping_cm();

//color Conditions
  if(greenFreq < redFreq && greenFreq < blueFreq){
    if(i == 1){
    breakMotors();
    }
    delay(100);
    turnRight90();
    moveForward(MAX_SPEED);
    delay(200);
    
  }else if(redFreq >35 && redFreq < 60 && greenFreq >120 && greenFreq < 165 && blueFreq <140 && blueFreq >90){
    if(i == 1){
    breakMotors();
    }    
    delay(100);
    turnLeft90();
    moveForward(MAX_SPEED);
    delay(200);
  }else if(blueFreq < redFreq && blueFreq < greenFreq){
    int lastRight = sonarRight.ping_cm();
    int lastLeft = sonarLeft.ping_cm();
    do {
      moveBackward(SLOWLY);
      delay(10);
      Distance_L = sonarLeft.ping_cm();
      Distance_R = sonarRight.ping_cm();
    } while ((Distance_L + Distance_R) < (lastLeft + lastRight +3) && Distance_F != 0 && Distance_R != 0 && Distance_L != 0);  // Back up until 15cm clearance
    delay(100);
    stopMotors();
    if (Distance_R > Distance_L ) {
      turnRight90();
    } else {
      turnLeft90();
    }
  }  // Case 1: Immediate obstacle in front (<20cm)
  else

// distances Conditions
   if (Distance_F < 30 && Distance_F != 0) {
    if(i == 1){
    breakMotors();
    }
    if(Distance_F > 10 && Distance_F != 0){
      do {
        moveForward(SLOWLY);
        delay(10);
        Distance_F = sonarFront.ping_cm();
      } while (Distance_F > 10 && Distance_F != 0);  // Back up until 15cm clearance
      stopMotors();
      delay(100);  // Brief pause for stabilization
      if(j == 1){
        int right = sonarRight.ping_cm();
        int left = sonarLeft.ping_cm();

        do {
          moveBackward(SLOWLY);
          delay(10);
          Distance_F = sonarFront.ping_cm();
        } while (Distance_F > 40 && Distance_F != 0);  // Back up until 15cm clearance
        stopMotors();
        delay(100);  // Brief pause for stabilization
        if(left > right){
          turnLeft10();
        }else{
          turnRight10();
        }
        j=0;
        do {
          moveForward(SLOWLY);
          delay(10);
          Distance_F = sonarFront.ping_cm();
        } while (Distance_F > 5 && Distance_F != 0);  // Back up until 15cm clearance
        breakMotors();
        delay(100);  // Brief pause for stabilization
      }
    }
    int redFreq = readColorFrequency(LOW, LOW);     // Rouge
    int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
    int greenFreq = readColorFrequency(HIGH, HIGH); // Vert
    if(redFreq >35 && redFreq < 60 && greenFreq >120 && greenFreq < 165 && blueFreq <140 && blueFreq >90 ){
      turnRight90();
    }else if(greenFreq < redFreq && greenFreq < blueFreq){
      turnLeft90();
    }else if(blueFreq < redFreq && blueFreq < greenFreq){
      int lastRight = sonarRight.ping_cm();
      int lastLeft = sonarLeft.ping_cm();
      do {
        moveBackward(SLOWLY);
        delay(10);
        Distance_L = sonarLeft.ping_cm();
        Distance_R = sonarRight.ping_cm();
      } while ((Distance_L + Distance_R) < (lastLeft + lastRight +3));  // Back up until 15cm clearance
      delay(100);
      stopMotors();
      if (Distance_R > Distance_L) {
        turnRight90();
      } else {
        turnLeft90();
      }
    }else if(Distance_F < 20 && Distance_F != 0 && Distance_L < 12 && Distance_L != 0 && Distance_R < 12 && Distance_R != 0){
      int lastRight = sonarRight.ping_cm();
      int lastLeft = sonarLeft.ping_cm();
      do {
        moveBackward(SLOWLY);
        delay(10);
        Distance_L = sonarLeft.ping_cm();
        Distance_R = sonarRight.ping_cm();
      } while ((Distance_L + Distance_R) < (lastLeft + lastRight +3));  // Back up until 15cm clearance
      delay(100);
      stopMotors();
      if (Distance_R > Distance_L) {
        turnRight90();
      } else {
        turnLeft90();
      }
    }else{
      if (Distance_R > Distance_L) {
        turnRight90();
      } else {
        turnLeft90();
      }
    }

  }
  // Case 2: Approaching obstacle (20-70cm)
  else if (Distance_F < 40 && Distance_F != 0) {
    if (i == 1) {  // If we were moving fast
      smoothStop();
      delay(100);  // Added missing semicolon
    }
    i = 0;
    moveForward(SLOWLY);  // Proceed with caution
  } 
  // else if(Distance_F != 0){
  //   moveBackward(SLOWLY);
  //   delay(100);
  //   stopMotors();
  // }
  // Case 3: Clear path ahead
  else {
    moveForward(MAX_SPEED);
    i = 1;  // Flag that we're moving fast
  }
  
  delay(20);  // Small delay between readings
    
 
}
void final(){
  int redFreq = readColorFrequency(LOW, LOW);     // Rouge
  int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
  int greenFreq = readColorFrequency(HIGH, HIGH); // Vert
  Distance_F = sonarFront.ping_cm();
  Distance_L = sonarLeft.ping_cm();
  Distance_R = sonarRight.ping_cm();
  if(greenFreq < redFreq && greenFreq < blueFreq){
    if(i == 1){
    breakMotors();
    }
    delay(100);
    turnRight90();
    moveForward(MAX_SPEED);
    delay(200);
    
  }else if(redFreq >35 && redFreq < 60 && greenFreq >120 && greenFreq < 165 && blueFreq <140 && blueFreq >90){
    if(i == 1){
    breakMotors();
    }    
    delay(100);
    turnLeft90();
    moveForward(MAX_SPEED);
    delay(200);
  }else if(blueFreq < redFreq && blueFreq < greenFreq){
    int lastRight = sonarRight.ping_cm();
    int lastLeft = sonarLeft.ping_cm();
    do {
      moveBackward(SLOWLY);
      delay(10);
      Distance_L = sonarLeft.ping_cm();
      Distance_R = sonarRight.ping_cm();
    } while ((Distance_L + Distance_R) < (lastLeft + lastRight +3) && Distance_F != 0 && Distance_R != 0 && Distance_L != 0);  // Back up until 15cm clearance
    delay(100);
    stopMotors();
    if (Distance_R > Distance_L ) {
      turnRight90();
    } else {
      turnLeft90();
    }
  }  // Case 1: Immediate obstacle in front (<20cm)
  else if (Distance_F < 20) {
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
  j=1;
}
void turnRight10() {
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
  analogWrite(M_LEFT_ENA, TURN_SPEED);    // Motor speed
  analogWrite(M_RIGHT_ENB, TURN_SPEED);
  delay(turnTime/4);          // Time for 90-degree turn
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
  j=1;
}

void turnLeft10() {
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
  analogWrite(M_LEFT_ENA, TURN_SPEED);    // Motor speed
  analogWrite(M_RIGHT_ENB, TURN_SPEED);
  delay(turnTime/4);          // Time for 90-degree turn
  stopMotors();
}

float calculatePID(int speed) {
  static float lastLeftDist = WALL_DISTANCE;
  static float lastRightDist = WALL_DISTANCE;
  static float integral = 0;
  static float lastError = 0;
  
  // Get filtered distances
  int leftDist = filterDistance(sonarLeft.ping_cm());
  int rightDist = filterDistance(sonarRight.ping_cm());
  
  // Calculate error (how far from desired position)
  float error = (leftDist - rightDist);  // Simplified error calculation
  
  // PID terms
  float P = KP * error;
  
  integral += error;
  integral = constrain(integral, -100, 100); // Prevent windup
  float I = KI * integral;
  
  float derivative = error - lastError;
  float D = KD * derivative;
  
  // Store values for next iteration
  lastError = error;
  lastLeftDist = leftDist;
  lastRightDist = rightDist;
  
  // Total correction
  float correction = P + I + D;
  
  // Limit correction to prevent extreme adjustments
  correction = constrain(correction, -speed/2, speed/2);
  
  return correction;
}

// Simple moving average filter for distance readings
float filterDistance(float currentDistance) {
  static float filteredDistances[3] = {0};
  static byte index = 0;
  
  // Ignore zero readings (common with ultrasonic sensors)
  if(currentDistance == 0) {
    return filteredDistances[(index-1)%3]; // Return last valid reading
  }
  
  filteredDistances[index] = currentDistance;
  index = (index + 1) % 3;
  
  // Return simple moving average
  return (filteredDistances[0] + filteredDistances[1] + filteredDistances[2]) / 3.0;
}

void moveForward1(int speed) {
  float correction = calculatePID(speed);
  
  // Calculate motor speeds with more constrained limits
  int leftSpeed = constrain(speed - correction, speed/2, speed*1.5);
  int rightSpeed = constrain(speed + correction, speed/2, speed*1.5);
  
  // Apply speeds
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void moveForward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void moveBackward(int speed) {
  // Calculate correction (same as forward movement)
  float correction = calculatePID(speed);
  
  // Calculate motor speeds (same logic as forward)
  int leftSpeed = constrain(speed - correction, speed/2, speed*1.5);
  int rightSpeed = constrain(speed + correction, speed/2, speed*1.5);
  
  // Apply motor control IN REVERSE DIRECTION
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, LOW);    // Reverse polarity for backward movement
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}




void smoothStop() {
  int currentSpeed = MAX_SPEED;
  Distance_F = sonarFront.ping_cm(); // Get current distance
  
  // Only start braking if we're within a certain range (e.g., 50cm)
  if(Distance_F <= 50 && Distance_F != 0) {
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

