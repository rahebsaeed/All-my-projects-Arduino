#include <Servo.h>
int angle;
Servo motor;
void setup() {
  // put your setup code here, to run once:

motor.attach(9);

}

void loop() {
  // put your main code here, to run repeatedly:
angle=random(0,180);
motor.write(angle);
delay(2000);

}
