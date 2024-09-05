
#include <Servo.h>                                                         // appel de la bibliothèque

        
#define potentiometre A0                                              // affectation des broches
#define servo 2


int valeur;

Servo monservo;                                                            // création de l'objet monservo

void setup()
{
  monservo.attach(servo);                                             // le servomoteur est relié à la broche N°2
}

void loop()
{
  valeur = analogRead(potentiometre);                       // conversion analogique-numérique de la tension sur A0
  valeur = map(valeur, 0, 1023, 0, 180);                       // changement d'échelle
  monservo.write(valeur);                                             // positionne le servomoteur à la valeur
  delay(50);                                                                     // pause pour laisser le temps au servomoteur de se positionner
}