#include <ESP8266WiFi.h>

// Pin definition for onboard LED
#define LED_PIN LED_BUILTIN // Onboard LED pin

// Wi-Fi Credentials (Optional, for Wi-Fi Test)
const char* ssid = "MyBluetooth";           // Replace with your Wi-Fi SSID
const char* password = "87432221";   // Replace with your Wi-Fi password

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000); // Allow time for serial monitor setup

  // Initialize onboard LED as output
  pinMode(LED_PIN, OUTPUT);

  // Blink LED to indicate startup
  blinkLED(3, 200);

  Serial.println("Wemos D1 Test Program");
  Serial.println("----------------------");
  
  // Optional: Test Wi-Fi connection
  testWiFi();
}

void loop() {
  // Blink the LED every second as a heartbeat
  digitalWrite(LED_PIN, LOW); // Turn LED on
  delay(500);
  digitalWrite(LED_PIN, HIGH); // Turn LED off
  delay(500);

  // Print a message to the serial monitor
  Serial.println("Wemos D1 is running...");
}

void blinkLED(int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW); // Turn LED on
    delay(duration);
    digitalWrite(LED_PIN, HIGH); // Turn LED off
    delay(duration);
  }
}

void testWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 100) {
      Serial.println("\nFailed to connect to Wi-Fi. Check credentials.");
      blinkLED(7, 100);
      return;
    }
  }

  Serial.println("\nWi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  blinkLED(10, 140);
}