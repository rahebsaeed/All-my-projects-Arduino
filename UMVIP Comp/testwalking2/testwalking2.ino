#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NewPing.h>
#include <SoftwareSerial.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();SoftwareSerial bluetooth(2, 3);

#define SERVOMIN  150 // PWM pour 0°
#define SERVOMAX  600 // PWM pour 180°

#define TRIG_PIN 13
#define ECHO_PIN 12
#define MAX_DISTANCE 200
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

const int FLHipChan = 0;
const int FLKneeChan = 2;
const int FRHipChan = 8;
const int FRKneeChan = 10;
const int RLHipChan = 4;
const int RLKneeChan = 6;
const int RRHipChan = 12;
const int RRKneeChan = 14;
//left
int FLH_initial = 135;
int FLK_initial = 30;
int RLH_initial = 45;
int RLK_initial = 150;
//right
int FRH_initial = 45;
int RRH_initial = 135;
int RRK_initial = 30;
int FRK_initial = 150;

int liftAngle = 30;
int stepSize = 40;
int turnAngle = 30;
int stepDelay = 40;
#define tunnelStepDelay 100
#define tunnelLiftAngle 45

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50); // 50 Hz pour les servos
  resetToInitialPositions();
  Serial.println("System Ready");
}

void loop() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 1000) {
    lastSend = millis();
    int distance = sonar.ping_cm();
    bluetooth.print("DIST:");
    bluetooth.println(distance);
    Serial.print("Distance: ");
    Serial.println(distance);
  }

  if (bluetooth.available()) {
    char command = bluetooth.read();
    executeCommand(command);
  }
}

void executeCommand(char cmd) {
  switch(cmd) {
    case 'F': moveForward(); break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft(); break;
    case 'R': turnRight(); break;
    case 'S': resetToInitialPositions(); break;
    case 'T': navigateTunnel(); break;
    case 'U': stairs(); break;
    case 'V': Speed(); break;
    case 'P': phase2(); break;
  }
}

void setServoAngle(uint8_t numServo, uint8_t angle) {
  uint16_t pwmVal = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(numServo, 0, pwmVal);
}

void resetToInitialPositions() {
  setServoAngle(FLHipChan, FLH_initial);
  setServoAngle(FLKneeChan, FLK_initial);
  setServoAngle(FRHipChan, FRH_initial);
  setServoAngle(FRKneeChan, FRK_initial);
  setServoAngle(RLHipChan, RLH_initial);
  setServoAngle(RLKneeChan, RLK_initial);
  setServoAngle(RRHipChan, RRH_initial);
  setServoAngle(RRKneeChan, RRK_initial);
  delay(500);
}

void resetToInitialPositionstairs() {
  setServoAngle(FLHipChan, FLH_initial);
  setServoAngle(FLKneeChan, FLK_initial);
  setServoAngle(FRHipChan, FRH_initial);
  setServoAngle(FRKneeChan, FRK_initial);
  setServoAngle(RLHipChan, RLH_initial);
  setServoAngle(RLKneeChan, RLK_initial);
  setServoAngle(RRHipChan, RRH_initial);
  setServoAngle(RRKneeChan, RRK_initial);
}

void moveForward() {
  setServoAngle(FLKneeChan, FLK_initial - liftAngle);
  setServoAngle(RRKneeChan, RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServoAngle(FLHipChan, FLH_initial + stepSize);
  setServoAngle(RRHipChan, RRH_initial + stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FLKneeChan, FLK_initial);
  setServoAngle(RRKneeChan, RRK_initial);
  delay(stepDelay/2);
  
  setServoAngle(FRHipChan, FRH_initial - stepSize/2);
  setServoAngle(RLHipChan, RLH_initial - stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FRKneeChan, FRK_initial - liftAngle);
  setServoAngle(RLKneeChan, RLK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServoAngle(FRHipChan, FRH_initial + stepSize);
  setServoAngle(RLHipChan, RLH_initial + stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FRKneeChan, FRK_initial);
  setServoAngle(RLKneeChan, RLK_initial);
  delay(stepDelay/2);
  
  setServoAngle(FLHipChan, FLH_initial - stepSize/2);
  setServoAngle(RRHipChan, RRH_initial - stepSize/2);
  delay(stepDelay);
  
  resetToInitialPositions();
  delay(stepDelay);
}

void moveBackward() {
  setServoAngle(FRKneeChan, FRK_initial - liftAngle);
  setServoAngle(RLKneeChan, RLK_initial - liftAngle);
  delay(stepDelay);
  
  setServoAngle(FRHipChan, FRH_initial - stepSize);
  setServoAngle(RLHipChan, RLH_initial - stepSize);
  delay(stepDelay);
  
  setServoAngle(FRKneeChan, FRK_initial);
  setServoAngle(RLKneeChan, RLK_initial);
  delay(stepDelay);
  
  setServoAngle(FLHipChan, FLH_initial + stepSize/2);
  setServoAngle(RRHipChan, RRH_initial + stepSize/2);
  delay(stepDelay*2);
  
  setServoAngle(FLKneeChan, FLK_initial - liftAngle);
  setServoAngle(RRKneeChan, RRK_initial - liftAngle);
  delay(stepDelay);
  
  setServoAngle(FLHipChan, FLH_initial - stepSize);
  setServoAngle(RRHipChan, RRH_initial - stepSize);
  delay(stepDelay);
  
  setServoAngle(FLKneeChan, FLK_initial);
  setServoAngle(RRKneeChan, RRK_initial);
  delay(stepDelay);
  
  setServoAngle(FRHipChan, FRH_initial + stepSize/2);
  setServoAngle(RLHipChan, RLH_initial + stepSize/2);
  delay(stepDelay*2);
  
  resetToInitialPositions();
}

void turnRight() {
  setServoAngle(FLKneeChan, FLK_initial - liftAngle);
  setServoAngle(RRKneeChan, RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServoAngle(FLHipChan, FLH_initial - stepSize);
  setServoAngle(RRHipChan, RRH_initial - stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FLKneeChan, FLK_initial);
  setServoAngle(RRKneeChan, RRK_initial);
  delay(stepDelay/2);
  
  setServoAngle(FRHipChan, FRH_initial + stepSize/2);
  setServoAngle(RLHipChan, RLH_initial + stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FRKneeChan, FRK_initial - liftAngle);
  setServoAngle(RLKneeChan, RLK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServoAngle(FRHipChan, FRH_initial - stepSize);
  setServoAngle(RLHipChan, RLH_initial - stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FRKneeChan, FRK_initial);
  setServoAngle(RLKneeChan, RLK_initial);
  delay(stepDelay/2);
  
  setServoAngle(FLHipChan, FLH_initial + stepSize/2);
  setServoAngle(RRHipChan, RRH_initial + stepSize/2);
  delay(stepDelay);
  
  resetToInitialPositions();
  delay(stepDelay);
}

void turnLeft() {
  setServoAngle(FRKneeChan, FRK_initial - liftAngle);
  setServoAngle(RLKneeChan, RLK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServoAngle(FRHipChan, FRH_initial - stepSize);
  setServoAngle(RLHipChan, RLH_initial - stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FRKneeChan, FRK_initial);
  setServoAngle(RLKneeChan, RLK_initial);
  delay(stepDelay/2);
  
  setServoAngle(FLHipChan, FLH_initial + stepSize/2);
  setServoAngle(RRHipChan, RRH_initial + stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FLKneeChan, FLK_initial - liftAngle);
  setServoAngle(RRKneeChan, RRK_initial - liftAngle);
  delay(stepDelay/2);
  
  setServoAngle(FLHipChan, FLH_initial - stepSize);
  setServoAngle(RRHipChan, RRH_initial - stepSize/2);
  delay(stepDelay);
  
  setServoAngle(FLKneeChan, FLK_initial);
  setServoAngle(RRKneeChan, RRK_initial);
  delay(stepDelay/2);
  
  setServoAngle(FRHipChan, FRH_initial + stepSize/2);
  setServoAngle(RLHipChan, RLH_initial + stepSize/2);
  delay(stepDelay);
  
  resetToInitialPositions();
  delay(stepDelay);
}

void setupTunnelPosition() {
  setServoAngle(FRHipChan, 90);
  setServoAngle(RRHipChan, 90);
  setServoAngle(FRKneeChan, 180);
  setServoAngle(RRKneeChan, 180);
  setServoAngle(FLHipChan, 90);
  setServoAngle(RLHipChan, 90);
  setServoAngle(FLKneeChan, 0);
  setServoAngle(RLKneeChan, 0);
  delay(100);
}

void moveForwardInTunnel() {
  setServoAngle(FLKneeChan, 0 + tunnelLiftAngle);
  setServoAngle(RRKneeChan, 180 - tunnelLiftAngle);
  delay(tunnelStepDelay);
  
  setServoAngle(FRKneeChan, 180 - tunnelLiftAngle);
  setServoAngle(RLKneeChan, 0 + tunnelLiftAngle);
  delay(tunnelStepDelay*2);
  
  setServoAngle(FLKneeChan, 0);
  setServoAngle(RRKneeChan, 180);
  delay(tunnelStepDelay);
  
  setServoAngle(FRKneeChan, 180 - tunnelLiftAngle);
  setServoAngle(RLKneeChan, 0 + tunnelLiftAngle);
  delay(tunnelStepDelay);
  
  setServoAngle(FLKneeChan, 0 + tunnelLiftAngle);
  setServoAngle(RRKneeChan, 180 - tunnelLiftAngle);
  delay(tunnelStepDelay*2);
  
  setServoAngle(FRKneeChan, 180);
  setServoAngle(RLKneeChan, 0);
  delay(tunnelStepDelay);
}

void navigateTunnel() {
  setupTunnelPosition();
  for (int i = 0; i < 4; i++) {
    moveForwardInTunnel();
  }
}

void Speed() {  
  resetToInitialPositionstairs();
  delay(300);
  setServoAngle(FLKneeChan, FLK_initial - liftAngle);
  setServoAngle(FRHipChan, FRH_initial);
  setServoAngle(RLHipChan, RLH_initial);
  setServoAngle(RRHipChan, RRH_initial);
  delay(300);
  moveForward();
}

void stairs(){
  setServoAngle(FLHipChan, 90);
  setServoAngle(FLKneeChan, 60);
  setServoAngle(FRHipChan, 180);
  setServoAngle(FRKneeChan, 135);
  setServoAngle(RLHipChan, 90);
  setServoAngle(RLKneeChan, 0);
  setServoAngle(RRHipChan, 90);
  setServoAngle(RRKneeChan, 180);
  delay(300);
  setServoAngle(FRKneeChan, 45);
  
  setServoAngle(FLHipChan, 0);
  setServoAngle(FLKneeChan, 45);
  setServoAngle(FRHipChan, 90);
  setServoAngle(FRKneeChan, 135);
  setServoAngle(RLHipChan, 90);
  setServoAngle(RLKneeChan, 0);
  setServoAngle(RRHipChan, 90);
  setServoAngle(RRKneeChan, 180);
  delay(300);
  moveForward();
  
  setServoAngle(FLHipChan, 90);
  setServoAngle(FLKneeChan, 0);
  setServoAngle(FRHipChan, 90);
  setServoAngle(FRKneeChan, 180);
  setServoAngle(RLHipChan, 90);
  setServoAngle(RLKneeChan, 135);
  setServoAngle(RRHipChan, 90);
  setServoAngle(RRKneeChan, 45);
  delay(300);
}

void phase2(){
  setServoAngle(FLHipChan, FLH_initial);
  setServoAngle(FLKneeChan, FLK_initial);
  setServoAngle(FRHipChan, FRH_initial);
  setServoAngle(FRKneeChan, FRK_initial);
  setServoAngle(RLHipChan, 90);
  setServoAngle(RLKneeChan, RLK_initial);
  setServoAngle(RRHipChan, RRH_initial);
  setServoAngle(RRKneeChan, RRK_initial);
  delay(200);
  setServoAngle(FRHipChan, 180);
  setServoAngle(FRKneeChan, 135);
  delay(200);
}