#include <Servo.h>

// Servo objects
Servo base, arm1, arm2, gripper;

// Servo configuration
const uint8_t servoPins[4] = {A0, A1, A2, A3};

// MG90S specific adjustments
const int MG90S_MIN_US = 500;
const int MG90S_MAX_US = 2500;

// Current and target positions [base, arm1, arm2, gripper]
float currentPos[4] = {0, 180, 180, 80};
float targetPos[4] = {0, 180, 180, 80};

// Predefined positions [base, arm1, arm2, gripper]
const float positions[25][4] = {
  {0, 180, 180, 80},    // HOME
  {0, 30, 90, 80},      // P1
  {0, 30, 90, 30},      // P2
  {0, 60, 30, 30},      // P3
  {0, 100, 70, 30},     // P4
  {0, 180, 180, 30},    // P5 
  {180, 180, 180, 30},  // P6
  {180, 90, 120, 30},   // P7 
  {180, 90, 120, 80},   // P8
  {180, 140, 140, 80},  // P9
  {180, 120, 160, 30},  // P10
  {180, 120, 160, 80},  // P11
  {180, 140, 120, 80},  // P12
  {180, 180, 180, 80},  // P13
  {180, 180, 180, 80},  // P14
  {180, 90, 120, 80},   // P15
  {180, 90, 120, 30},   // P16
  {180, 140, 140, 30},  // P17
  {180, 120, 160, 80},  // P18
  {180, 120, 160, 30},  // P19
  {180, 140, 120, 30},  // P20
  {180, 180, 180, 30},  // P21
  {0, 180, 180, 30},    // P22
  {0, 30, 90, 30},      // P23
  {0, 30, 90, 80}       // P24
};

// Movement control
const uint16_t moveDelay = 20;
unsigned long lastMoveTime = 0;
bool isMoving = false;
unsigned long moveStartTime = 0;
float moveDuration = 0;

// Sequence control
const int sequence1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 0};
const int sequence2[] = {0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 0};
const int sequence3[] = {0, 14, 18, 19, 20, 21, 22, 23, 24, 0};
const int sequence4[] = {0, 14, 15, 16, 17, 21, 22, 23, 24, 0};

const int seq1Length = sizeof(sequence1)/sizeof(sequence1[0]);
const int seq2Length = sizeof(sequence2)/sizeof(sequence2[0]);
const int seq3Length = sizeof(sequence3)/sizeof(sequence3[0]);
const int seq4Length = sizeof(sequence4)/sizeof(sequence4[0]);

int currentSequence = 1;
int sequenceStep = 0;
unsigned long stepStartTime = 0;
bool sequenceComplete = false;
const unsigned long stepDelay = 1000;

void setup() {
  Serial.begin(115200);
  
  // Attach servos
  base.attach(servoPins[0]);
  arm1.attach(servoPins[1], MG90S_MIN_US, MG90S_MAX_US);
  arm2.attach(servoPins[2]);
  gripper.attach(servoPins[3]);
  
  // Initialize positions
  writeAllServos();
  delay(1000);
  Serial.println("Starting sequences...");
}

void loop() {
  if (!sequenceComplete) {
    switch(currentSequence) {
      case 1:
        runSequence(sequence1, seq1Length);
        if (sequenceStep >= seq1Length) {
          currentSequence = 2;
          sequenceStep = 0;
          Serial.println("Starting sequence 2...");
        }
        break;
      case 2:
        runSequence(sequence2, seq2Length);
        if (sequenceStep >= seq2Length) {
          currentSequence = 3;
          sequenceStep = 0;
          Serial.println("Starting sequence 3...");
        }
        break;
      case 3:
        runSequence(sequence3, seq3Length);
        if (sequenceStep >= seq3Length) {
          currentSequence = 4;
          sequenceStep = 0;
          Serial.println("Starting sequence 4...");
        }
        break;
      case 4:
        runSequence(sequence4, seq4Length);
        if (sequenceStep >= seq4Length) {
          sequenceComplete = true;
          Serial.println("All sequences complete!");
        }
        break;
    }
  }
  
  if (isMoving) {
    updateServos();
  }
}

void runSequence(const int seq[], int seqLength) {
  if (!isMoving) {
    if (millis() - stepStartTime > stepDelay || sequenceStep == 0) {
      if (sequenceStep < seqLength) {
        goToPosition(seq[sequenceStep]);
        sequenceStep++;
        stepStartTime = millis();
      }
    }
  }
}

void writeAllServos() {
  base.write(round(currentPos[0]));
  arm1.write(round(currentPos[1]));
  arm2.write(round(currentPos[2]));
  gripper.write(round(currentPos[3]));
}

void updateServos() {
  if (millis() - lastMoveTime < moveDelay) return;
  lastMoveTime = millis();

  float progress = (millis() - moveStartTime) / moveDuration;
  progress = constrain(progress, 0.0, 1.0);
  
  for (int i = 0; i < 4; i++) {
    currentPos[i] = currentPos[i] + (targetPos[i] - currentPos[i]) * progress;
  }
  
  writeAllServos();
  
  if (progress >= 1.0) {
    isMoving = false;
  }
}

void goToPosition(uint8_t posIndex) {
  if (posIndex >= 25) return;
  
  Serial.print("\nMoving to position ");
  Serial.println(posIndex);
  
  // Calculate movement duration
  float maxDistance = 0;
  for (uint8_t i = 0; i < 4; i++) {
    float distance = abs(positions[posIndex][i] - currentPos[i]);
    if (distance > maxDistance) maxDistance = distance;
  }
  
  moveDuration = maxDistance * 15;
  moveStartTime = millis();
  
  // Set targets
  for (uint8_t i = 0; i < 4; i++) {
    targetPos[i] = positions[posIndex][i];
    Serial.print("Servo "); Serial.print(i); 
    Serial.print(" moving from "); Serial.print(currentPos[i]);
    Serial.print(" to "); Serial.println(targetPos[i]);
  }
  
  isMoving = true;
}