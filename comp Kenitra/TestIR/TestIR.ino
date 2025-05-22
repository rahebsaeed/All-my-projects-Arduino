#define IR_RIGHT 6         
#define IR_LEFT 7          

void setup() {
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_LEFT, INPUT);
  
  Serial.begin(9600);
  Serial.println("IR Sensor Test Started");
}

void loop() {
  int rightState = digitalRead(IR_RIGHT);
  int leftState = digitalRead(IR_LEFT);

  Serial.print("IR_LEFT: ");
  Serial.print(leftState == LOW ? "Detected\t" : "Clear\t");
  
  Serial.print("IR_RIGHT: ");
  Serial.println(rightState == LOW ? "Detected" : "Clear");

  delay(200);  // Refresh every 200ms
}
