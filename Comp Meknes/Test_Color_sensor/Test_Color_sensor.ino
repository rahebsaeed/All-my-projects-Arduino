const int S2 = A0;  // Filtre Rouge
const int S3 = A1;  // Filtre Rouge
const int OUT = A4; // Signal

void setup() {
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  Serial.begin(9600);
}

void loop() {
  // Mesurer le Rouge (S2=0, S3=0)
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int rouge = pulseIn(OUT, LOW);

  // Mesurer le Vert (S2=1, S3=1)
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int vert = pulseIn(OUT, LOW);

  // Mesurer le Bleu (S2=0, S3=1)
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int bleu = pulseIn(OUT, LOW);

  // Afficher les valeurs
  Serial.print("R:"); Serial.print(rouge);
  Serial.print(" V:"); Serial.print(vert);
  Serial.print(" B:"); Serial.println(bleu);

  // Détection du rouge dominant (seuils à ajuster !)
  if (rouge < vert && rouge < bleu && rouge < 100) {
    Serial.println(">>> Rouge RGB détecté ! <<<");
  }

  delay(200);
}