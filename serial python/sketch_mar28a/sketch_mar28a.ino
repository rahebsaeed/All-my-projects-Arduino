#include <Servo.h>

Servo myServo;  // Create servo object

void setup() {
    Serial.begin(9600);  // Start serial communication
    myServo.attach(9);   // Servo connected to pin 9
    myServo.write(90);   // Start at center position
}

void loop() {
    if (Serial.available() > 0) {
        int angle = Serial.parseInt();  // Read angle from Python
        if (angle >= 0 && angle <= 180) {
            myServo.write(angle);  // Move servo
            Serial.print("Moving to: ");
            Serial.println(angle);
        }
    }
}