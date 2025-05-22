#include <Servo.h>

// Define leg structure
struct Leg {
  Servo servoSide;  // Abduction/adduction (side-to-side)
  Servo servoHip;   // Hip joint (forward-back)
  Servo servoKnee;  // Knee joint
  int sidePin, hipPin, kneePin;
  int sideCenter, hipCenter, kneeCenter;
};

// Stance parameters
float stanceWidth = 5.775;
float stanceLength = 6.35;
float stanceHeight = 9.0;

// Initialize legs
Leg leg1, leg2, leg3, leg4;

void setup() {
  Serial.begin(9600);
  
  // Configure legs (adjust pins and center positions to your setup)
  // Center positions are the servo values (0-180) when leg is in neutral position
  initLeg(&leg1, 11, 12, 13, 90, 90, 90);
  initLeg(&leg2, 8, 9, 10, 90, 90, 90);
  initLeg(&leg3, 2, 3, 4, 90, 90, 90);
  initLeg(&leg4, 5, 6, 7, 90, 90, 90);
  
  // Set initial stance
  setInitialPosition();
  
  Serial.println("Robot initialized");
}

void loop() {
  // Basic movement pattern example
  walkForward();
  delay(2000); // Walk for 2 seconds
  stopWalking();
  delay(1000); // Pause for 1 second
}

void initLeg(Leg* leg, int sidePin, int hipPin, int kneePin, 
             int sideCenter, int hipCenter, int kneeCenter) {
  leg->sidePin = sidePin;
  leg->hipPin = hipPin;
  leg->kneePin = kneePin;
  
  leg->servoSide.attach(sidePin);
  leg->servoHip.attach(hipPin);
  leg->servoKnee.attach(kneePin);
  
  leg->sideCenter = sideCenter;
  leg->hipCenter = hipCenter;
  leg->kneeCenter = kneeCenter;
}

void setInitialPosition() {
  // Move all legs to neutral position
  Serial.println("Setting initial position");
  
  for(int i=0; i<3; i++) {
    leg1.servoSide.write(leg1.sideCenter);
    leg1.servoHip.write(leg1.hipCenter);
    leg1.servoKnee.write(leg1.kneeCenter);
    
    leg2.servoSide.write(leg2.sideCenter);
    leg2.servoHip.write(leg2.hipCenter);
    leg2.servoKnee.write(leg2.kneeCenter);
    
    leg3.servoSide.write(leg3.sideCenter);
    leg3.servoHip.write(leg3.hipCenter);
    leg3.servoKnee.write(leg3.kneeCenter);
    
    leg4.servoSide.write(leg4.sideCenter);
    leg4.servoHip.write(leg4.hipCenter);
    leg4.servoKnee.write(leg4.kneeCenter);
    
    delay(100);
  }
}

void walkForward() {
  // Very basic walking pattern - you'll need to improve this
  Serial.println("Walking forward");
  
  // Lift leg 1 and 3
  moveLeg(&leg1, leg1.sideCenter, leg1.hipCenter-20, leg1.kneeCenter+30);
  moveLeg(&leg3, leg3.sideCenter, leg3.hipCenter-20, leg3.kneeCenter+30);
  delay(300);
  
  // Move them forward
  moveLeg(&leg1, leg1.sideCenter, leg1.hipCenter+20, leg1.kneeCenter+30);
  moveLeg(&leg3, leg3.sideCenter, leg3.hipCenter+20, leg3.kneeCenter+30);
  delay(300);
  
  // Lower them
  moveLeg(&leg1, leg1.sideCenter, leg1.hipCenter+20, leg1.kneeCenter);
  moveLeg(&leg3, leg3.sideCenter, leg3.hipCenter+20, leg3.kneeCenter);
  delay(300);
  
  // Lift leg 2 and 4
  moveLeg(&leg2, leg2.sideCenter, leg2.hipCenter-20, leg2.kneeCenter+30);
  moveLeg(&leg4, leg4.sideCenter, leg4.hipCenter-20, leg4.kneeCenter+30);
  delay(300);
  
  // Move them forward
  moveLeg(&leg2, leg2.sideCenter, leg2.hipCenter+20, leg2.kneeCenter+30);
  moveLeg(&leg4, leg4.sideCenter, leg4.hipCenter+20, leg4.kneeCenter+30);
  delay(300);
  
  // Lower them
  moveLeg(&leg2, leg2.sideCenter, leg2.hipCenter+20, leg2.kneeCenter);
  moveLeg(&leg4, leg4.sideCenter, leg4.hipCenter+20, leg4.kneeCenter);
  delay(300);
}

void stopWalking() {
  // Return to neutral position
  setInitialPosition();
  Serial.println("Stopped walking");
}

void moveLeg(Leg* leg, int sidePos, int hipPos, int kneePos) {
  // Constrain positions to prevent damage
  sidePos = constrain(sidePos, 0, 180);
  hipPos = constrain(hipPos, 0, 180);
  kneePos = constrain(kneePos, 0, 180);
  
  // Move servos
  leg->servoSide.write(sidePos);
  leg->servoHip.write(hipPos);
  leg->servoKnee.write(kneePos);
}