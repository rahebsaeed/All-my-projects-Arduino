//   Case1                                                                                
//                                                                                   
//                                                                                   
//                        __________                                                          
//                       |          |                                                 
//                       |          |                                                 
//                       |          |                                                 
//          |____________| OO       |________________                                                                         
//          |                                                                         
//          |                              -->                                          
//          |_____________           ______________                                                             
//                       |          |                                                  
//                       |          |                                                  
//                       |          |                                                  
//                       |  Start   |                                                 





//                                   
#include <Ultrasonic.h>

// --- Pin Definitions ---
// Motor Driver Pins (L298N)
#define M_LEFT_IN1  2
#define M_LEFT_IN2  3
#define M_RIGHT_IN3 4
#define M_RIGHT_IN4 5
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10

// Ultrasonic Sensor Pins
#define TRIG_F 6
#define ECHO_F 7
#define TRIG_R 8
#define ECHO_R A2
#define TRIG_L A3
#define ECHO_L 11

// Infrared Sensor Pins
#define INF_R 12 // Right IR
#define INF_L A5 // Left IR
#define INF_B 13 // Back IR

// --- Constants ---
#define BASE_SPEED 125
#define MAX_SPEED 180
#define TURN_SPEED 100
#define FRONT_SPEED_THRESHOLD 100.0 // cm
#define MIN_OBSTACLE_DISTANCE 30.0 // cm
#define IR_OBSTACLE_STATE LOW // LOW means obstacle detected
#define BACKUP_SPEED 100
#define MIN_DISTANCE_CHANGE_PERCENT 150  // 150% increase = 1.5x previous distance
#define MIN_ABSOLUTE_CHANGE 30          // Minimum absolute change in cm to consider
#define STABLE_READINGS_REQUIRED 3      // Number of consistent readings needed
#define HISTORY_SIZE 5                  // Number of readings to track
#define MAX_OPENINGS 10                 // Maximum openings to remember per direction

Ultrasonic sonarFront(TRIG_F, ECHO_F);
Ultrasonic sonarRight(TRIG_R, ECHO_R);
Ultrasonic sonarLeft(TRIG_L, ECHO_L);

// Structure to store opening information
struct Opening {
  int frontDistance;
  int sideDistance;
  unsigned long timestamp;
};

// Circular buffers for storing openings
Opening leftOpenings[MAX_OPENINGS];
Opening rightOpenings[MAX_OPENINGS];
int leftOpeningCount = 0;
int rightOpeningCount = 0;
int leftOpeningIndex = 0;
int rightOpeningIndex = 0;

// Structure to track distance trends
typedef struct {
  int readings[HISTORY_SIZE];
  int index;
  bool stable;
  int stableCount;
} DistanceTrend;

DistanceTrend frontTrend = {{0}, 0, false, 0};
DistanceTrend leftTrend = {{0}, 0, false, 0};
DistanceTrend rightTrend = {{0}, 0, false, 0};

void setup() {
  // Motor control pins
  pinMode(M_LEFT_IN1, OUTPUT);
  pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT);
  pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT);
  pinMode(M_RIGHT_ENB, OUTPUT);
  
  // IR sensor pins
  pinMode(INF_R, INPUT);
  pinMode(INF_L, INPUT);
  pinMode(INF_B, INPUT);
  
  Serial.begin(9600);
  
  // Initialize trend buffers with current readings
  int initialDist = sonarFront.ping_cm();
  for (int i = 0; i < HISTORY_SIZE; i++) {
    frontTrend.readings[i] = initialDist;
    leftTrend.readings[i] = initialDist;
    rightTrend.readings[i] = initialDist;
  }
}

void updateTrend(DistanceTrend* trend, int newDistance) {
  // Update the circular buffer
  trend->readings[trend->index] = newDistance;
  trend->index = (trend->index + 1) % HISTORY_SIZE;
  
  // Check if readings are stable (not fluctuating)
  int minReading = trend->readings[0];
  int maxReading = trend->readings[0];
  for (int i = 1; i < HISTORY_SIZE; i++) {
    if (trend->readings[i] < minReading) minReading = trend->readings[i];
    if (trend->readings[i] > maxReading) maxReading = trend->readings[i];
  }
  
  // If readings haven't fluctuated much, consider stable
  if ((maxReading - minReading) < 10) {  // Less than 10cm variation
    trend->stableCount++;
    if (trend->stableCount >= STABLE_READINGS_REQUIRED) {
      trend->stable = true;
    }
  } else {
    trend->stable = false;
    trend->stableCount = 0;
  }
}

bool detectOpeningDynamic(DistanceTrend* trend, int currentDistance) {
  if (!trend->stable) return false;
  
  // Calculate average of previous stable readings
  int sum = 0;
  for (int i = 0; i < HISTORY_SIZE; i++) {
    sum += trend->readings[i];
  }
  int avgPrevious = sum / HISTORY_SIZE;
  
  // Calculate percentage change
  float percentChange = ((float)currentDistance - avgPrevious) / avgPrevious * 100;
  
  // Detect opening when:
  // 1. Significant percentage increase AND
  // 2. Significant absolute increase
  bool significantChange = (percentChange >= MIN_DISTANCE_CHANGE_PERCENT) && 
                          ((currentDistance - avgPrevious) >= MIN_ABSOLUTE_CHANGE);
  
  if (significantChange) {
    // Reset stability after detecting an opening
    trend->stable = false;
    trend->stableCount = 0;
    return true;
  }
  
  return false;
}

void addLeftOpening(int frontDist, int leftDist) {
  leftOpenings[leftOpeningIndex] = {frontDist, leftDist, millis()};
  leftOpeningIndex = (leftOpeningIndex + 1) % MAX_OPENINGS;
  if (leftOpeningCount < MAX_OPENINGS) leftOpeningCount++;
  Serial.print("Stored left opening. Total: ");
  Serial.println(leftOpeningCount);
}

void addRightOpening(int frontDist, int rightDist) {
  rightOpenings[rightOpeningIndex] = {frontDist, rightDist, millis()};
  rightOpeningIndex = (rightOpeningIndex + 1) % MAX_OPENINGS;
  if (rightOpeningCount < MAX_OPENINGS) rightOpeningCount++;
  Serial.print("Stored right opening. Total: ");
  Serial.println(rightOpeningCount);
}

Opening getRecentLeftOpening() {
  if (leftOpeningCount == 0) return {-1, -1, 0};
  int index = (leftOpeningIndex - 1 + MAX_OPENINGS) % MAX_OPENINGS;
  return leftOpenings[index];
}

Opening getRecentRightOpening() {
  if (rightOpeningCount == 0) return {-1, -1, 0};
  int index = (rightOpeningIndex - 1 + MAX_OPENINGS) % MAX_OPENINGS;
  return rightOpenings[index];
}

void printStoredOpenings() {
  Serial.println("Stored Openings:");
  Serial.println("Left openings:");
  for (int i = 0; i < leftOpeningCount; i++) {
    int idx = (leftOpeningIndex - 1 - i + MAX_OPENINGS) % MAX_OPENINGS;
    Serial.print(i+1);
    Serial.print(": Front=");
    Serial.print(leftOpenings[idx].frontDistance);
    Serial.print("cm, Side=");
    Serial.print(leftOpenings[idx].sideDistance);
    Serial.print("cm, Age=");
    Serial.print((millis() - leftOpenings[idx].timestamp) / 1000);
    Serial.println("s");
  }

  Serial.println("Right openings:");
  for (int i = 0; i < rightOpeningCount; i++) {
    int idx = (rightOpeningIndex - 1 - i + MAX_OPENINGS) % MAX_OPENINGS;
    Serial.print(i+1);
    Serial.print(": Front=");
    Serial.print(rightOpenings[idx].frontDistance);
    Serial.print("cm, Side=");
    Serial.print(rightOpenings[idx].sideDistance);
    Serial.print("cm, Age=");
    Serial.print((millis() - rightOpenings[idx].timestamp) / 1000);
    Serial.println("s");
  }
}

// --- Motor Control Functions ---
void stopMotors() {
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void turnLeft(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void turnRight(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void moveBack(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
}

void moveForward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void loop() {
  // Read sensor distances
  int frontDist = sonarFront.ping_cm();
  int leftDist = sonarLeft.ping_cm();
  int rightDist = sonarRight.ping_cm();
  
  // Update distance trends
  updateTrend(&frontTrend, frontDist);
  updateTrend(&leftTrend, leftDist);
  updateTrend(&rightTrend, rightDist);
  
  // Detect openings using dynamic thresholds
  bool leftOpening = detectOpeningDynamic(&leftTrend, leftDist);
  bool rightOpening = detectOpeningDynamic(&rightTrend, rightDist);
  bool frontOpening = detectOpeningDynamic(&frontTrend, frontDist);
  
  if (leftOpening) {
    Serial.print("Left opening detected! Change from ~");
    Serial.print(leftTrend.readings[(leftTrend.index - 1 + HISTORY_SIZE) % HISTORY_SIZE]);
    Serial.print("cm to ");
    Serial.print(leftDist);
    Serial.println("cm");
    addLeftOpening(frontDist, leftDist);
  }
  
  if (rightOpening) {
    Serial.print("Right opening detected! Change from ~");
    Serial.print(rightTrend.readings[(rightTrend.index - 1 + HISTORY_SIZE) % HISTORY_SIZE]);
    Serial.print("cm to ");
    Serial.print(rightDist);
    Serial.println("cm");
    addRightOpening(frontDist, rightDist);
  }
  
  // Print stored openings when requested
  if (Serial.available()) {
    Serial.read(); // Clear the buffer
    printStoredOpenings();
  }
  delay(500); // Small delay to prevent overwhelming the serial monitor

}