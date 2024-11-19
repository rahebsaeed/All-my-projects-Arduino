#include "DHT.h"
#include "ThingSpeak.h"
#include ‹ESP8266WiFi.h›
#define DHTPIN D8
// Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 22 (AM2302) , AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)|
unsigned long myChannelNumber =2344288;
const char * mywriteAPIKey = "4DG2GI2GBS426C9W"; // Define ThingSpeak channel number and write API key
const chart ssid = "NOUVEAU";
const char* password = "00000000000000"; // Define WiFi network credentials
DHT dht (DHTPIN, DHTTYPE) :
void setupo {
Serial. begin (9600) :
Serial.printin (F ("DHTxx test!")) ;
delay (10) ;
dht.begin(); //Se inicia el sensor
// Connect to WiFi network
Serial.printin();
Serial.printin(;
Serial.print ("Connecting to ");
Serial.printin(ssid);
WiFi.begin(ssid, password) ;
while (Wifi.status()!= WL_CONNECTED) {
delay (500) :
Serial.print (".");
}
Serial.println ("*");
Serial.printin ("WiFi connected");
Serial.printin (WiFi.localIP());
}

void loop() {
// Wait a few seconds between measurements.
delay (2000) ;
// Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
float h = dht. readHumidity() ;
// Read temperature as Celsius (the default)
float t = dht. readTemperature(;
// Read temperature as Fahrenheit (isFahrenheit = true)
float f = dht. readlemperature (true);
// Check if any reads failed and exit early (to try again).
if (isnan(h) || isnan(t) || isnan (f)) {
Serial.println (F("Failed to read from DHT sensor!"));
return;
}
// Compute heat index in Fahrenheit (the default)
float hif = dht.computeHeatIndex (f, h);
// Compute heat index in Celsius (isFahreheit = false)
float hic = dht.computeheatIndex (t, h, false);
Serial.print (F ("Humidity: ")):
Serial.print (h);
Serial.print (F("* Temperature: ")):
Serial.print (t);
Serial.print (F("°C ")):
Serial.print (f) ;
Serial.print (F("°F Heat index: "));
Serial.print (hic);
Serial.print (F("°C ")) ;
Serial.print (hif);
Serial.printin (F("°F"));
}
