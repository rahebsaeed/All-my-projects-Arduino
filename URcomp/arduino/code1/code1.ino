// Motor pins
#define ENA 9   // Right motor speed control
#define ENB 10  // Left motor speed control
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// Ultrasonic sensor pins
#define TRIG 8
#define ECHO 7

// IR sensor pins
#define IR_RIGHT A0
#define IR_MIDRIGHT A1
#define IR_MIDLEFT A2
#define IR_LEFT A3

// Thresholds (adjust based on your surface)
#define IR_THRESHOLD 400  // Below this means white line detected
#define OBSTACLE_DISTANCE 30  // cm

// Motor speeds (adjust these values between 0-255)

#define DEFAULT_SPEED 150
#define TURN_SPEED 180
#define REVERSE_SPEED 180
#define ATTACK_SPEED 255  // Max speed for attacking

void setup() {
  // Motor control pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  // Ultrasonic sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  // Start with motors stopped
  stopMotors();
}

void loop() {
  // Read IR sensors
  int irRight = analogRead(IR_RIGHT);
  int irMidRight = analogRead(IR_MIDRIGHT);
  int irMidLeft = analogRead(IR_MIDLEFT);
  int irLeft = analogRead(IR_LEFT);
  
  // Read ultrasonic distance
  float distance = getDistance();
  
  // Sumo robot logic:
  if (irRight < IR_THRESHOLD || irMidRight < IR_THRESHOLD || 
      irMidLeft < IR_THRESHOLD || irLeft < IR_THRESHOLD) {
    avoidEdge();
  }
  else if (distance < OBSTACLE_DISTANCE) {
    attack();
  }
  else {
    search();
  }
}

float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  long duration = pulseIn(ECHO, HIGH);
  return duration * 0.034 / 2;  // Convert to cm
}

void avoidEdge() {
  // Back up with reduced speed
  analogWrite(ENA, REVERSE_SPEED);
  analogWrite(ENB, REVERSE_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(200);
  
  // Turn away from the edge with turn speed
  if (analogRead(IR_RIGHT) < IR_THRESHOLD || analogRead(IR_MIDRIGHT) < IR_THRESHOLD) {
    // Turn left (right motor forward, left motor backward)
    analogWrite(ENA, TURN_SPEED);
    analogWrite(ENB, TURN_SPEED);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } 
  else {
    // Turn right (right motor backward, left motor forward)
    analogWrite(ENA, TURN_SPEED);
    analogWrite(ENB, TURN_SPEED);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  delay(400);
  stopMotors();
}

void attack() {
  // Full attack speed forward
  analogWrite(ENA, ATTACK_SPEED);
  analogWrite(ENB, ATTACK_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(300);
}

void search() {
  // Move forward with default speed
  analogWrite(ENA, DEFAULT_SPEED);
  analogWrite(ENB, DEFAULT_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(random(200, 500));
  
  // Turn slightly with turn speed
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  if (random(2) == 0) {  // Random left
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {  // Random right
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  delay(random(300, 700));
  stopMotors();
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}