#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <SD.h>
#include <ETH.h>
#include <Update.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 
#define SCREEN_ADDRESS2 0x3D

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define DHTPIN 32     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

const char *ssid = "Priamh";
const char *password = "123123123";
const char* mqtt_server = "192.168.137.1";

int state=0;
int compteurfan=0;
int compteurlang=0;


int cf=0;

int lumi=0;

const int buttonPin =25;
const int ledpinR = 5;
const int ledpinG = 17;
const int ledpinB = 16;

const int ledpinH = 23;
const int ledpinM = 19;
const int ledpinC = 18;

const int ldrPin = 39;

int motor1Pin1 = 26; 
int motor1Pin2 = 27; 
int enable1Pin = 22; 
 String messageTemp;


static char temperatureTemp[7];
static char humidityTemp[7];
DynamicJsonDocument doc(256);
DynamicJsonDocument docRecep(512);
char json_string[256];
float t;
float h;

long now = millis();
long lastMeasure = 0;

WiFiClient espClient;
PubSubClient client;


void callback(String topic, byte* message, unsigned int length) {
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];

    
  }
  Serial.println(messageTemp);
  deserializeJson(docRecep , messageTemp);
  messageTemp="";

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    oled.setCursor(0, 10);       // set position to display
    oled.print("Attempting MQTT connection...");
      oled.display();  
    // Attempt to connect
    if (client.connect("ESP32mod")) {
      oled.println("connected");
      oled.display();  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("esp32OUT2");
    } else {
      // Wait 5 seconds before retrying
     vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}
void screensetup(){
  // initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }

  vTaskDelay(pdMS_TO_TICKS( 2000));         // wait two seconds for initializing

  oled.clearDisplay(); // clear display
  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0, 10);       // set position to display


}

void connectmqtt(){
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  oled.setCursor(0, 10);       // set position to display

  oled.println("Started");
  oled.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
  vTaskDelay(pdMS_TO_TICKS( 500));
  oled.print(".");
  oled.display();  
  }
  client.setClient(espClient);
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
}



void espconnect(void *pvParameters ){

for(;;){
 
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
}}
void dhtread(void *pvParameters){
  for(;;){
  
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    }
 vTaskDelay( pdMS_TO_TICKS( 10000 ) );
 }
}


void otherinfo(void *pvParameters){

  for(;;){
    
 compteurfan= int(docRecep["fan2"]);
    lumi= analogRead(ldrPin)/50;
    if(t>=25 ){
      digitalWrite(ledpinH,HIGH);
      digitalWrite(ledpinM,HIGH);
      digitalWrite(ledpinC,HIGH);
    }
    if(t<=18){
      digitalWrite(ledpinH,LOW);
      digitalWrite(ledpinM,LOW);
      digitalWrite(ledpinC,HIGH);
    }
    if(t<25 && t>18){
      digitalWrite(ledpinH,LOW);
      digitalWrite(ledpinM,HIGH);
      digitalWrite(ledpinC,HIGH);

    }

    if(digitalRead(buttonPin)==HIGH){
      int rouge = docRecep["rouge2"];
      int vert = docRecep["vert2"];
      int bleu = docRecep["bleu2"];
      analogWrite(ledpinR,rouge );
      analogWrite(ledpinG,vert);
      analogWrite(ledpinB,bleu);

    }
    if(digitalRead(buttonPin)==LOW){
      analogWrite(ledpinR,0 );
      analogWrite(ledpinG,0);
      analogWrite(ledpinB,0);

    }

    if(compteurfan==0){

  digitalWrite(enable1Pin, LOW);
  digitalWrite(motor1Pin2, LOW);

analogWrite(motor1Pin1, 0);


    }
     if(compteurfan==1){

  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin2, LOW);

analogWrite(motor1Pin1, 50); 

    }
    if(compteurfan==2){

  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin2, LOW);

  analogWrite(motor1Pin1, 100);

    }
    if(compteurfan==3){

  digitalWrite(enable1Pin, HIGH);
  digitalWrite(motor1Pin2, LOW);
analogWrite(motor1Pin1, 200);

    }
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
  }

}

void jsonadd(void *pvParameters){
  for(;;){
 dtostrf(t, 2, 2, temperatureTemp);   
 dtostrf(h, 2, 2, humidityTemp);
 doc["temperaturemodule"] = temperatureTemp;
 doc["humiditymodule"] = humidityTemp;
 doc["luminosite"] = lumi;


 serializeJson(doc, json_string);
 vTaskDelay( pdMS_TO_TICKS( 500 ) );
 }
}

void displaydata(void *pvParameters){
  for(;;){

    compteurlang=int(docRecep["lang"]);

    cf=int(docRecep["degre"]);
    oled.clearDisplay();
    oled.setCursor(0, 10);       // set position to display
  vTaskDelay(pdMS_TO_TICKS(  100));
  
  if(compteurlang==0){
  oled.clearDisplay();
  oled.setCursor(0, 10);       // set position to display
  oled.println("MODULE:");
 oled.drawLine(0,17,40, 17, WHITE);
  oled.print("Humidite: ");
  oled.print(humidityTemp);
  oled.println(" %");  
  oled.print("Temperature:");
  if(cf==0){
  oled.print(temperatureTemp);
  oled.println(" *C ");
  }
  if(cf==1){
  oled.print(String(((9*t)/5) +32));
  oled.println(" *F ");
  }
     oled.print("Luminosite: ");
  oled.print(String(lumi));
  oled.println(" %");  
  oled.print("Vit.Ventil.Module:");
  oled.println(String(compteurfan));

  }

   if(compteurlang==1){
  oled.clearDisplay();
  oled.setCursor(0, 10);       // set position to display
  oled.println("MODULE:");
  oled.drawLine(0,17,40, 17, WHITE);
  oled.print("Humidity: ");
  oled.print(humidityTemp);
  oled.println(" %");  
  oled.print("Temperature:");
  if(cf==0){
  oled.print(temperatureTemp);
  oled.println("*C");
  }
  if(cf==1){
  oled.print(String(((9*t)/5) +32));
  oled.println("*F");
  }
    oled.print("Luminosity: ");
  oled.print(String(lumi));
  oled.println(" %");  
  oled.print("Fan speed module:");
  oled.println(String(compteurfan));
  

  
  }
  
  
  oled.display();  
  
  vTaskDelay( pdMS_TO_TICKS( 500 ) );
}
}

void vTask1( void *pvParameters )
{

  
  
  for( ;; )
  {
    client.publish("esp32mod", json_string);
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
  }
}


void setup() {
  Serial.begin(9600);
  vTaskDelay(pdMS_TO_TICKS( 10));
screensetup();
connectmqtt();
  dht.begin();

  pinMode(buttonPin, INPUT);
  pinMode(ledpinR,OUTPUT);
  pinMode(ledpinG,OUTPUT);
  pinMode(ledpinB,OUTPUT);
  pinMode(ledpinH,OUTPUT);
  pinMode(ledpinM,OUTPUT);
  pinMode(ledpinC,OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(ldrPin,INPUT);
 


xTaskCreatePinnedToCore(vTask1, "vTask1", 10000, NULL, 3, NULL,0 );
xTaskCreatePinnedToCore(espconnect, "esptask", 10000, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(jsonadd, "jsonreceivetask", 10000, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(displaydata, "displaytask", 10000, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(dhtread, "dhttask", 10000, NULL, 2, NULL, 1);
xTaskCreatePinnedToCore(otherinfo, "lampe", 10000, NULL, 1, NULL, 1);


}
void loop(){}