// Motor Control Pins
#define ENA 5   // PWM for left side speed
#define ENB 3   // PWM for right side speed
#define IN1_1 4 // Left Front motor direction
#define IN2_1 2
#define IN3_1 6 // Left Rear motor direction
#define IN4_1 7
#define IN1_2 9  // Right Front motor direction
#define IN2_2 10
#define IN3_2 11 // Right Rear motor direction
#define IN4_2 12

// Sensor Pins
#define IR_SENSOR_PIN 8    // Digital IR sensor for wall tracking
#define TRIG_PIN A5         // Ultrasonic trigger
#define ECHO_PIN A4         // Ultrasonic echo

// Thresholds
#define OBSTACLE_DISTANCE 20 // CM threshold for front obstacle
#define WALL_FOLLOW_SPEED 100 // Speed during wall following
#define TURN_DURATION 500   // ms to turn when avoiding obstacle

// Control variables
int speed = 70;
int turnSpeed = 80;
bool wallFollowingMode = false;

void setup() {
  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1_1, OUTPUT); pinMode(IN2_1, OUTPUT);
  pinMode(IN3_1, OUTPUT); pinMode(IN4_1, OUTPUT);
  pinMode(IN1_2, OUTPUT); pinMode(IN2_2, OUTPUT);
  pinMode(IN3_2, OUTPUT); pinMode(IN4_2, OUTPUT);
  
  // Sensor pins
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  Serial.begin(9600);
  Serial.println("Enhanced Mecanum Robot Ready");
}

void loop() {
  // Read sensors
  bool wallDetected = digitalRead(IR_SENSOR_PIN); // Digital read instead of analog
  float distance = getUltrasonicDistance();
  
  // Check for front obstacle
  if (distance > 0 && distance > OBSTACLE_DISTANCE) {
    avoidObstacle();
    return; // Skip manual control during avoidance
  }
  
  // Wall following logic
  if (wallFollowingMode) {
    followWall(wallDetected);
    return; // Skip manual control in wall following mode
  }
  
  // Manual control via serial
  if (Serial.available() > 0) {
    handleSerialCommand();
  }
}

float getUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2; // Convert to cm
}

void avoidObstacle() {
  Serial.println("Obstacle detected! Avoiding...");
  stopMotors();
  delay(200);
  
  // Turn right (you can modify this to check both sides)
  moveRight();
  delay(TURN_DURATION);
  stopMotors();
}

void followWall(bool wallDetected) {
  Serial.print("Wall following - Detected: "); Serial.println(wallDetected ? "YES" : "NO");
  
  if (wallDetected) {
    // Wall detected - adjust right to maintain distance
    mecanumAdjust(1); // Move slightly right
  } else {
    // No wall detected - adjust left to find wall
    mecanumAdjust(-1); // Move slightly left
  }
}

void mecanumAdjust(int direction) {
  // direction: -1 = left, 1 = right
  analogWrite(ENA, WALL_FOLLOW_SPEED);
  analogWrite(ENB, WALL_FOLLOW_SPEED);
  
  if (direction < 0) {
    // Adjust left
    digitalWrite(IN1_1, HIGH); digitalWrite(IN2_1, LOW);
    digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, HIGH);
    digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, LOW);
    digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, HIGH);
  } else {
    // Adjust right
    digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, HIGH);
    digitalWrite(IN3_1, HIGH); digitalWrite(IN4_1, LOW);
    digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, HIGH);
    digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, LOW);
  }
}

void handleSerialCommand() {
  char command = Serial.read();
  
  switch(toupper(command)) {
    case 'W': moveForward(); break;
    case 'S': moveBackward(); break;
    case 'A': moveLeft(); break;
    case 'D': moveRight(); break;
    case 'Q': moveForwardLeft(); break;
    case 'E': moveForwardRight(); break;
    case 'Z': moveBackwardLeft(); break;
    case 'C': moveBackwardRight(); break;
    case 'X': stopMotors(); break;
    case '+': speed = min(speed + 10, 255); break;
    case '-': speed = max(speed - 10, 0); break;
    case 'M': // Toggle wall following mode
      wallFollowingMode = !wallFollowingMode;
      Serial.print("Wall following mode: ");
      Serial.println(wallFollowingMode ? "ON" : "OFF");
      if (!wallFollowingMode) stopMotors();
      break;
  }
}

// Movement functions for mecanum wheels
void moveForward() {
  // Left motors forward
  digitalWrite(IN1_1, HIGH); digitalWrite(IN2_1, LOW);
  digitalWrite(IN3_1, HIGH); digitalWrite(IN4_1, LOW);
  // Right motors forward
  digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, LOW);
  digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, LOW);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}

void moveBackward() {
  // Left motors backward
  digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, HIGH);
  digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, HIGH);
  // Right motors backward
  digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, HIGH);
  digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, HIGH);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}

void moveLeft() {
  // Left motors backward
  digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, HIGH);
  digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, HIGH);
  // Right motors forward
  digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, LOW);
  digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, LOW);
  analogWrite(ENA, turnSpeed); analogWrite(ENB, turnSpeed);
}

void moveRight() {
  // Left motors forward
  digitalWrite(IN1_1, HIGH); digitalWrite(IN2_1, LOW);
  digitalWrite(IN3_1, HIGH); digitalWrite(IN4_1, LOW);
  // Right motors backward
  digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, HIGH);
  digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, HIGH);
  analogWrite(ENA, turnSpeed); analogWrite(ENB, turnSpeed);
}

void moveForwardLeft() {
  // Left front stop, left rear forward
  digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, LOW);
  digitalWrite(IN3_1, HIGH); digitalWrite(IN4_1, LOW);
  // Right front forward, right rear stop
  digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, LOW);
  digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, LOW);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}

void moveForwardRight() {
  // Left front forward, left rear stop
  digitalWrite(IN1_1, HIGH); digitalWrite(IN2_1, LOW);
  digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, LOW);
  // Right front stop, right rear forward
  digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, LOW);
  digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, LOW);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}

void moveBackwardLeft() {
  // Left front stop, left rear backward
  digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, LOW);
  digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, HIGH);
  // Right front backward, right rear stop
  digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, HIGH);
  digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, LOW);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}

void moveBackwardRight() {
  // Left front backward, left rear stop
  digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, HIGH);
  digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, LOW);
  // Right front stop, right rear backward
  digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, LOW);
  digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, HIGH);
  analogWrite(ENA, speed); analogWrite(ENB, speed);
}

void stopMotors() {
  // All motors stop (brake)
  digitalWrite(IN1_1, LOW); digitalWrite(IN2_1, LOW);
  digitalWrite(IN3_1, LOW); digitalWrite(IN4_1, LOW);
  digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, LOW);
  digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, LOW);
  analogWrite(ENA, 0); analogWrite(ENB, 0);
}