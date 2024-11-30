#include "DHT.h"
#include "WiFi.h"
#include <PubSubClient.h>
#include "ThingSpeak.h"

// Pin and DHT configuration
#define DHTPIN 4
#define DHTTYPE DHT11
#define LEDPIN 13  // Pin for controlling the LED

// ThingSpeak credentials
unsigned long myChannelNumber = 2755011;
const char* myWriteAPIKey = "HOJP1764PT1SCX7F";

// WiFi credentials
const char* ssid = "mywifi";
const char* password = "87654321";

// MQTT server and topics
const char* mqtt_server = "192.168.43.173";
const char* topic_humidity = "dht22/humidity";
const char* topic_temperature = "dht22/temperature";
const char* topic_led = "dht22sensor/ledpin12";
const char* led_state = "dht22sensor/ledstate";
const char* refresh = "dht22/refreshdata";

long lastMsg = 0;

// Initialize DHT, WiFi, MQTT, and ThingSpeak
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Callback function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';  // Null-terminate payload
  String data = String((char*)payload);

  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(data);

  // Handle LED control
  if (strcmp(topic, topic_led) == 0) {
    if (data == "true") {
      digitalWrite(LEDPIN, HIGH);  // Turn on LED
      Serial.println("LED turned ON");
      ThingSpeak.setField(3, 1);
    } else if (data == "false") {
      digitalWrite(LEDPIN, LOW);  // Turn off LED
      Serial.println("LED turned OFF");
      ThingSpeak.setField(3, 0);
    }
  } 
  // Handle refresh message
  else if (strcmp(topic, refresh) == 0) {
    Serial.println("Refresh command received");
    lastMsg = millis() - 20001;  // Force immediate update
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32-DHT22-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(topic_led);      // Subscribe to LED control topic
      client.subscribe(refresh);        // Subscribe to refresh topic
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LEDPIN, OUTPUT);  // Set LED pin as output

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" connected!");
  Serial.println("Local IP: " + WiFi.localIP().toString());

  // Initialize MQTT and ThingSpeak
  ThingSpeak.begin(espClient);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // Ensure MQTT connection
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publish LED state
  client.publish(led_state, digitalRead(LEDPIN) ? "true" : "false");

  // Sensor data publishing logic
  long now = millis();
  if (now - lastMsg > 20000) {  // Publish every 20 seconds
    lastMsg = now;

    // Read sensor data
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Validate sensor data
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Publish to MQTT topics
    char humidity_str[8];
    char temperature_str[8];
    dtostrf(humidity, 6, 2, humidity_str);
    dtostrf(temperature, 6, 2, temperature_str);
    client.publish(topic_humidity, humidity_str);
    client.publish(topic_temperature, temperature_str);

    // Send data to ThingSpeak
    ThingSpeak.setField(1, humidity);
    ThingSpeak.setField(2, temperature);
    int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    // Print debug info
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    if (response == 200) {
      Serial.println("ThingSpeak update successful.");
    } else {
      Serial.println("ThingSpeak update failed. Code: " + String(response));
    }
  }
}
