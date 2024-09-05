char x,y;
void setup() {
  pinMode(12,OUTPUT);
    pinMode(13,OUTPUT);

  for(int i=10;i<=11;i++)pinMode(i,INPUT);
 digitalWrite(12,0); 
digitalWrite(13,0);
}

void loop() {
  x=digitalRead(10);
  y=digitalRead(11);
if(x==1){digitalWrite(12,0);digitalWrite(13,1);}
if(y==1){digitalWrite(13,0);digitalWrite(12,1);}

  

}
