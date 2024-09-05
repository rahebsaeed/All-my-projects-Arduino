int i=0;
void setup() {
  pinMode(2,INPUT);
    pinMode(3,INPUT);
    pinMode(4,INPUT);
      pinMode(13,OUTPUT);
      pinMode(12,OUTPUT);
      pinMode(11,OUTPUT);
      pinMode(10,OUTPUT);

}

void loop() {
if(digitalRead(2)==1){
  while(digitalRead(3)!=1){
    digitalWrite(13, 1);
    digitalWrite(12, 0);
    

  }
  digitalWrite(13, 0);
    digitalWrite(12, 0);
    delay(7000);
     while(digitalRead(4)!=1){
    digitalWrite(13, 0);
    digitalWrite(12, 1);
    
  }
  digitalWrite(13, 0);
    digitalWrite(12, 0);
i++;
}
if(i>=3){
digitalWrite(11, 1);
digitalWrite(10, 1);
}

}
