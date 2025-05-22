#include <Servo.h>

// Define servo pins
const int frontLeftHip     = 3;
const int frontLeftKnee    = 5;
const int frontRightHip    = 6;
const int frontRightKnee   = 9;
const int rearLeftHip      = 10;
const int rearLeftKnee     = 11;
const int rearRightHip     = A0;
const int rearRightKnee    = A1;

// Create servo objects
Servo servoFLHip;
Servo servoFLKnee;
Servo servoFRHip;
Servo servoFRKnee;
Servo servoRLHip;
Servo servoRLKnee;
Servo servoRRHip;
Servo servoRRKnee;

void setup() {
  // Attach all servos to their pins
  servoFLHip.attach(frontLeftHip);
  servoFLKnee.attach(frontLeftKnee);
  servoFRHip.attach(frontRightHip);
  servoFRKnee.attach(frontRightKnee);
  servoRLHip.attach(rearLeftHip);
  servoRLKnee.attach(rearLeftKnee);
  servoRRHip.attach(rearRightHip);
  servoRRKnee.attach(rearRightKnee);

  // Set all servos to 90 degrees
  servoFLHip.write(90);
  servoFLKnee.write(90);
  servoFRHip.write(90);
  servoFRKnee.write(90);
  servoRLHip.write(90);
  servoRLKnee.write(90);
  servoRRHip.write(90);
  servoRRKnee.write(90);

  // Optional: Start serial for feedback
  Serial.begin(9600);
  Serial.println("All servos set to 90 degrees.");
}

void loop() {
  // No loop actions for now
}
