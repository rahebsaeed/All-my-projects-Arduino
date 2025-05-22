const int s0 = A0;
const int s1 = A1;
const int s2 = A2;
const int s3 = A3;
const int outPin = 6;

int whiteRed, whiteGreen, whiteBlue;
const int tolerance = 15; // percent deviation from white baseline

void setup() {
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(outPin, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  Serial.begin(9600);

  Serial.println("Calibrating... Place sensor on WHITE surface.");
  delay(3000);

  whiteRed = readColorFreq(LOW, LOW);
  whiteGreen = readColorFreq(HIGH, HIGH);
  whiteBlue = readColorFreq(LOW, HIGH);

  Serial.println("Calibration Done!");
  Serial.print("White R: "); Serial.println(whiteRed);
  Serial.print("White G: "); Serial.println(whiteGreen);
  Serial.print("White B: "); Serial.println(whiteBlue);
}

int readColorFreq(int s2Val, int s3Val) {
  digitalWrite(s2, s2Val);
  digitalWrite(s3, s3Val);
  delayMicroseconds(100); // faster than delay(100)
  return pulseIn(outPin, LOW, 25000); // 25ms timeout for speed
}

void loop() {
  int red = readColorFreq(LOW, LOW);
  int green = readColorFreq(HIGH, HIGH);
  int blue = readColorFreq(LOW, HIGH);

  // Normalize against white (avoid division; use percentage diff)
  int redDiff = ((whiteRed - red) * 100) / whiteRed;
  int greenDiff = ((whiteGreen - green) * 100) / whiteGreen;
  int blueDiff = ((whiteBlue - blue) * 100) / whiteBlue;

  String detected = "WHITE";

  if (redDiff > tolerance && redDiff > greenDiff && redDiff > blueDiff) {
    detected = "RED";
  } else if (greenDiff > tolerance && greenDiff > redDiff && greenDiff > blueDiff) {
    detected = "GREEN";
  } else if (blueDiff > tolerance && blueDiff > redDiff && blueDiff > greenDiff) {
    detected = "BLUE";
  }

  Serial.print("Detected: "); Serial.println(detected);
  delay(150); // Reduce if you want even faster response
}
