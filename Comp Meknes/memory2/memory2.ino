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


#define BASE_SPEED 80
#define SLOWLY 80
#define MAX_SPEED 120
#define TURN_SPEED 120
#define turnTime  300

#define TARGET_OFFSET 5      

// PID Constants - Tune these values for your robot
#define KP  8   // Proportional gain (reduced from original)
#define KI 0.01   // Integral gain
#define KD 0.3   // Derivative gain
#define WALL_DISTANCE 5  // Desired distance from wall (cm)

// Color sensor pins
int s0 = A0;
int s1 = A1;
int s2 = A2;
int s3 = A3;
int outPin = 6;

// Path memory system
#define MAX_MEMORY 30  // Maximum number of path decisions to remember
#define FORWARD 0
#define RIGHT 1
#define LEFT 2
#define BACK 3

// Simple path memory - stores decision history as a sequence of moves
// Uses minimal memory while still avoiding simple backtracking
byte pathMemory[MAX_MEMORY];  // Stores directions taken (FORWARD, RIGHT, LEFT, BACK)
byte pathCount = 0;           // Current number of moves in memory

// Junction memory (where robot made decisions)
#define MAX_JUNCTIONS 20
// Structure: [0]=estimated X, [1]=estimated Y, [2]=direction chosen 
byte junctions[MAX_JUNCTIONS][3];
byte junctionCount = 0;

// Simple position tracking (estimated)
int posX = 50;  // Start in middle of imaginary grid (0-100)
int posY = 50;
int facing = FORWARD;  // Current direction robot is facing

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

int i = 0;
int Distance_F, Distance_L, Distance_R;

// Records a junction point with its position and chosen direction
void recordJunction(int direction) {
  if (junctionCount < MAX_JUNCTIONS) {
    // Check if already recorded at this location
    bool found = false;
    for (int i = 0; i < junctionCount; i++) {
      if (abs(junctions[i][0] - posX) < 2 && abs(junctions[i][1] - posY) < 2) {
        // Update existing junction
        junctions[i][2] = direction;
        found = true;
        break;
      }
    }
    
    // Add new junction if not found
    if (!found) {
      junctions[junctionCount][0] = posX;
      junctions[junctionCount][1] = posY;
      junctions[junctionCount][2] = direction;
      junctionCount++;
    }
  }
}

// Check if we've been at this junction before and which way we went
int getJunctionHistory() {
  for (int i = 0; i < junctionCount; i++) {
    if (abs(junctions[i][0] - posX) < 2 && abs(junctions[i][1] - posY) < 2) {
      return junctions[i][2];  // Return the direction we chose last time
    }
  }
  return -1;  // No history found
}

// Updates position based on movement
void updatePosition(int direction) {
  // Store this move in memory
  if (pathCount < MAX_MEMORY) {
    pathMemory[pathCount++] = direction;
  } else {
    // Memory full, shift array to make room
    for (int i = 0; i < MAX_MEMORY - 1; i++) {
      pathMemory[i] = pathMemory[i + 1];
    }
    pathMemory[MAX_MEMORY - 1] = direction;
  }
  
  // Update position based on current direction and move
  switch (facing) {
    case FORWARD:
      if (direction == FORWARD) posY -= 2;
      else if (direction == RIGHT) posX += 2;
      else if (direction == LEFT) posX -= 2;
      else if (direction == BACK) posY += 2;
      break;
    case RIGHT:
      if (direction == FORWARD) posX += 2;
      else if (direction == RIGHT) posY += 2;
      else if (direction == LEFT) posY -= 2;
      else if (direction == BACK) posX -= 2;
      break;
    case LEFT:
      if (direction == FORWARD) posX -= 2;
      else if (direction == RIGHT) posY -= 2;
      else if (direction == LEFT) posY += 2;
      else if (direction == BACK) posX += 2;
      break;
    case BACK:
      if (direction == FORWARD) posY += 2;
      else if (direction == RIGHT) posX -= 2;
      else if (direction == LEFT) posX += 2;
      else if (direction == BACK) posY -= 2;
      break;
  }
  
  // Update facing direction
  facing = (facing + direction) % 4;
}

// Check if a path would lead to backtracking
bool isBacktrackPath(int direction) {
  // Calculate where this direction would lead
  int newX = posX;
  int newY = posY;
  int newFacing = (facing + direction) % 4;
  
  switch (newFacing) {
    case FORWARD: newY -= 2; break;
    case RIGHT: newX += 2; break;
    case LEFT: newX -= 2; break;
    case BACK: newY += 2; break;
  }
  
  // Check if this position is in our junction memory
  for (int i = 0; i < junctionCount; i++) {
    if (abs(junctions[i][0] - newX) < 2 && abs(junctions[i][1] - newY) < 2) {
      return true;  // This would be backtracking
    }
  }
  
  return false;
}

// Choose a direction that avoids previously visited areas
int chooseDirection() {
  bool canGoForward = (Distance_F > 20 || Distance_F == 0);
  bool canGoRight = (Distance_R > 20 || Distance_R == 0);
  bool canGoLeft = (Distance_L > 20 || Distance_L == 0);
  
  // Count available directions
  int options = 0;
  if (canGoForward) options++;
  if (canGoRight) options++;
  if (canGoLeft) options++;
  
  // If at a junction (multiple choices), record it
  if (options > 1) {
    // Check if we've been here before
    int prevDirection = getJunctionHistory();
    
    if (prevDirection >= 0) {
      // We've been here before, take a different direction
      if (prevDirection == FORWARD && canGoRight) return RIGHT;
      if (prevDirection == FORWARD && canGoLeft) return LEFT;
      if (prevDirection == RIGHT && canGoLeft) return LEFT;
      if (prevDirection == RIGHT && canGoForward) return FORWARD;
      if (prevDirection == LEFT && canGoForward) return FORWARD;
      if (prevDirection == LEFT && canGoRight) return RIGHT;
    }
    
    // If no history or all directions tried, prioritize unexplored paths
    if (canGoForward && !isBacktrackPath(FORWARD)) {
      recordJunction(FORWARD);
      return FORWARD;
    }
    if (canGoRight && !isBacktrackPath(RIGHT)) {
      recordJunction(RIGHT);
      return RIGHT;
    }
    if (canGoLeft && !isBacktrackPath(LEFT)) {
      recordJunction(LEFT);
      return LEFT;
    }
    
    // All options are previously visited, pick one (right, forward, left in priority)
    if (canGoRight) {
      recordJunction(RIGHT);
      return RIGHT;
    }
    if (canGoForward) {
      recordJunction(FORWARD);
      return FORWARD;
    }
    if (canGoLeft) {
      recordJunction(LEFT);
      return LEFT;
    }
  }
  
  // If only one option, take it
  if (canGoForward) return FORWARD;
  if (canGoRight) return RIGHT;
  if (canGoLeft) return LEFT;
  
  // No obvious option, turn around
  return BACK;
}

int readColorFrequency(int s2Val, int s3Val) {
  digitalWrite(s2, s2Val);
  digitalWrite(s3, s3Val);
  delay(100);
  return pulseIn(outPin, LOW);
}

void loop() {
  colorWithUS();
}

void colorWithUS() {
  // Read Color
  int redFreq = readColorFrequency(LOW, LOW);     // Rouge
  int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
  int greenFreq = readColorFrequency(HIGH, HIGH); // Vert
  
  // Read all sensors
  Distance_F = sonarFront.ping_cm();
  Distance_L = sonarLeft.ping_cm();
  Distance_R = sonarRight.ping_cm();

  // Color detection logic (preserved from original)
  if (redFreq > 35 && redFreq < 60 && greenFreq > 120 && greenFreq < 165 && blueFreq < 140 && blueFreq > 90) {
    turnRight90();
    updatePosition(RIGHT);
  } else if (greenFreq < redFreq && greenFreq < blueFreq) {
    turnLeft90();
    updatePosition(LEFT);
  } else if (blueFreq < redFreq && blueFreq < greenFreq) {
    int lastRight = sonarRight.ping_cm();
    int lastLeft = sonarLeft.ping_cm();
    do {
      moveBackward(SLOWLY);
      delay(10);
      Distance_L = sonarLeft.ping_cm();
      Distance_R = sonarRight.ping_cm();
    } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));
    delay(100);
    stopMotors();
    
    // Memory-based decision instead of simple left/right comparison
    int direction = chooseDirection();
    
    if (direction == RIGHT) {
      turnRight90();
      updatePosition(RIGHT);
    } else {
      turnLeft90();
      updatePosition(LEFT);
    }
  }
  // Case 1: Immediate obstacle in front (<20cm)
  else if (Distance_F < 20 && Distance_F != 0) {
    if (i == 1) {
      smoothStop();
    } else {
      stopMotors();
    }
    delay(100);

    // Re-read color for confirmation
    redFreq = readColorFrequency(LOW, LOW);
    blueFreq = readColorFrequency(LOW, HIGH);
    greenFreq = readColorFrequency(HIGH, HIGH);
    
    // Color-based decision (preserved from original)
    if (redFreq > 35 && redFreq < 60 && greenFreq > 120 && greenFreq < 165 && blueFreq < 140 && blueFreq > 90) {
      turnRight90();
      updatePosition(RIGHT);
    } else if (greenFreq < redFreq && greenFreq < blueFreq) {
      turnLeft90();
      updatePosition(LEFT);
    } else if (blueFreq < redFreq && blueFreq < greenFreq) {
      int lastRight = sonarRight.ping_cm();
      int lastLeft = sonarLeft.ping_cm();
      do {
        moveBackward(SLOWLY);
        delay(10);
        Distance_L = sonarLeft.ping_cm();
        Distance_R = sonarRight.ping_cm();
      } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));
      delay(100);
      stopMotors();
      
      // Memory-based decision
      int direction = chooseDirection();
      
      if (direction == RIGHT) {
        turnRight90();
        updatePosition(RIGHT);
      } else {
        turnLeft90();
        updatePosition(LEFT);
      }
    } else if (Distance_F < 20 && Distance_L < 12 && Distance_R < 12) {
      // Dead end - back up and turn based on memory
      int lastRight = sonarRight.ping_cm();
      int lastLeft = sonarLeft.ping_cm();
      do {
        moveBackward(SLOWLY);
        delay(10);
        Distance_L = sonarLeft.ping_cm();
        Distance_R = sonarRight.ping_cm();
      } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));
      delay(100);
      stopMotors();
      
      // Check memory for the best direction
      int direction = chooseDirection();
      
      if (direction == RIGHT) {
        turnRight90();
        updatePosition(RIGHT);
      } else {
        turnLeft90();
        updatePosition(LEFT);
      }
    } else {
      // Memory-based decision at a normal junction
      int direction = chooseDirection();
      
      if (direction == RIGHT) {
        turnRight90();
        updatePosition(RIGHT);
      } else if (direction == LEFT) {
        turnLeft90();
        updatePosition(LEFT);
      } else if (direction == BACK) {
        // Turn around (two right turns)
        turnRight90();
        delay(100);
        turnRight90();
        updatePosition(BACK);
      } else {
        // Default case - shouldn't happen at an obstacle
        if (Distance_R > Distance_L) {
          turnRight90();
          updatePosition(RIGHT);
        } else {
          turnLeft90();
          updatePosition(LEFT);
        }
      }
    }
  }
  // Case 2: Approaching obstacle (20-70cm)
  else if (Distance_F < 70 && Distance_F != 0) {
    if (i == 1) {
      smoothStop();
      delay(200);
    }
    i = 0;
    moveForward(SLOWLY);
  } 
  // Case 3: Clear path ahead
  else {
    moveForward(MAX_SPEED);
    i = 1;
    
    // Occasionally update position when moving forward
    static int forwardCount = 0;
    if (++forwardCount > 50) {  // Arbitrary counter to avoid too frequent updates
      forwardCount = 0;
      updatePosition(FORWARD);
    }
  }
  
  delay(20);
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
  analogWrite(M_LEFT_ENA, TURN_SPEED);
  analogWrite(M_RIGHT_ENB, TURN_SPEED);
  delay(turnTime);
  stopMotors();
}

void turnLeft90() {
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
  analogWrite(M_LEFT_ENA, TURN_SPEED);
  analogWrite(M_RIGHT_ENB, TURN_SPEED);
  delay(turnTime);
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
  float error = (leftDist - rightDist);
  
  // PID terms
  float P = KP * error;
  
  integral += error;
  integral = constrain(integral, -100, 100);
  float I = KI * integral;
  
  float derivative = error - lastError;
  float D = KD * derivative;
  
  // Store values for next iteration
  lastError = error;
  lastLeftDist = leftDist;
  lastRightDist = rightDist;
  
  // Total correction
  float correction = P + I + D;
  correction = constrain(correction, -speed/2, speed/2);
  
  return correction;
}

float filterDistance(float currentDistance) {
  static float filteredDistances[3] = {0};
  static byte index = 0;
  
  if (currentDistance == 0) {
    return filteredDistances[(index-1+3)%3];
  }
  
  filteredDistances[index] = currentDistance;
  index = (index + 1) % 3;
  
  return (filteredDistances[0] + filteredDistances[1] + filteredDistances[2]) / 3.0;
}

void moveForward(int speed) {
  float correction = calculatePID(speed);
  
  int leftSpeed = constrain(speed - correction, speed/2, speed*1.5);
  int rightSpeed = constrain(speed + correction, speed/2, speed*1.5);
  
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void moveBackward(int speed) {
  float correction = calculatePID(speed);
  
  int leftSpeed = constrain(speed - correction, speed/2, speed*1.5);
  int rightSpeed = constrain(speed + correction, speed/2, speed*1.5);
  
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void smoothStop() {
  int currentSpeed = MAX_SPEED;
  Distance_F = sonarFront.ping_cm();
  
  if (Distance_F <= 50) {
    while (Distance_F > 20 && currentSpeed > 0) {
      currentSpeed = map(Distance_F, 20, 50, 0, MAX_SPEED);
      currentSpeed = constrain(currentSpeed, 0, MAX_SPEED);
      
      analogWrite(M_LEFT_ENA, currentSpeed);
      analogWrite(M_RIGHT_ENB, currentSpeed);
      digitalWrite(M_LEFT_IN1, HIGH);
      digitalWrite(M_LEFT_IN2, LOW);
      digitalWrite(M_RIGHT_IN3, HIGH);
      digitalWrite(M_RIGHT_IN4, LOW);
      
      Distance_F = sonarFront.ping_cm();
      delay(50);
    }
    
    stopMotors();
  }
}