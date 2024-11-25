#include "DHT.h"
#include "WiFi.h"
#include "ThingSpeak.h"
#define DHTPIN 4
// Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302) , AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)|
unsigned long myChannelNumber =2755011;
const char* mywriteAPIKey = "HOJP1764PT1SCX7F"; // Define ThingSpeak channel number and write API key
const char* ssid = "Wokwi-GUEST";
const char* password = ""; // Define WiFi network credentials
DHT dht (DHTPIN, DHTTYPE);

const char* server = "api.thingspeak.com";
WiFiClient client;
void setup() {
Serial. begin (115200);
Serial.println (F ("DHT22 test!"));
delay (10) ;
dht.begin(); //Se inicia el sensor
// Connect to WiFi network
Serial.println();
Serial.println();
Serial.print ("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
  delay(100);
  Serial.print(".");
}
Serial.println ("*");
Serial.println ("WiFi connected");
Serial.println ("Local IP: " + WiFi.localIP());
WiFi.mode(WIFI_STA);
ThingSpeak.begin(client);
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  ThingSpeak.setField(1,h);
  ThingSpeak.setField(2,hic);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));


  int x = ThingSpeak.writeFields(myChannelNumber,mywriteAPIKey);


  if(x == 200){
    Serial.println("Data pushed successfull");
  }else{
    Serial.println("Push error" + String(x));
  }
  Serial.println("---"); 
  // Wait a few seconds between measurements.
  delay(2000);
}
