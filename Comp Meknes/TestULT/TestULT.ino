#include <Arduino.h>

// Pin definitions for the ultrasonic sensors
#define TRIG_PIN 7         // Single trigger pin for all sensors
#define ECHO_F 8           // Front sensor echo pin
#define ECHO_R 11          // Right sensor echo pin
#define ECHO_L 12          // Left sensor echo pin

// Constants
#define MAX_DISTANCE 400   // Maximum distance in cm
#define TIMEOUT 50000      // Increased timeout in microseconds

// Variables for sensor readings
float distance_front, distance_right, distance_left;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  
  // Set trigger pin as OUTPUT
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  // Set echo pins as INPUT
  pinMode(ECHO_F, INPUT);
  pinMode(ECHO_R, INPUT);
  pinMode(ECHO_L, INPUT);
  
  Serial.println("Ultrasonic Sensors with Single TRIG Pin - Improved Version");
  
  delay(1000);
}

void loop() {
  // Get readings from all three sensors
  measureDistances();
  
  // Print the results
  Serial.print("Front: ");
  Serial.print(distance_front);
  Serial.print(" cm | Right: ");
  Serial.print(distance_right);
  Serial.print(" cm | Left: ");
  Serial.print(distance_left);
  Serial.println(" cm");
  
  // Delay before next reading cycle
  delay(100);
}

void measureDistances() {
  long duration_front, duration_right, duration_left;
  
  // Measure front sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_front = pulseIn(ECHO_F, HIGH, TIMEOUT);
  
  // Small delay between sensors
  delay(10);
  
  // Measure right sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_right = pulseIn(ECHO_R, HIGH, TIMEOUT);
  
  delay(10);
  
  // Measure left sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_left = pulseIn(ECHO_L, HIGH, TIMEOUT);
  
  // Calculate distances
  distance_front = (duration_front == 0) ? MAX_DISTANCE : (duration_front * 0.034 / 2.0);
  distance_right = (duration_right == 0) ? MAX_DISTANCE : (duration_right * 0.034 / 2.0);
  distance_left = (duration_left == 0) ? MAX_DISTANCE : (duration_left * 0.034 / 2.0);
  
  // Apply simple filtering for stability
  static float prev_front = MAX_DISTANCE;
  static float prev_right = MAX_DISTANCE;
  static float prev_left = MAX_DISTANCE;
  
  distance_front = (distance_front * 0.7) + (prev_front * 0.3);
  distance_right = (distance_right * 0.7) + (prev_right * 0.3);
  distance_left = (distance_left * 0.7) + (prev_left * 0.3);
  
  // Save current readings as previous for next iteration
  prev_front = distance_front;
  prev_right = distance_right;
  prev_left = distance_left;
}

