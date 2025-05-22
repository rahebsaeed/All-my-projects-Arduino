//   Case4      O  Obstcle under Ultrasonic Front                                                                  
//          |_________________________________________________                                                                         
//          |                               O                                          
//          |                            -->          O                                       
//          |_____________           ________________________                                                             
//                       |          |                                                  
//                       |          |                                                  
//                       |          |                                                  
//                       |  Start   |                                                 
//                                                                                   
#include <NewPing.h>

// --- Pin Definitions ---
// Motor Driver Pins (L298N)
#define M_LEFT_IN1  2
#define M_LEFT_IN2  3
#define M_RIGHT_IN3 4
#define M_RIGHT_IN4 5
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10

// Ultrasonic Sensor Pins
#define TRIG_F 6
#define ECHO_F 7
#define TRIG_R 8
#define ECHO_R A2
#define TRIG_L A3
#define ECHO_L 11

// Infrared Sensor Pins
#define INF_R A0 // Right IR
#define INF_L A5 // Left IR
#define INF_B A1 // Back IR

// Color Sensor Pins
#define S2 12
#define S3 13
#define OUT A4

// --- Constants ---
#define BASE_SPEED 125
#define MAX_SPEED 220
#define TURN_SPEED 180
#define FRONT_SPEED_THRESHOLD 100.0 // cm
#define MIN_OBSTACLE_DISTANCE 30.0 // cm
#define MIN_SIDES_DISTANCE 10.0 // cm

#define IR_OBSTACLE_STATE LOW // LOW means obstacle detected
#define RED_THRESHOLD 100 // Adjust based on your sensor
#define BACKUP_SPEED 140
#define TURN_SPEED 255
#define OBSTACLE_MAX 500
#define OBSTACLE_MIN 200
#define WIDE 40



NewPing sonarFront(TRIG_F, ECHO_F, 200);
NewPing sonarRight(TRIG_R, ECHO_R, 200);
NewPing sonarLeft(TRIG_L, ECHO_L, 200);
int Distance_F;
int Distance_R;
int Distance_L;
short i = 0; // to count 

void setup() {
  // Motor control pins
  pinMode(M_LEFT_IN1, OUTPUT);
  pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT);
  pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT);
  pinMode(M_RIGHT_ENB, OUTPUT);
  
  // IR sensor pins
  pinMode(INF_R, INPUT);
  pinMode(INF_L, INPUT);
  pinMode(INF_B, INPUT);
  
  // Color sensor pins
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  
  Serial.begin(9600);
}
void loop() {
  

  
  // Read sensors with simple filtering
  Distance_F =  sonarFront.ping_cm();
  Distance_R = sonarRight.ping_cm();
  Distance_L = sonarLeft.ping_cm();
  
  int irRight = analogRead(INF_R);
  int irLeft = analogRead(INF_L);
  int irBack = analogRead(INF_B);
  if(Distance_F < MIN_OBSTACLE_DISTANCE ){
    stopMotors();

    if(Distance_R > Distance_L){
      while(sonarFront.ping_cm() < MIN_OBSTACLE_DISTANCE){
        turnLeft(TURN_SPEED);
      }
      stopMotors();
      
    }else{
      while(sonarFront.ping_cm() < MIN_OBSTACLE_DISTANCE){
        turnRight(TURN_SPEED);
      }
      stopMotors();
    }

  }else {
    if(Distance_F > FRONT_SPEED_THRESHOLD ){
      moveForward(MAX_SPEED);
    } else {
      moveForward(BASE_SPEED);
    }

    if(irLeft < OBSTACLE_MAX and irLeft > OBSTACLE_MIN && irRight < OBSTACLE_MAX && irRight > OBSTACLE_MIN && sonarFront.ping_cm() > MIN_OBSTACLE_DISTANCE || (Distance_R < MIN_OBSTACLE_DISTANCE && Distance_R > MIN_SIDES_DISTANCE && Distance_L < MIN_OBSTACLE_DISTANCE && Distance_L > MIN_SIDES_DISTANCE)){
      while(analogRead(INF_R) > OBSTACLE_MIN && analogRead(INF_L) > OBSTACLE_MIN && sonarFront.ping_cm() > MIN_OBSTACLE_DISTANCE){
        moveForward(BASE_SPEED);
      }
      stopMotors();
    }else if(irLeft < OBSTACLE_MAX and irLeft > OBSTACLE_MIN ){
      while(analogRead(INF_R) > OBSTACLE_MIN && i < 200 ){
        turnRight(TURN_SPEED);
        delay(10);
        i +=10;
      }
      stopMotors();
      i=0;
    }else if(irLeft < OBSTACLE_MIN){
      while(sonarFront.ping_cm() > (Distance_F + 30) && analogRead(INF_B) > OBSTACLE_MAX ){
        moveBack(BACKUP_SPEED);
      }      
      stopMotors();
      while(analogRead(INF_R) >OBSTACLE_MIN && i < 50){
        turnRight(TURN_SPEED);
        delay(10);
        i +=10;
      }
      stopMotors();
      i=0;
    }else if(irRight < OBSTACLE_MAX && irRight > OBSTACLE_MIN){
      while(sonarFront.ping_cm() > (Distance_F + 30) && analogRead(INF_B) > OBSTACLE_MAX){
        moveBack(BACKUP_SPEED);
      }
      stopMotors();
      while(analogRead(INF_L) > OBSTACLE_MIN && i < 50){
        turnLeft(TURN_SPEED);
        delay(10);
        i +=10;
      }
      stopMotors();
      i=0;    
    }else if(irRight < OBSTACLE_MIN){
      while(sonarFront.ping_cm() > (Distance_F + 50) && analogRead(INF_B) > OBSTACLE_MAX){
        moveBack(BACKUP_SPEED);
      }
      stopMotors();
      while(analogRead(INF_L) > OBSTACLE_MIN && i < 50){
        turnLeft(TURN_SPEED);
        delay(10);
        i +=10;
      }
      stopMotors();
      i=0;         
    }
  }
}


// --- Motor Control Functions ---
void stopMotors() {
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, LOW);
  digitalWrite(M_RIGHT_IN3, LOW);
  digitalWrite(M_RIGHT_IN4, LOW);
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

void moveBack(int speed) {
  analogWrite(M_LEFT_ENA, speed);
  analogWrite(M_RIGHT_ENB, speed);
  digitalWrite(M_LEFT_IN1, LOW);
  digitalWrite(M_LEFT_IN2, HIGH);
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

bool detectRed() {
  // Set to read red component
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int red = pulseIn(OUT, LOW);

  // Set to read green component
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int green = pulseIn(OUT, LOW);

  // Set to read blue component
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int blue = pulseIn(OUT, LOW);

  // Red detection logic
  return (red < green && red < blue && red < RED_THRESHOLD);
}