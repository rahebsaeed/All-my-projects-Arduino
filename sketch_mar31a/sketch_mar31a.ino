#include <Keypad.h>                      
#define L1 2                                    // affectation des broches
#define L2 3
#define L3 4
#define L4 5
#define C1 6
#define C2 7
#define C3 8
const byte lignes = 4;                    // 4 lignes
const byte colonnes = 3;               // 3 colonnes
int i=0;
char code[lignes][colonnes] = {  
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
char touche[4];
byte broches_lignes[lignes] = {L1, L2, L3, L4};                   // connexion des lignes
byte broches_colonnes[colonnes] = {C1, C2, C3};              // connexion des colonnes

Keypad clavier = Keypad( makeKeymap(code), broches_lignes, broches_colonnes, lignes, colonnes);    
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);                     // initialisation du moniteur série
}

void loop() {
  // put your main code here, to run repeatedly:
            // acquisition de la touche
           while(i!=4){
touche[i] = clavier.getKey();
  if (touche[i] != NO_KEY)                 // si appui sur une touche
  {
    Serial.print(touche[i]);  
     i++; 
     delay(200);     
    }
       }
 
 Serial.println(' ');
 if(touche[0]=='1'&&touche[1]=='2'&&touche[2]=='3'&&touche[3]=='4'){Serial.println("code correct");i=0;}
    else{Serial.println("code faux");i=0;}
}
