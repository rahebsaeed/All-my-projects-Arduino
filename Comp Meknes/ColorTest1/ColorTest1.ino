#include <NewPing.h>
#define TRIG_F 11
#define ECHO_F 12
// Motor Driver Pins (L298N)
#define M_LEFT_IN1  2
#define M_LEFT_IN2  3
#define M_RIGHT_IN3 4
#define M_RIGHT_IN4 5
#define M_LEFT_ENA  9 
#define M_RIGHT_ENB 10


#define BASE_SPEED 80
#define MAX_SPEED 120
#define TURN_SPEED 140


int s0 = A0;
int s1 = A1;
int s2 = A2;
int s3 = A3;
int outPin = 6;

NewPing sonarFront(TRIG_F, ECHO_F, 200);

void setup() {
    // Motor control pins
  pinMode(M_LEFT_IN1, OUTPUT);
  pinMode(M_LEFT_IN2, OUTPUT);
  pinMode(M_RIGHT_IN3, OUTPUT);
  pinMode(M_RIGHT_IN4, OUTPUT);
  pinMode(M_LEFT_ENA, OUTPUT);
  pinMode(M_RIGHT_ENB, OUTPUT);

  Serial.begin(9600);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(outPin, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
}
int i =0;
int Distance_F;

int readColorFrequency(int s2Val, int s3Val) {
  digitalWrite(s2, s2Val);
  digitalWrite(s3, s3Val);
  delay(100);
  return pulseIn(outPin, LOW);
}

void loop() {
tstColors();
}

void tstColors(){
  int redFreq = readColorFrequency(LOW, LOW);     // Rouge
  int blueFreq = readColorFrequency(LOW, HIGH);   // Bleu
  int greenFreq = readColorFrequency(HIGH, HIGH); // Vert

  String colorName = "Aucune couleur détectée";

  // Seuil : si tous sont au-dessus de 200, alors rien n’est détecté
    // Plus la fréquence est basse, plus la couleur est dominante
    if (redFreq >10 && redFreq < 45 && greenFreq >80 && greenFreq < 130 && blueFreq <110 && blueFreq >60 ) {
      turnRight(TURN_SPEED);      
      colorName = "Rouge";

    }
    else if (greenFreq < redFreq && greenFreq < blueFreq) {
      moveForward(BASE_SPEED);
      i=1;
      colorName = "Vert";
    }
    else if (blueFreq < redFreq && blueFreq < greenFreq) {
      colorName = "Bleu";
      if(i == 1){
        breakMotors();
      }
      i=0;
      stopMotors();

    }

  Serial.print("R: ");
  Serial.print(redFreq);
  Serial.print("  G: ");
  Serial.print(greenFreq);
  Serial.print("  B: ");
  Serial.print(blueFreq);
  Serial.print("  → ");
  Serial.println(colorName);

  delay(20);
}

void distance(){
    Distance_F =  sonarFront.ping_cm();
  if(Distance_F < 20 ){
    if(i == 1){
      breakMotors();
    }
    i=0;
    stopMotors();
  }else{
    moveForward(MAX_SPEED);
    i =1;
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
void breakMotors(){
  moveBack(MAX_SPEED);
  delay(50);
  stopMotors();
  delay(50);
  moveBack(MAX_SPEED);
  delay(50);
  stopMotors();
  delay(50);

}