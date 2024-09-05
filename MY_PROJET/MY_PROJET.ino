#define button1 35
#include <LiquidCrystal.h>
#include <Keypad.h>
char code [4][3]={
  {'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
  char toutch[16];
  char i=0;
byte prochecolones[3]={26,27,28};
byte prochelignes[4]={22,23,24,25};
LiquidCrystal lcd(29,30,31,32,33,34);
Keypad clavier=Keypad( makeKeymap(code),prochelignes,prochecolones,4,3);
void setup() {
  pinMode(35,INPUT);
  lcd.begin(16,2);
 lcd.setCursor(2,0);
 lcd.print("Welcome to");
 lcd.setCursor(0,1);
 lcd.print("System security");
 delay(1000);
 lcd.clear();

}

void loop() {
  lcd.clear();
  while(digitalRead(button1)==0){
  while(toutch==0){toutch[i]=clavier.getKey();}
  
  lcd.print(toutch[i]);
 delay(200);
  lcd.print('*');

  i++;
  }
if(toutch[0]=='1'&&toutch[1]=='2'&&toutch[2]=='3'&&toutch[3]=='4'){
  lcd.clear();
  lcd.print("code correct");
}
else{
lcd.clear();
    lcd.print("code incorrect");}
    delay(1000);

}
