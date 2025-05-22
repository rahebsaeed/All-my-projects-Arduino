// Left Side Driver (Driver 1)
#define ENA 5   // PWM for left side speed
#define ENB 3   // PWM for right side speed
#define IN1_1 4 // Left Front motor direction
#define IN2_1 2
#define IN3_1 6 // Left Rear motor direction
#define IN4_1 7

// Right Side Driver (Driver 2)
#define IN1_2 9  // Right Front motor direction
#define IN2_2 10
#define IN3_2 11 // Right Rear motor direction
#define IN4_2 12

// Mecanum wheel control speeds
int speed = 70; // Default speed (0-255)
int turnSpeed = 80; // Turning speed

void setup() {
  // Set all pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1_1, OUTPUT); pinMode(IN2_1, OUTPUT);
  pinMode(IN3_1, OUTPUT); pinMode(IN4_1, OUTPUT);
  pinMode(IN1_2, OUTPUT); pinMode(IN2_2, OUTPUT);
  pinMode(IN3_2, OUTPUT); pinMode(IN4_2, OUTPUT);
  
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Mecanum Robot Ready");
  Serial.println("Use WASD keys to move, QE for diagonals, X to stop");
}

void loop() {
  if (Serial.available() > 0) {
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
    }
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