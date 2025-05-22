// IR Sensor Pins (Digital - changed from Analog)
const int irLeftFront = 12;    
const int irRightFront = 13;   
const int irLeftRear = 6;     
const int irRightRear = 11; 

// Ultrasonic Sensor Pins
const int trigPin = 8;
const int echoPin = 7;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irLeftFront, INPUT);
  pinMode(irRightFront, INPUT);
  pinMode(irRightRear, INPUT);
  pinMode(irLeftRear, INPUT);
}

void loop() {
  // IR Sensor Readings
  int lf = digitalRead(irLeftFront);
  int rf = digitalRead(irRightFront);
  int lr = digitalRead(irLeftRear);
  int rr = digitalRead(irRightRear);

  // Ultrasonic Reading
  long duration;
  float distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert to cm

  // Print all values
  Serial.print("LF: "); Serial.print(lf);
  Serial.print(" RF: "); Serial.print(rf);
  Serial.print(" LR: "); Serial.print(lr);
  Serial.print(" RR: "); Serial.print(rr);
  Serial.print(" | Ultrasonic: "); Serial.print(distance); Serial.println(" cm");

  delay(200);
}
