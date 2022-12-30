#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>

#define DHTPIN 26     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "Priamh";
const char *password = "123123123";
const char* mqtt_server = "192.168.137.1";

int state=0;

const int buttonPin =12;
const int ledpin = 2;
const int ldrPin = 39;

long now = millis();
long lastMeasure = 0;

WiFiClient espClient;
PubSubClient client;

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  //string to json => deserialized into use value
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32")) {
      Serial.println("connected"); 

      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("esp32");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(10);
   dht.begin();

  pinMode(buttonPin, INPUT);
  pinMode(ledpin,OUTPUT);
  pinMode(ldrPin, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
      Serial.display();  
  }

  client.setClient(espClient);
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);

}
void loop(){
  
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 5000) {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Computes temperature values in Celsius

    static char temperatureTemp[7];
    dtostrf(t, 2, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 2, 2, humidityTemp);

    DynamicJsonDocument doc(256);

    doc["temperature"] = temperatureTemp;
    doc["humidity"] = humidityTemp;
    char json_string[256];


    serializeJson(doc, json_string);

  
    client.publish("esp32", json_string);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(json_string);
  }}
