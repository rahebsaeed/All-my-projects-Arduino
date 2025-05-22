// Ultrasonic Sensor Pins
#define TRIG_0 11
#define ECHO_0 12
#define TRIG_R 8
#define ECHO_R A2  
#define TRIG_L A3  
#define ECHO_L 11

// Infrared sensor pins
#define INF_R A0  // Right infrared
#define INF_L A5  // Left infrared
#define INF_B A1  // Back infrared

// Max distance for ultrasonic sensors (cm)
#define MAX_DISTANCE 400

void setup() {
  // Initialize serial communication (faster for debugging)
  Serial.begin(9600);
  
  // Set up ultrasonic sensors
  pinMode(TRIG_0, OUTPUT);
  pinMode(ECHO_0, INPUT);
  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);
  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);
  
  // Set up infrared sensors
  pinMode(INF_R, INPUT);
  pinMode(INF_L, INPUT);
  pinMode(INF_B, INPUT);
  
  Serial.println("Sensor Testing Initialized - Fast Mode");
  Serial.println("------------------------------------");
}

// Fast ultrasonic reading with median filtering
unsigned int getFilteredDistance(int trigPin, int echoPin) {
  unsigned int d1 = getSingleReading(trigPin, echoPin);
  delay(10); // Small delay between pings
  unsigned int d2 = getSingleReading(trigPin, echoPin);
  delay(10);
  unsigned int d3 = getSingleReading(trigPin, echoPin);

  // Sort the three values (median filter)
  if (d1 > d2) { unsigned int t = d1; d1 = d2; d2 = t; }
  if (d2 > d3) { unsigned int t = d2; d2 = d3; d3 = t; }
  if (d1 > d2) { unsigned int t = d1; d1 = d2; d2 = t; }

  return d2; // Return the median value
}

// Single ultrasonic reading with timeout
unsigned int getSingleReading(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Timeout for max distance (30000µs = 5m)
  unsigned long duration = pulseIn(echoPin, HIGH, 30000); 
  return (duration == 0) ? MAX_DISTANCE : duration / 58; // Convert to cm
}

void loop() {
  // Read ultrasonic sensors (median filtered)
  unsigned int distance0 = getFilteredDistance(TRIG_0, ECHO_0);  // Front
  unsigned int distanceR = getFilteredDistance(TRIG_R, ECHO_R);  // Right
  unsigned int distanceL = getFilteredDistance(TRIG_L, ECHO_L);  // Left
  
  // Read infrared sensors (1 = no obstacle, 0 = obstacle detected)
  int infR = analogRead(INF_R);
  int infL = analogRead(INF_L);
  int infB = analogRead(INF_B);
  
  // Print all sensor readings
  Serial.println("Ultrasonic Sensors (cm):");
  Serial.print("Front: "); Serial.print(distance0);
  Serial.print(" | Right: "); Serial.print(distanceR);
  Serial.print(" | Left: "); Serial.println(distanceL);
  
  Serial.println("Infrared Sensors (0=obstacle, 1=clear):");
  Serial.print("Right IR: "); Serial.print(infR);
  Serial.print(" | Left IR: "); Serial.print(infL);
  Serial.print(" | Back IR: "); Serial.println(infB);
  
  Serial.println("------------------------------------");
  delay(200); // Reduced delay for faster response
}