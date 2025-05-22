// Définition des broches pour les moteurs et le pont H L298N
#define IN1 2  // Moteur gauche avant
#define IN2 3  // Moteur gauche arrière
#define IN3 4  // Moteur droit avant
#define IN4 5  // Moteur droit arrière

// Définition des broches pour les capteurs à ultrasons
#define TRIG_0 6
#define ECHO_0 7
#define TRIG_R 8
#define ECHO_R 9
#define TRIG_L 10
#define ECHO_L 11

// Définition des broches pour les capteurs infrarouges
#define INF_R 12 // Capteur infrarouge à droite
#define INF_L A0 // Capteur infrarouge à gauche
// Définition de la broche pour le capteur de couleur (désactivé pour l'instant)

long duration_0, duration_R, duration_L;
int distance_0, distance_R, distance_L;

void setup() {
  // Initialisation des broches pour les moteurs et le pont H
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Initialisation des broches pour les capteurs à ultrasons
  pinMode(TRIG_0, OUTPUT);
  pinMode(ECHO_0, INPUT);
  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);
  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);
  
  // Initialisation des capteurs infrarouges
  pinMode(INF_R, INPUT);
  pinMode(INF_L, INPUT);
  
  // Sérialisation pour le débogage
  Serial.begin(9600);
}

void loop() {
  // Lire les distances des capteurs à ultrasons
  distance_0 = readUltrasonicDistance(TRIG_0, ECHO_0);
  distance_R = readUltrasonicDistance(TRIG_R, ECHO_R);
  distance_L = readUltrasonicDistance(TRIG_L, ECHO_L);
  
  // Lire l'état des capteurs infrarouges
  int irRight = digitalRead(INF_R);
  int irLeft = digitalRead(INF_L);
  
  // Logique de mouvement en fonction des distances et des capteurs infrarouges
  if (distance_0 < 20 && distance_R < 20 && distance_L < 20) {
    // Si tous les capteurs à ultrasons détectent un obstacle
    // Vérifier les capteurs infrarouges
    if (irRight == HIGH) {  // Si le capteur infrarouge à droite détecte une zone libre
      turnRight();
      Serial.println("Obstacle détecté à droite et à gauche, rotation à droite");
    } else if (irLeft == HIGH) {  // Si le capteur infrarouge à gauche détecte une zone libre
      turnLeft();
      Serial.println("Obstacle détecté à droite et à gauche, rotation à gauche");
    } else {
      // Si aucun infrarouge ne détecte une voie libre, on fait un mouvement aléatoire
      randomTurn();
      Serial.println("Aucun infrarouge libre, mouvement aléatoire");
    }
  } else {
    // Si des obstacles sont détectés uniquement sur le côté droit ou gauche
    if (distance_0 < 20) {
      stopMovement();
      if (distance_R < distance_L) {
        turnLeft();
      } else {
        turnRight();
      }
    } else if (irRight == LOW) {
      turnLeft();
    } else if (irLeft == LOW) {
      turnRight();
    } else {
      moveForward();
    }
  }

  delay(100);
}

// Fonction pour lire la distance d'un capteur à ultrasons
long readUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

// Fonction pour faire avancer le robot
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Fonction pour stopper le robot
void stopMovement() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Fonction pour tourner à gauche
void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Fonction pour tourner à droite
void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Fonction pour tourner aléatoirement si aucun infrarouge ne détecte un obstacle
void randomTurn() {
  int turnDirection = random(0, 2);  // Génère un nombre aléatoire entre 0 et 1
  if (turnDirection == 0) {
    turnLeft();
    Serial.println("Rotation aléatoire à gauche");
  } else {
    turnRight();
    Serial.println("Rotation aléatoire à droite");
  }
}