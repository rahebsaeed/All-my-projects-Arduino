#include <SoftwareSerial.h>

#define M_LEFT_IN1  4
#define M_LEFT_IN2  5
#define M_RIGHT_IN3 6
#define M_RIGHT_IN4 7
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10

// Pin definitions for the infrared sensors
// --- Broches des capteurs IR ---
#define SENSOR_OUTER_LEFT    A4
#define SENSOR_INNER_LEFT    A3
#define SENSOR_INNER_RIGHT   A2
#define SENSOR_OUTER_RIGHT   A1


// Constants
// --- Vitesses (0 à 255) ---
const int BASE_SPEED        = 100;  // avancer droit
const int TURN_SPEED        = 170;  // virage doux
const int SHARP_TURN_SPEED  = 220;  // virage appuyé


int speed = 250;
int i = 1;
unsigned long lastCommandTime = 0;
const unsigned long COMMAND_TIMEOUT = 100; // Stop if no command for 500ms


SoftwareSerial mySerial(2, 3); // RX, TX
void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(M_LEFT_IN1, OUTPUT); pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT); pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT); pinMode(M_RIGHT_ENB, OUTPUT);

  // Sensor pins
  // configuration capteurs
  pinMode(SENSOR_OUTER_LEFT,   INPUT);
  pinMode(SENSOR_INNER_LEFT,   INPUT);
  pinMode(SENSOR_INNER_RIGHT,  INPUT);
  pinMode(SENSOR_OUTER_RIGHT,  INPUT);



  stopMotors();
  Serial.println("Slave ready.");
  delay(1000);
}
bool needsInitialMove = false; // Flag for initial forward move in line mode
bool isMode = false; // false = manual, true = line-following
char lastCommand= 'S'; // Persistent state
char prevCommand = 'S'; // Previous command for speed adjustment
void loop() {
  // 1. Timeout check - return to stop if no commands received
  if (millis() - lastCommandTime > COMMAND_TIMEOUT) {
    lastCommand = 'S';
    stopMotors();  // Actually stop motors on timeout
  }
  if(isMode){
    lineFollowing();
  }
  // 2. Process incoming commands
  while (mySerial.available()) {
    char cmd = mySerial.read();
    
    // Filter out invalid commands first
    if (cmd == 'M' || strchr("FBRLS", cmd) != NULL) {
      Serial.print("Processing cmd: ");
      Serial.println(cmd);
      
      lastCommandTime = millis();  // Reset timeout timer
      lastCommand = cmd;
      if(isMode){
        lineFollowing();
      }
      if (cmd == 'M') {
        // Toggle between modes
        isMode = !isMode;
        stopMotors();       // Immediate stop
        Serial.println(isMode ? "LINE mode" : "MANUAL mode");
      }
    }
    else {
      Serial.print("Invalid cmd: ");
      Serial.println(cmd);
    }
  }

  // 3. Execute appropriate control mode
  if (!isMode) {
    // Manual control mode - process only valid movement commands
    switch (lastCommand) {
      case 'F': moveForward(speed); break;
      case 'B': moveBackward(speed); break;
      case 'L': turnLeft(speed); break;
      case 'R': turnRight(speed); break;
      case 'S': stopMotors(); break;
      default: break; // Ignore other commands
    }
  } 
  else {
    // Line following mode
    lineFollowing();
  }
}

// Add these global variables at the top of your program
enum TurnDirection { NONE, LEFT_TURN, RIGHT_TURN };
TurnDirection lastTurn = NONE;

void lineFollowing() {
  // Lecture des capteurs (inverted logic)
  int outerLeftSensor  = digitalRead(SENSOR_OUTER_LEFT);  
  int innerLeftSensor  = digitalRead(SENSOR_INNER_LEFT);
  int innerRightSensor = digitalRead(SENSOR_INNER_RIGHT);
  int outerRightSensor = digitalRead(SENSOR_OUTER_RIGHT);

  // 1) Tous les capteurs voient du blanc (hors ligne)
  if (outerLeftSensor  == LOW &&
      innerLeftSensor  == LOW &&
      innerRightSensor == LOW &&
      outerRightSensor == LOW) {
      stopMotors();
  }
  // 2) Intersection épaisse (tous sur la ligne)
  else if (outerLeftSensor  == HIGH &&
           innerLeftSensor  == HIGH &&
           innerRightSensor == HIGH &&
           outerRightSensor == HIGH) {
    moveBackward(BASE_SPEED);
  }
  // 3) Capteurs intérieurs seuls => centré
  else if (outerLeftSensor  == LOW &&
           innerLeftSensor  == HIGH &&
           innerRightSensor == HIGH &&
           outerRightSensor == LOW) {
    moveBackward(BASE_SPEED);  // Changed to moveForward (logical for centered position)
  }
  // 4) Ligne légère à droite => virage DROIT doux
  else if (innerLeftSensor == HIGH && innerRightSensor == LOW) {
    turnLeft(TURN_SPEED);  // Fixed: was turnLeft
  }
  // 5) Ligne légère à gauche => virage GAUCHE doux
  else if (innerLeftSensor == LOW && innerRightSensor == HIGH) {
    turnRight(TURN_SPEED);  // Fixed: was turnRight
  }
  // 6) Ligne très à droite => virage DROIT appuyé
  else if (outerRightSensor == HIGH) {
    turnRight(SHARP_TURN_SPEED);  // Fixed: was turnLeft
  }
  // 7) Ligne très à gauche => virage GAUCHE appuyé
  else if (outerLeftSensor == HIGH) {
    turnLeft(SHARP_TURN_SPEED);  // Fixed: was turnRight
  }
  // 8) Cas indéfini => arrêt de sécurité
  else {
    stopMotors();
  }
}

// === Movement Functions ===
void moveForward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void moveBackward(int speed) {
  // Apply motor control IN REVERSE DIRECTION
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);    // Reverse polarity for backward movement
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
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

void stopMotors() {
  analogWrite(M_LEFT_ENA, 0);
  analogWrite(M_RIGHT_ENB, 0);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, LOW);
}