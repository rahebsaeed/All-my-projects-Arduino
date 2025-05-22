#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create servo driver instance
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Define servo channels (0–15)
const int frontLeftHip     = 3;
const int frontLeftKnee    = 5;
const int frontRightHip    = 6;
const int frontRightKnee   = 9;
const int rearLeftHip      = 10;
const int rearLeftKnee     = 11;
const int rearRightHip     = A0;
const int rearRightKnee    = A1;

// Angle to pulse function
int angleToPulse(int angle) {
  int minPulse = 150; // Minimum pulse length count (usually ~500 µs)
  int maxPulse = 600; // Maximum pulse length count (~2500 µs)
  return map(angle, 0, 180, minPulse, maxPulse);
}

// Default movement values
int neutralPos = 90;
int liftAngle = 30;
int stepSize = 20;
int turnAngle = 25;
int stepDelay = 300;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Scanning I2C bus...");
}

void loop() {
  
  byte error, address;
  int count = 0;

  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      count++;
    }
  }

  if (count == 0)
    Serial.println("No I2C devices found.\n");

  else
    Serial.println("Done.\n");

  delay(5000);
}

void setServo(int channel, int angle) {
  pwm.setPWM(channel, 0, angleToPulse(angle));
}

void resetAllServos() {
  setServo(frontLeftHip, neutralPos);
  setServo(frontLeftKnee, neutralPos);
  setServo(frontRightHip, neutralPos);
  setServo(frontRightKnee, neutralPos);
  setServo(rearLeftHip, neutralPos);
  setServo(rearLeftKnee, neutralPos);
  setServo(rearRightHip, neutralPos);
  setServo(rearRightKnee, neutralPos);
}

void moveForward() {
  setServo(frontRightKnee, neutralPos - liftAngle);
  setServo(rearLeftKnee, neutralPos - liftAngle);
  delay(stepDelay/2);

  setServo(frontRightHip, neutralPos + stepSize);
  setServo(rearLeftHip, neutralPos + stepSize);
  delay(stepDelay/2);

  setServo(frontRightKnee, neutralPos);
  setServo(rearLeftKnee, neutralPos);
  delay(stepDelay);

  setServo(frontLeftHip, neutralPos - stepSize);
  setServo(rearRightHip, neutralPos - stepSize);
  delay(stepDelay);

  setServo(frontLeftKnee, neutralPos - liftAngle);
  setServo(rearRightKnee, neutralPos - liftAngle);
  delay(stepDelay/2);

  setServo(frontLeftHip, neutralPos + stepSize);
  setServo(rearRightHip, neutralPos + stepSize);
  delay(stepDelay/2);

  setServo(frontLeftKnee, neutralPos);
  setServo(rearRightKnee, neutralPos);
  delay(stepDelay);

  setServo(frontRightHip, neutralPos - stepSize);
  setServo(rearLeftHip, neutralPos - stepSize);
  delay(stepDelay);

  // Reset all hips
  resetAllServos();
}

void moveBackward() {
  setServo(frontRightKnee, neutralPos - liftAngle);
  setServo(rearLeftKnee, neutralPos - liftAngle);
  delay(stepDelay/2);

  setServo(frontRightHip, neutralPos - stepSize);
  setServo(rearLeftHip, neutralPos - stepSize);
  delay(stepDelay/2);

  setServo(frontRightKnee, neutralPos);
  setServo(rearLeftKnee, neutralPos);
  delay(stepDelay);

  setServo(frontLeftHip, neutralPos + stepSize);
  setServo(rearRightHip, neutralPos + stepSize);
  delay(stepDelay);

  setServo(frontLeftKnee, neutralPos - liftAngle);
  setServo(rearRightKnee, neutralPos - liftAngle);
  delay(stepDelay/2);

  setServo(frontLeftHip, neutralPos - stepSize);
  setServo(rearRightHip, neutralPos - stepSize);
  delay(stepDelay/2);

  setServo(frontLeftKnee, neutralPos);
  setServo(rearRightKnee, neutralPos);
  delay(stepDelay);

  setServo(frontRightHip, neutralPos + stepSize);
  setServo(rearLeftHip, neutralPos + stepSize);
  delay(stepDelay);

  resetAllServos();
}

void turnLeft() {
  setServo(frontRightKnee, neutralPos - liftAngle);
  setServo(rearRightKnee, neutralPos - liftAngle);
  delay(stepDelay);

  setServo(frontRightHip, neutralPos + turnAngle);
  setServo(rearRightHip, neutralPos + turnAngle);
  delay(stepDelay);

  setServo(frontRightKnee, neutralPos);
  setServo(rearRightKnee, neutralPos);
  delay(stepDelay);

  setServo(frontLeftHip, neutralPos - turnAngle);
  setServo(rearLeftHip, neutralPos - turnAngle);
  delay(stepDelay);

  setServo(frontRightKnee, neutralPos - liftAngle);
  setServo(rearRightKnee, neutralPos - liftAngle);
  delay(stepDelay);

  setServo(frontRightHip, neutralPos);
  setServo(rearRightHip, neutralPos);
  delay(stepDelay);

  setServo(frontRightKnee, neutralPos);
  setServo(rearRightKnee, neutralPos);
  delay(stepDelay);

  setServo(frontLeftHip, neutralPos);
  setServo(rearLeftHip, neutralPos);
  delay(stepDelay);
}

void turnRight() {
  setServo(frontLeftKnee, neutralPos - liftAngle);
  setServo(rearLeftKnee, neutralPos - liftAngle);
  delay(stepDelay);

  setServo(frontLeftHip, neutralPos + turnAngle);
  setServo(rearLeftHip, neutralPos + turnAngle);
  delay(stepDelay);

  setServo(frontLeftKnee, neutralPos);
  setServo(rearLeftKnee, neutralPos);
  delay(stepDelay);

  setServo(frontRightHip, neutralPos - turnAngle);
  setServo(rearRightHip, neutralPos - turnAngle);
  delay(stepDelay);

  setServo(frontLeftKnee, neutralPos - liftAngle);
  setServo(rearLeftKnee, neutralPos - liftAngle);
  delay(stepDelay);

  setServo(frontLeftHip, neutralPos);
  setServo(rearLeftHip, neutralPos);
  delay(stepDelay);

  setServo(frontLeftKnee, neutralPos);
  setServo(rearLeftKnee, neutralPos);
  delay(stepDelay);

  setServo(frontRightHip, neutralPos);
  setServo(rearRightHip, neutralPos);
  delay(stepDelay);
}

void standUp() {
  setServo(frontLeftKnee, neutralPos + liftAngle);
  setServo(frontRightKnee, neutralPos + liftAngle);
  setServo(rearLeftKnee, neutralPos + liftAngle);
  setServo(rearRightKnee, neutralPos + liftAngle);
  delay(500);
}

void sitDown() {
  setServo(frontLeftKnee, neutralPos - liftAngle);
  setServo(frontRightKnee, neutralPos - liftAngle);
  setServo(rearLeftKnee, neutralPos - liftAngle);
  setServo(rearRightKnee, neutralPos - liftAngle);
  delay(500);
}
