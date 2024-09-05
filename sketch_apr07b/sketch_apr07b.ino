#include <LiquidCrystal.h>
LiquidCrystal lcd(11,10,9,8,7,6);
void setup() {
 lcd.begin(16,2);
pinMode(13,OUTPUT);
pinMode(12,OUTPUT);
lcd.clear();
}

void loop() {
  moteur(13,12,1,0);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("sens direct");
  delay(60000);
  moteur(13,12,0,0);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("moteur arret");
  delay(30000);
  moteur(13,12,0,1);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("sens inverse");
  delay(60000);
  moteur(13,12,0,0);
  lcd.clear();
  delay(10);
  lcd.setCursor(2,0);
  lcd.print("moteur arret");
  delay(30000);


}
void moteur(int pin1,int pin2,int s1,int s2){
  digitalWrite(pin1,s1);
  digitalWrite(pin2,s2);
}
