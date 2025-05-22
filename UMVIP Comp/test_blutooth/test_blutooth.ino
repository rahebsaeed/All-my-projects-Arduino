#include <SoftwareSerial.h>

#define FORWARD 'F'
#define BACKWARD 'B'
#define LEFT 'L'
#define RIGHT 'R'
#define CIRCLE 'C'
#define CROSS 'X'
#define TRIANGLE 'T'
#define SQUARE 'S'
#define START 'A'
#define PAUSE 'P'

SoftwareSerial bluetooth(10, 11); // RX, TX (HC-06)

void setup() {
  Serial.begin(9600);   // For Serial Monitor (USB)
  bluetooth.begin(9600); // Default HC-06 baud rate
  Serial.println("Bluetooth Ready!");
}

void loop() {
  if (bluetooth.available()) {
    char command = bluetooth.read();
    Serial.print("Received: ");
    Serial.println(command);

    // Execute actions based on the command
    switch (command) {
      case FORWARD:
        Serial.println("Moving FORWARD");
        break;
      case BACKWARD:
        Serial.println("Moving BACKWARD");
        break;
      case LEFT:
        Serial.println("Turning LEFT");
        break;
      case RIGHT:
        Serial.println("Turning RIGHT");
        break;
      case CIRCLE:
        Serial.println("Circle Button Pressed");
        break;
      case CROSS:
        Serial.println("Cross Button Pressed");
        break;
      case TRIANGLE:
        Serial.println("Triangle Button Pressed");
        break;
      case SQUARE:
        Serial.println("Square Button Pressed");
        break;
      case START:
        Serial.println("Start Command");
        break;
      case PAUSE:
        Serial.println("Pause Command");
        break;
      default:
        Serial.println("Unknown Command");
    }
  }
}