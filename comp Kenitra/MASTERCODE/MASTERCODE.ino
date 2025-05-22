#include <SoftwareSerial.h>

#define BUTTON_FORWARD 8
#define BUTTON_BACKWARD 6
#define BUTTON_LEFT 7
#define BUTTON_RIGHT 5
#define BUTTON_MODE 4

#define time 100

// Potentiometer and Servo control
#define POT_PIN A0        // Analog pin for potentiometer
#define SERVO_DEADZONE 3  // Minimum change to send new value (adjust as needed)
#define SERVO_UPDATE_INTERVAL 50 // ms between servo updates

SoftwareSerial mySerial(2, 3); // RX, TX

// Variables for servo control
int lastServoValue = -1; // Stores last sent servo value
unsigned long lastServoUpdate = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(BUTTON_FORWARD, INPUT_PULLUP);
  pinMode(BUTTON_BACKWARD, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_MODE, INPUT_PULLUP);

  Serial.println("Master ready. Waiting for button press...");
}

void loop() {
  if (digitalRead(BUTTON_FORWARD) == LOW) {
    mySerial.write('F');
    Serial.println("Sent: Forward");
    delay(time);  // Debounce delay
  }else
  if (digitalRead(BUTTON_BACKWARD) == LOW) {
    mySerial.write('B');
    Serial.println("Sent: Backward");
    delay(time);
  }else
  if (digitalRead(BUTTON_LEFT) == LOW) {
    mySerial.write('L');
    Serial.println("Sent: Left");
    delay(time);
  }else
  if (digitalRead(BUTTON_RIGHT) == LOW) {
    mySerial.write('R');
    Serial.println("Sent: Right");
    delay(time);
  }else {
    mySerial.write('S');
  }
  
  if (digitalRead(BUTTON_MODE) == LOW) {
    mySerial.write('M');
    Serial.println("Sent: MODE");
    delay(300);
  }

}