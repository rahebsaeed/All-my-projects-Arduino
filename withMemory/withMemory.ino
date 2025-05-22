#include <NewPing.h>
#include <EEPROM.h> // Include EEPROM library

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

int s0 = A0;
int s1 = A1;
int s2 = A2;
int s3 = A3;
int outPin = 6;

NewPing sonarFront(TRIG_F, ECHO_F, 200);
NewPing sonarRight(TRIG_R, ECHO_R, 200);
NewPing sonarLeft(TRIG_L, ECHO_L, 200);

// Maze Mapping Variables
const int MAX_MAZE_SIZE = 32; // Maximum number of locations to remember - adjust based on testing and available EEPROM space.  Must be power of 2!
byte mazeMemory[MAX_MAZE_SIZE];   // Stores the path - each bit represents a direction
int currentPosition = 0;      // Current position in the maze memory array
int memoryIndex = 0;       // Index for writing to EEPROM

// Direction Definitions (bits in mazeMemory)
#define DIRECTION_FORWARD 0x01 // 00000001
#define DIRECTION_RIGHT   0x02 // 00000010
#define DIRECTION_LEFT    0x04 // 00000100
#define DIRECTION_BACKWARD 0x08 // 00001000


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

  // Initialize mazeMemory array (optional, but good practice)
  for (int i = 0; i < MAX_MAZE_SIZE; i++) {
    mazeMemory[i] = 0;
  }

  // Load maze data from EEPROM (uncomment if you want to load a previous maze)
  //loadMazeFromEEPROM();
}

int i = 0;
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

void colorWithUS() {
  // Read Color
  int redFreq = readColorFrequency(LOW, LOW);     // Rouge
  int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
  int greenFreq = readColorFrequency(HIGH, HIGH); // Vert
  // Read all sensors
  Distance_F = sonarFront.ping_cm();
  Distance_L = sonarLeft.ping_cm();
  Distance_R = sonarRight.ping_cm();

  if (redFreq > 35 && redFreq < 60 && greenFreq > 120 && greenFreq < 165 && blueFreq < 140 && blueFreq > 90 ) {
    turnRight90();
    recordMove(DIRECTION_RIGHT);
  } else if (greenFreq < redFreq && greenFreq < blueFreq) {
    // Check if turning left has been tried before
    if (!hasDirectionBeenTried(currentPosition, DIRECTION_LEFT)) {
      turnLeft90();
      recordMove(DIRECTION_LEFT);
    } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_RIGHT)) {
      turnRight90();
      recordMove(DIRECTION_RIGHT);
    } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_BACKWARD)) {
      // Implement backing up logic and direction recording here
      int lastRight = sonarRight.ping_cm();
      int lastLeft = sonarLeft.ping_cm();
      do {
        moveBackward(SLOWLY);
        delay(10);
        Distance_L = sonarLeft.ping_cm();
        Distance_R = sonarRight.ping_cm();
      } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));  // Back up until 15cm clearance
      delay(100);
      stopMotors();
      recordMove(DIRECTION_BACKWARD); // Record move before turning.
      turnRight90();
      recordMove(DIRECTION_RIGHT);

    } else {
      // Nowhere to go!  Handle the dead end (turn around, etc.)
      turnRight90();  // Simple turnaround.
      turnRight90();
      recordMove(DIRECTION_RIGHT);
      recordMove(DIRECTION_RIGHT);

    }
  } else if (blueFreq < redFreq && blueFreq < greenFreq) {
    // Check if turning left has been tried before
    if (!hasDirectionBeenTried(currentPosition, DIRECTION_RIGHT)) {
      turnRight90();
      recordMove(DIRECTION_RIGHT);
    } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_LEFT)) {
      turnLeft90();
      recordMove(DIRECTION_LEFT);
    } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_BACKWARD)) {
      // Implement backing up logic and direction recording here
      int lastRight = sonarRight.ping_cm();
      int lastLeft = sonarLeft.ping_cm();
      do {
        moveBackward(SLOWLY);
        delay(10);
        Distance_L = sonarLeft.ping_cm();
        Distance_R = sonarRight.ping_cm();
      } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));  // Back up until 15cm clearance
      delay(100);
      stopMotors();
      recordMove(DIRECTION_BACKWARD);
      turnRight90();
      recordMove(DIRECTION_RIGHT);
    } else {
      // Nowhere to go!  Handle the dead end (turn around, etc.)
      turnRight90();  // Simple turnaround.
      turnRight90();
      recordMove(DIRECTION_RIGHT);
      recordMove(DIRECTION_RIGHT);
    }

  }
  // Case 1: Immediate obstacle in front (<20cm)
  else if (Distance_F < 20 && Distance_F != 0) {
    if (i == 1) {
      smoothStop();  // Gradual stop if we were moving fast
    } else {
      stopMotors();  // Immediate stop if moving slowly
    }
    delay(100);  // Brief pause for stabilization

    int redFreq = readColorFrequency(LOW, LOW);     // Rouge
    int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
    int greenFreq = readColorFrequency(HIGH, HIGH); // Vert
    if (redFreq > 35 && redFreq < 60 && greenFreq > 120 && greenFreq < 165 && blueFreq < 140 && blueFreq > 90 ) {
      turnRight90();
      recordMove(DIRECTION_RIGHT);
    } else if (greenFreq < redFreq && greenFreq < blueFreq) {
      // Check if turning left has been tried before
      if (!hasDirectionBeenTried(currentPosition, DIRECTION_LEFT)) {
        turnLeft90();
        recordMove(DIRECTION_LEFT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_RIGHT)) {
        turnRight90();
        recordMove(DIRECTION_RIGHT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_BACKWARD)) {
        // Implement backing up logic and direction recording here
        int lastRight = sonarRight.ping_cm();
        int lastLeft = sonarLeft.ping_cm();
        do {
          moveBackward(SLOWLY);
          delay(10);
          Distance_L = sonarLeft.ping_cm();
          Distance_R = sonarRight.ping_cm();
        } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));  // Back up until 15cm clearance
        delay(100);
        stopMotors();
        recordMove(DIRECTION_BACKWARD); // Record move before turning.
        turnRight90();
        recordMove(DIRECTION_RIGHT);

      } else {
        // Nowhere to go!  Handle the dead end (turn around, etc.)
        turnRight90();  // Simple turnaround.
        turnRight90();
        recordMove(DIRECTION_RIGHT);
        recordMove(DIRECTION_RIGHT);

      }
    } else if (blueFreq < redFreq && blueFreq < greenFreq) {
      // Check if turning left has been tried before
      if (!hasDirectionBeenTried(currentPosition, DIRECTION_RIGHT)) {
        turnRight90();
        recordMove(DIRECTION_RIGHT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_LEFT)) {
        turnLeft90();
        recordMove(DIRECTION_LEFT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_BACKWARD)) {
        // Implement backing up logic and direction recording here
        int lastRight = sonarRight.ping_cm();
        int lastLeft = sonarLeft.ping_cm();
        do {
          moveBackward(SLOWLY);
          delay(10);
          Distance_L = sonarLeft.ping_cm();
          Distance_R = sonarRight.ping_cm();
        } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));  // Back up until 15cm clearance
        delay(100);
        stopMotors();
        recordMove(DIRECTION_BACKWARD);
        turnRight90();
        recordMove(DIRECTION_RIGHT);
      } else {
        // Nowhere to go!  Handle the dead end (turn around, etc.)
        turnRight90();  // Simple turnaround.
        turnRight90();
        recordMove(DIRECTION_RIGHT);
        recordMove(DIRECTION_RIGHT);
      }

    } else if (Distance_F < 20 && Distance_L < 12 && Distance_R < 12) {
      // Check if turning left has been tried before
      if (!hasDirectionBeenTried(currentPosition, DIRECTION_RIGHT)) {
        turnRight90();
        recordMove(DIRECTION_RIGHT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_LEFT)) {
        turnLeft90();
        recordMove(DIRECTION_LEFT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_BACKWARD)) {
        // Implement backing up logic and direction recording here
        int lastRight = sonarRight.ping_cm();
        int lastLeft = sonarLeft.ping_cm();
        do {
          moveBackward(SLOWLY);
          delay(10);
          Distance_L = sonarLeft.ping_cm();
          Distance_R = sonarRight.ping_cm();
        } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));  // Back up until 15cm clearance
        delay(100);
        stopMotors();
        recordMove(DIRECTION_BACKWARD);
        turnRight90();
        recordMove(DIRECTION_RIGHT);
      } else {
        // Nowhere to go!  Handle the dead end (turn around, etc.)
        turnRight90();  // Simple turnaround.
        turnRight90();
        recordMove(DIRECTION_RIGHT);
        recordMove(DIRECTION_RIGHT);
      }

    } else {
      // Check if turning left has been tried before
      if (!hasDirectionBeenTried(currentPosition, DIRECTION_RIGHT)) {
        turnRight90();
        recordMove(DIRECTION_RIGHT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_LEFT)) {
        turnLeft90();
        recordMove(DIRECTION_LEFT);
      } else if (!hasDirectionBeenTried(currentPosition, DIRECTION_BACKWARD)) {
        // Implement backing up logic and direction recording here
        int lastRight = sonarRight.ping_cm();
        int lastLeft = sonarLeft.ping_cm();
        do {
          moveBackward(SLOWLY);
          delay(10);
          Distance_L = sonarLeft.ping_cm();
          Distance_R = sonarRight.ping_cm();
        } while ((Distance_L + Distance_R) < (lastLeft + lastRight + 3));  // Back up until 15cm clearance
        delay(100);
        stopMotors();
        recordMove(DIRECTION_BACKWARD);
        turnRight90();
        recordMove(DIRECTION_RIGHT);
      } else {
        // Nowhere to go!  Handle the dead end (turn around, etc.)
        turnRight90();  // Simple turnaround.
        turnRight90();
        recordMove(DIRECTION_RIGHT);
        recordMove(DIRECTION_RIGHT);
      }
    }

  }
  // Case 2: Approaching obstacle (20-70cm)
  else if (Distance_F < 70 && Distance_F != 0) {
    if (i == 1) {  // If we were moving fast
      smoothStop();
      delay(200);  // Added missing semicolon
    }
    i = 0;
    moveForward(SLOWLY);  // Proceed with caution
    recordMove(DIRECTION_FORWARD);
  }
  // Case 3: Clear path ahead
  else {
    moveForward(MAX_SPEED);
    i = 1;  // Flag that we're moving fast
    recordMove(DIRECTION_FORWARD);
  }

  delay(20);  // Small delay between readings


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
  correction = constrain(correction, -speed / 2, speed / 2);

  return correction;
}

// Simple moving average filter for distance readings
float filterDistance(float currentDistance) {
  static float filteredDistances[3] = {0};
  static byte index = 0;

  // Ignore zero readings (common with ultrasonic sensors)
  if (currentDistance == 0) {
    return filteredDistances[(index - 1) % 3]; // Return last valid reading
  }

  filteredDistances[index] = currentDistance;
  index = (index + 1) % 3;

  // Return simple moving average
  return (filteredDistances[0] + filteredDistances[1] + filteredDistances[2]) / 3.0;
}

void moveForward(int speed) {
  float correction = calculatePID(speed);

  // Calculate motor speeds with more constrained limits
  int leftSpeed = constrain(speed - correction, speed / 2, speed * 1.5);
  int rightSpeed = constrain(speed + correction, speed / 2, speed * 1.5);

  // Apply speeds
  analogWrite(M_LEFT_ENA, leftSpeed);
  analogWrite(M_RIGHT_ENB, rightSpeed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}


void moveBackward(int speed) {
  // Calculate correction (same as forward movement)
  float correction = calculatePID(speed);

  // Calculate motor speeds (same logic as forward)
  int leftSpeed = constrain(speed - correction, speed / 2, speed * 1.5);
  int rightSpeed = constrain(speed + correction, speed / 2, speed * 1.5);

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
  if (Distance_F <= 50) {
    while (Distance_F > 20 && currentSpeed > 0) {
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

//----------------------- Maze Mapping Functions -----------------------

// Records a move (direction) in the mazeMemory array
void recordMove(byte direction) {
  mazeMemory[currentPosition] |= direction;  // Mark the direction as visited
  Serial.print("Record move: Pos: ");
  Serial.print(currentPosition);
  Serial.print(", Direction: 0x");
  Serial.println(direction, HEX);
  currentPosition = (currentPosition + 1) % MAX_MAZE_SIZE; // Increment position (wrap around if needed)

  //Potentially write the maze memory to EEPROM less frequently than every move to save on EEPROM writes.
  if (currentPosition % 4 == 0) {
    saveMazeToEEPROM();
  }

}

// Checks if a direction has been tried from the current position
boolean hasDirectionBeenTried(int position, byte direction) {
  return (mazeMemory[position] & direction) != 0;
}


void saveMazeToEEPROM() {
  Serial.println("Saving maze to EEPROM...");
  for (int i = 0; i < MAX_MAZE_SIZE; i++) {
    EEPROM.update(i, mazeMemory[i]);
    Serial.print("EEPROM [");
    Serial.print(i);
    Serial.print("] = 0x");
    Serial.println(EEPROM.read(i), HEX);
  }
}

void loadMazeFromEEPROM() {
  Serial.println("Loading maze from EEPROM...");
  for (int i = 0; i < MAX_MAZE_SIZE; i++) {
    mazeMemory[i] = EEPROM.read(i);
    Serial.print("EEPROM [");
    Serial.print(i);
    Serial.print("] = 0x");
    Serial.println(EEPROM.read(i), HEX);
  }
}