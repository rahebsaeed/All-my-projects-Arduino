// Définition des broches pour les moteurs et le pont H L298N
#define IN1 2  
#define IN2 3  
#define IN3 4  
#define IN4 5  
#define ENA A2
#define ENB A3

#define TRIG_0 6
#define ECHO_0 7
#define TRIG_R 8
#define ECHO_R 9
#define TRIG_L 10
#define ECHO_L 11

#define INF_R 12 
#define INF_L A5 
#define INF_B 13 
// sensor of color 
#define S2 A0 
#define S3 A1 
#define Out A4 

long duration_0, duration_R, duration_L;
int distance_0, distance_R, distance_L;

void setup() {
  // Initialisation des broches pour les moteurs et le pont H
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Initialisation des broches pour les capteurs à ultrasons
  pinMode(TRIG_0, OUTPUT);
  pinMode(ECHO_0, INPUT);
  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);
  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);

  // Initialisation des capteurs infrarouges
  pinMode(INF_R, INPUT);
  pinMode(INF_L, INPUT);
  pinMode(INF_B, INPUT);

  // Sérialisation pour le débogage
  Serial.begin(9600);
}

void loop() {
  // Read sensor values
  distance_0 = readUltrasonicDistance(TRIG_0, ECHO_0);
  distance_R = readUltrasonicDistance(TRIG_R, ECHO_R);
  distance_L = readUltrasonicDistance(TRIG_L, ECHO_L);

  int irRight = digitalRead(INF_R);
  int irLeft = digitalRead(INF_L);
  int irBack = digitalRead(INF_B);

  // --- START of Flowchart Logic ---

  // Is US_0 < 20?
  if (distance_0 < 20) {
    stopMovement();
    Condition_commun(irRight, irLeft, irBack, distance_R, distance_L);
  } else {
    moveForward();
    distance_0 = readUltrasonicDistance(TRIG_0, ECHO_0);
    if (distance_0 > 20) {
      Condition_commun(irRight, irLeft, irBack, distance_R, distance_L);
    }
  }
  delay(100);
}

// --- Function Definitions ---

// Function to read the distance from an ultrasonic sensor
long readUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// Function to move the robot forward
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Function to move the robot backward
void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Function to stop the robot
void stopMovement() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Function to turn the robot left
void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Function to turn the robot right
void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Function for turning randomly
void randomTurn() {
  int turnDirection = random(0, 2);  // Generate a random number between 0 and 1
  if (turnDirection == 0) {
    turnLeft();
    Serial.println("Random rotation to the left");
  } else {
    turnRight();
    Serial.println("Random rotation to the right");
  }
  delay(500); // Time to turn
}

// Function to handle common conditions
void Condition_commun(int irRight, int irLeft, int irBack, int distance_R, int distance_L) {
  if (irRight == HIGH && distance_R < 20) {
    while (irBack == LOW) {
      moveBackward();
      irBack = digitalRead(INF_B); // Update IR_B reading
    }
  } else if (distance_L > 20 || distance_R < 20 || irLeft == LOW) {
    while (distance_L > 20 || distance_R < 20 || irLeft == LOW) {
      turnLeft();
      distance_R = readUltrasonicDistance(TRIG_R, ECHO_R);
      distance_L = readUltrasonicDistance(TRIG_L, ECHO_L);
      irLeft = digitalRead(INF_L);
    }
  } else if (irLeft == LOW && distance_L < 20) {
    while (distance_R > 20 || distance_L < 20 || irRight == LOW) {
      turnRight();
      distance_R = readUltrasonicDistance(TRIG_R, ECHO_R);
      distance_L = readUltrasonicDistance(TRIG_L, ECHO_L);
      irRight = digitalRead(INF_R);
    }
  }
}