#define button1 35
#define buttonDelete 36
#define buzzer 37
#define securite 38
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>
int how=0;
char code [4][3]={
  {'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
  char toutch[16];
  int i=0,n,w=0;
  int alarme=0;
byte prochecolones[3]={26,27,28};
byte prochelignes[4]={22,23,24,25};
LiquidCrystal lcd(29,30,31,32,33,34);
Keypad clavier=Keypad( makeKeymap(code),prochelignes,prochecolones,4,3);
char motdepasse[15];
char again[15];

void setup() {

//****to reset password**** marche arduino sans commenter la fonction (for ) en bas et apres commenter la fonction dans programme
for(int m=0;m<8;m++)EEPROM.write(m,NO_KEY);
   
  for(int t=39;t<45;t++)pinMode(t,OUTPUT);
  pinMode(button1,INPUT);
  pinMode(buttonDelete,INPUT);
  pinMode(buzzer,OUTPUT);
pinMode(securite,INPUT);  
pinMode(46,INPUT);
pinMode(45,INPUT);
  lcd.begin(32,2);
 lcd.setCursor(11,0);
 lcd.print("Welcome to");
 lcd.setCursor(9,1);
 lcd.print("System security");
 delay(1000);
lcd.clear();
 for(int m=0;m<8;m++){
  motdepasse[m]= EEPROM.read(m);
 }
 if(digitalRead(45)==1){
lcd.setCursor(11,0);
lcd.print("click # to");
lcd.setCursor(8,1);
lcd.print("change password");
delay(1300);}
for(int m=0;m<8;m++){
  motdepasse[m]= EEPROM.read(m);
 }
   if(motdepasse[0]==NO_KEY&&motdepasse[1]==NO_KEY&&motdepasse[2]==NO_KEY&&motdepasse[3]==NO_KEY&&motdepasse[4]==NO_KEY&&motdepasse[5]==NO_KEY&&motdepasse[7]==NO_KEY&&motdepasse[7]==NO_KEY)
   {password();}
   
}




void loop() {firest:
  if(digitalRead(45)==1){
      debit:for(int m=0;m<8;m++)toutch[m]=0;
      lcd.display();
      digitalWrite(40,0);
      i=0;digitalWrite(buzzer,0);
    lcd.clear();  
    delay(10);
    lcd.setCursor(4,0);
       lcd.print("inter the passoword :"); 
       /*lcd.setCursor(0,1);
       lcd.print(motdepasse[0]); 
       lcd.print(motdepasse[1]);
       lcd.print(motdepasse[2]);
       lcd.print(motdepasse[3]);*/

       while(digitalRead(button1)==0){
           toutch[i]=0;
           
             while(toutch[i]==NO_KEY){toutch[i]=clavier.getKey();
                                        if(digitalRead(button1)!=0&&i>=3 )goto end;
                                        if(digitalRead(button1)!=0&&i<3 ){
                                          lcd.clear();
                                          lcd.setCursor(6, 0);
                                          lcd.print("the password must be");
                                          lcd.setCursor(7, 1);
                                          lcd.print("more than 3 digits");
                                          delay(1400);
                                          goto debit;
                                        }
                                        if(digitalRead(buttonDelete)==1&&i>0)goto debit;
                                        if(digitalRead(45)==0)goto firest;
                                        }digitalWrite(buzzer,1);
    delay(25);    digitalWrite(buzzer,0);

     if(toutch[i]=='#'){
                    password();
                    lcd.clear();
                    delay(10);
                    goto debit;
    lcd.setCursor(i+12,1);        
   lcd.print(toutch[i]);
   
    delay(150); 
                    }
    lcd.setCursor(i+12,1);
   lcd.print('*');
    end:
   i++;
  }
if(toutch[0]==motdepasse[0]&&toutch[1]==motdepasse[1]&&toutch[2]==motdepasse[2]&&toutch[3]==motdepasse[3]&&toutch[4]==motdepasse[4]&&toutch[5]==motdepasse[5]&&toutch[6]==motdepasse[6]&&toutch[7]==motdepasse[7]){
lcd.clear();
lcd.setCursor(6,0);
lcd.print("the password correct");
digitalWrite(39, 1);
alarme=0;
     digitalWrite(41, 1);
    digitalWrite(42, 0);  
digitalWrite(43, 0);
    digitalWrite(44, 1);
      delay(1400);
      digitalWrite(41, 0);
    digitalWrite(42, 0);  
digitalWrite(43, 0);
    digitalWrite(44, 0);
    delay(2000);
    digitalWrite(39, 0);
    for(int m=0;m<140;m++){
      digitalWrite(41, 0);
    digitalWrite(42, 1);  
digitalWrite(43, 1);
    digitalWrite(44, 0);
      delay(10);}
       digitalWrite(41, 0);
    digitalWrite(42, 0);  
digitalWrite(43, 0);
    digitalWrite(44, 0);
}
else{
     lcd.clear();
     delay(10);
     lcd.setCursor(5,0);
    lcd.print("the passowrd incorrect");
    digitalWrite(40, 1);
    alarme++;}
    if(alarme==3){alarme=0;lcd.clear();

    lcd.setCursor(10, 0);
    lcd.print("DANGER......");
      while(digitalRead(securite)==0){
       for(int m=0;m<30;m++) 
       {lcd.display();
       digitalWrite(40, 1);
      digitalWrite(buzzer,1);
      if(digitalRead(securite)==1)goto debit;
      delay(10);}
    
    for(int m=0;m<30;m++) 
       {lcd.noDisplay();
digitalWrite(buzzer,0);
digitalWrite(40, 0);
if(digitalRead(securite)==1)goto debit;
    delay(10); }   

    }
    }fin:
    delay(1000);
    digitalWrite(39, 0);digitalWrite(40, 0);
  }
  else{
    lcd.noDisplay();
    if(digitalRead(46)==1){
      open:
      if(how==0&&w==0)n=140;
      else if(how==0&&w==1) n=how;
      else n=how;
      for(int m=0;m<n;m++){
     digitalWrite(41, 1);
    digitalWrite(42, 0);  
digitalWrite(43, 0);
    digitalWrite(44, 1);
      delay(10);}
      digitalWrite(41, 0);
    digitalWrite(42, 0);  
digitalWrite(43, 0);
    digitalWrite(44, 0);
    delay(2000);
    for(how=0;how<140;how++){
      w=1;if(digitalRead(46)==1)goto open;
      digitalWrite(41, 0);
    digitalWrite(42, 1);  
digitalWrite(43, 1);
    digitalWrite(44, 0);
      delay(10);
      }
      how=0;w=0;
      digitalWrite(41, 0);
    digitalWrite(42, 0);  
digitalWrite(43, 0);
    digitalWrite(44, 0);
     
    }
    else{
     digitalWrite(41, 0);
    digitalWrite(42, 0);  
digitalWrite(43, 0);
    digitalWrite(44, 0);
    

      }
    
  }

}







void password(){
 


if(motdepasse[0]!=NO_KEY&&motdepasse[1]!=NO_KEY&&motdepasse[2]!=NO_KEY){
lcd.clear();
delay(10);
lcd.setCursor(7, 0);
lcd.print("to change password");
lcd.setCursor(13, 1);
lcd.print("click 1");
for(int m=0;m<8;m++)toutch[m]=0;

toutch[i]=0;
  while(toutch[i]==NO_KEY){toutch[i]=clavier.getKey();}
digitalWrite(buzzer,1);
    delay(25);
    digitalWrite(buzzer,0);
if(toutch[i]=='1'){
   re:
   for(int m=0;m<8;m++)toutch[m]=0;
   i=0;
  if(alarme==3){alarme=0;lcd.clear();
    lcd.setCursor(10, 0);
    lcd.print("DANGER......");
      while(digitalRead(securite)==0){
       for(int m=0;m<30;m++) 
       {lcd.display();
      digitalWrite(buzzer,1);
      digitalWrite(40, 1);
      if(digitalRead(securite)==1)goto vf;
      delay(10);}
    
    for(int m=0;m<30;m++) 
       {lcd.noDisplay();
             digitalWrite(40, 0);

digitalWrite(buzzer,0);
if(digitalRead(securite)==1)goto vf;
    delay(10); }   

    }
  }
   mn:
   for(int m=0;m<8;m++)toutch[m]=0;
   i=0;
    lcd.clear();
    delay(10);
    lcd.setCursor(6,0);
lcd.print("inter old password :");
lcd.setCursor(12,1);
while(digitalRead(button1)==0){
toutch[i]=0;
  while(toutch[i]==NO_KEY){toutch[i]=clavier.getKey();
                           if(digitalRead(button1)!=0&&i>=3)goto fn;
                            if(digitalRead(button1)!=0&&i<3 ){
                                          lcd.clear();
                                          lcd.setCursor(6, 0);
                                          lcd.print("the password must be");
                                          lcd.setCursor(7, 1);
                                          lcd.print("more than 3 digits");
                                          delay(1400);
                                          goto mn;
                                        }
                           if(digitalRead(buttonDelete)==1&&i>0)goto mn;
                           }
digitalWrite(buzzer,1);
    delay(25);
    digitalWrite(buzzer,0);
     if(toutch[i]=='#'){
                    goto vf;
                    } 
  lcd.setCursor(i+12,1);                                       
  lcd.print(toutch[i]);
  
 delay(150);
 lcd.setCursor(i+12,1);
  lcd.print('*');   
 fn:                       
i++;
}

    if(toutch[0]==motdepasse[0]&&toutch[1]==motdepasse[1]&&toutch[2]==motdepasse[2]&&toutch[3]==motdepasse[3]&&toutch[4]==motdepasse[4]&&toutch[5]==motdepasse[5]&&toutch[6]==motdepasse[6]&&toutch[7]==motdepasse[7]){
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("the password correct");
        digitalWrite(39, 1);
        delay(500);
        digitalWrite(39, 0);
} 
  else{
lcd.clear();
delay(10);
lcd.setCursor(5,0);
    lcd.print("the password incorrect");
    digitalWrite(40, 1);
    alarme++;
    delay(500); digitalWrite(40, 0);
    goto re;}
   delay(500);}
   else{lcd.clear(); goto vf;}
}
   ll:
  for(int m=0;m<8;m++)toutch[m]=0;
   lcd.clear();
   delay(10);
   lcd.setCursor(7,0);
   lcd.print("inter new passowrd");
      i=0;
      while(digitalRead(button1)==0){
    toutch[i]=0;
  while(toutch[i]==NO_KEY){toutch[i]=clavier.getKey();
                           if(digitalRead(button1)!=0&&i>=3)goto sn;
                            if(digitalRead(button1)!=0&&i<3 ){
                                          lcd.clear();
                                          lcd.setCursor(6, 0);
                                          lcd.print("the password must be");
                                          lcd.setCursor(7, 1);
                                          lcd.print("more than 3 digits");
                                          delay(1400);
                                          goto ll;
                                        }
                           if(digitalRead(buttonDelete)==1&&i>0)goto ll;
                           }
                              lcd.setCursor(i+12,1);
                              digitalWrite(buzzer,1);
                          delay(25);
                          digitalWrite(buzzer,0);
                             lcd.print(toutch[i]);
                             delay(150);if(toutch[i]=='#')goto vf;
                              lcd.setCursor(i+12,1);
                              lcd.print('*');
                          
                           i++;
                           }  
    sn:  
    i=0;
  for(int m=0;m<8;m++)again[m]=0;

   lcd.clear();
   delay(10);
   lcd.setCursor(10,0);
   lcd.print("inter again");
     
      i=0;
      delay(500);
      while(digitalRead(button1)==0){
    again[i]=0;
   while(again[i]==NO_KEY){again[i]=clavier.getKey();
                           if(digitalRead(button1)!=0&&i>=3)goto ff;
                           if(digitalRead(button1)!=0&&i<3 ){
                                          lcd.clear();
                                          lcd.setCursor(6, 0);
                                          lcd.print("the password must be");
                                          lcd.setCursor(7, 1);
                                          lcd.print("more than 3 digits");
                                          delay(1400);
                                          goto sn;
                                        }                           
                           if(digitalRead(buttonDelete)==1&&i>0)goto sn;
                           }
   digitalWrite(buzzer,1);
    delay(25);
    digitalWrite(buzzer,0);                        
   if(toutch[i]=='#')goto vf;                        
   lcd.setCursor(i+12,1);   
   lcd.print(again[i]);
   delay(150);
   lcd.setCursor(i+12,1);
   lcd.print('*');
                           ff:
                           i++;}
   if(again[0]==toutch[0]&&again[1]==toutch[1]&&again[2]==toutch[2]&&toutch[3]==again[3]&&toutch[4]==again[4]&&toutch[5]==again[5]&&toutch[6]==again[6]&&toutch[7]==again[7]){
   lcd.clear();
   for (int d = 0 ; d < 8 ; d++) {
    EEPROM.write(d, 0);
  }
  lcd.setCursor(10, 0);
lcd.print("please wait"); 
    for(int m=0;m<8;m++){
   EEPROM.write(m,again[m]);
   digitalWrite(39, 0);
   digitalWrite(40, 1);
  motdepasse[m]= EEPROM.read(m);
  lcd.setCursor(m+12, 1);
lcd.print('.'); 
delay(100); 
digitalWrite(39, 1);
digitalWrite(40, 0);
delay(100);
 }digitalWrite(39, 0);
  lcd.clear();
   delay(10);
   lcd.setCursor(11,0);
   lcd.print("the change ");
   lcd.setCursor(11,1);
   lcd.print("successful");} 

   else{lcd.clear();
   lcd.setCursor(10,0);
   lcd.print("please repeat");
  delay(500);
  goto ll;

  }                        
  delay(500);                            
 vf:i=0;
 lcd.clear();
 delay(500);

}

