#include <Arduino.h>
#include <QTRSensors.h>

// Motor Driver Pins (L298N)
#define M_LEFT_IN1  2
#define M_LEFT_IN2  3
#define M_RIGHT_IN3 4
#define M_RIGHT_IN4 5
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10

// Constants
#define BASE_SPEED 50      // Base motor speed (0-255)
#define TURN_SPEED 80      // Speed during turns (0-255)
#define NUM_SENSORS 2      // Number of reflectance sensors
#define TIMEOUT 2500       // Waits for 2500 microseconds for sensor outputs to go low

// Sensor pins (using digital pins)
QTRSensors qtr;
unsigned int sensorValues[NUM_SENSORS];

void setup() {
  // Motor control pins
  pinMode(M_LEFT_IN1, OUTPUT);
  pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT);
  pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT);
  pinMode(M_RIGHT_ENB, OUTPUT);

  // Configure the sensor (digital version)
  qtr.setTypeRC(); // Use RC mode even for digital sensors
  qtr.setSensorPins((const uint8_t[]){6, 7}, NUM_SENSORS);
  
  // Calibration sequence
  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Turn on Arduino's LED to indicate calibration
  
  // Manual calibration: move sensors over line during this delay
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(10);
  }
  digitalWrite(LED_BUILTIN, LOW); // Turn off LED to indicate calibration is done
  
  delay(1000);
}

void loop() {
  lineFollowing();
  delay(10);
}

void lineFollowing() {
  // Read calibrated sensor values (will return 0 for white, 1000 for black)
  qtr.readCalibrated(sensorValues);
  
  // Get individual sensor readings
  int leftSensor = sensorValues[0];
  int rightSensor = sensorValues[1];
  
  // Threshold for detecting the line
  int threshold = 500; // Midpoint between digital HIGH and LOW
  
  // Line following logic
  if (leftSensor > threshold && rightSensor > threshold) {
    // Both sensors on the line
    moveForward(BASE_SPEED);
  } 
  else if (leftSensor > threshold && rightSensor <= threshold) {
    // Line detected on left sensor - turn right
    turnLeft(TURN_SPEED);
  } 
  else if (leftSensor <= threshold && rightSensor > threshold) {
    // Line detected on right sensor - turn left
    turnRight(TURN_SPEED);
  } 
  else {
    // No line detected
    moveForward(BASE_SPEED);
  }
  
  // Optional: Print sensor values for debugging
  Serial.print(leftSensor);
  Serial.print('\t');
  Serial.println(rightSensor);
}

// --- Motor Control Functions (unchanged) ---
void stopMotors() {
  analogWrite(M_LEFT_ENA, 0);
  analogWrite(M_RIGHT_ENB, 0);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, LOW);
}

void moveBackward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, HIGH);
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

void moveForward(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, HIGH);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, HIGH);
  digitalWrite(M_RIGHT_IN4, LOW);
}