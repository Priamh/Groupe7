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
Adafruit_SSD1306 oled2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
int compteurmenu=1;
int compteurlang=0;
int compteuropt=1;
int compteurfan2=0;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int lock =0;
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
const int JOYX = 36;
const int JOYY = 35;
const int JOYbutt = 2;

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
    if (client.connect("ESP32")) {
      oled.println("connected");
      oled.display();  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("esp32OUT");
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
   if (!oled2.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }

  vTaskDelay(pdMS_TO_TICKS( 2000));         // wait two seconds for initializing

  oled.clearDisplay(); // clear display
  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0, 10);       // set position to display


  oled2.clearDisplay(); // clear display
  oled2.setTextSize(1);         // set text size
  oled2.setTextColor(WHITE);    // set text color
  oled2.setCursor(0, 10);       // set position to display

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
      int rouge = docRecep["rouge"];
      int vert = docRecep["vert"];
      int bleu = docRecep["bleu"];
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
 doc["temperaturecentrale"] = temperatureTemp;
 doc["humiditycentrale"] = humidityTemp;
 doc["fancentral"] = compteurfan;
 doc["fanmodule"] = compteurfan2;
 doc["langue"]= compteurlang;
 doc["degres"]=cf;

 serializeJson(doc, json_string);
 vTaskDelay( pdMS_TO_TICKS( 500 ) );
 }
}

void displaydata(void *pvParameters){
  for(;;){
    oled2.clearDisplay();
    oled2.setCursor(0, 10);       // set position to display
    oled.clearDisplay();
    oled.setCursor(0, 10);       // set position to display
  if(SW_state==0){
  vTaskDelay(pdMS_TO_TICKS(  100));
  
  if(compteurlang==0){
  oled.clearDisplay();
  oled.setCursor(0, 10);       // set position to display
  oled.println("CENTRALE:");
  oled.drawLine(0,17,50, 17, WHITE);
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
  oled.print("Vit.Ventil.Centrale:");
  oled.println(String(compteurfan));
  oled.print("Vit.Ventil.Module :");
  oled.println(String(compteurfan2));
  }

   if(compteurlang==1){
  oled.clearDisplay();
  oled.setCursor(0, 10);       // set position to display
  oled.println("CENTRAL:");
  oled.drawLine(0,17,40, 17, WHITE);
  oled.print("Humidity: ");
  oled.print(humidityTemp);
  oled.println(" %");  
  oled.print("Temperature: ");
  if(cf==0){
  oled.print(temperatureTemp);
  oled.println("*C");
  }
  if(cf==1){
  oled.print(String(((9*t)/5) +32));
  oled.println("*F");
  }
  oled.print("Fan speed central:");
  oled.println(String(compteurfan));
  oled.print("Fan speed module:");
  oled.println(String(compteurfan2));
  }
  oled.print("Options");
  
  if(compteurmenu==1){
      oled.drawLine(0,33,128, 33, WHITE);
      oled.drawLine(0,41,128, 41, WHITE);
  }
  if(compteurmenu==2){
      oled.drawLine(0,41,128, 41, WHITE);
      oled.drawLine(0,49,128, 49, WHITE);
  }
  if(compteurmenu==3){
      oled.drawLine(0,49,128, 49, WHITE);
      oled.drawLine(0,57,128, 57, WHITE);
  }
  }
  if(SW_state==1){
    oled.clearDisplay();
    oled.setCursor(0, 10);       // set position to display
    if(compteurlang==0){
    oled.println("OPTIONS");
     oled.println("");
     oled.drawLine(0,17,41, 17, WHITE);
    oled.print("Langue:");
    oled.println("Francais");
    oled.print("Degres:");
    }
    if(compteurlang==1){
    oled.clearDisplay();
    oled.setCursor(0, 10);       // set position to display
    oled.println("OPTION");
     oled.println("");
     oled.drawLine(0,17,36, 17, WHITE);
    oled.print("Langue:");
    oled.println("English");
    oled.print("Degrees:");
    }
    if(cf==0){
    oled.println("Celsius");
    }
    if(cf==1){
    oled.println("Farenheit");
    }

    if(compteuropt==1){
      oled.drawLine(0,25,128, 25, WHITE);
      oled.drawLine(0,33,128, 33, WHITE);
  }
  if(compteuropt==2){
      oled.drawLine(0,33,128, 33, WHITE);
      oled.drawLine(0,41,128, 41, WHITE);
  }
    
  }
  if(compteurlang==0){
  oled2.println("MODULE 1:");
  oled2.drawLine(0,17,45, 17, WHITE);
  oled2.println("");
  oled2.print("Temperature:");
 String temp = docRecep["temp2"];
 oled2.print(temp);
 if(cf==0){
 oled2.println("*C");
 }
 if(cf==1){
 oled2.println("*F");
 }

 oled2.print("Humidite:");
 String humi = docRecep["humidity2"];
 oled2.print(humi);
 oled2.println("%");


  oled2.print("Luminosite:");
 String lumi = docRecep["lumi"];
 oled2.print(lumi);
 oled2.println("%");
  }
  if(compteurlang==1){
  oled2.println("MODULE 1:");
  oled2.drawLine(0,17,45, 17, WHITE);
  oled2.println("");
  oled2.print("Temperature:");
 String temp2 = docRecep["temp2"];
 oled2.print(temp2);
 if(cf==0){
 oled2.println("*C");
 }
 if(cf==1){
 oled2.println("*F");
 }

 oled2.print("Humidity:");
 String humi2 = docRecep["humidity2"];
 oled2.print(humi2);
 oled2.println("%");


  oled2.print("Luminosity:");
 String lumi2 = docRecep["lumi"];
 oled2.print(lumi2);
 oled2.println("%");

  }

  oled.display();  
  oled2.display(); 
  
  vTaskDelay( pdMS_TO_TICKS( 500 ) );
}
}

void vTask1( void *pvParameters )
{

  
  
  for( ;; )
  {
    client.publish("esp32", json_string);
    vTaskDelay( pdMS_TO_TICKS( 500 ) );
  }
}

void joystickcontrol( void *pvParameters ){

for(;;){

xPosition = analogRead(JOYX);
yPosition = analogRead(JOYY);
  
if(xPosition  <=1600 && lock==0 && SW_state == 0){
  lock=1;
  if(compteurmenu<=3){
    compteurmenu=compteurmenu +1;
    }
    if(compteurmenu>3){compteurmenu=1;}
  

}
if(xPosition >=2000&& lock==0&& SW_state == 0){
  lock=1;
  if(compteurmenu==1){
    compteurmenu=4;
    }
    if(compteurmenu>1){compteurmenu=compteurmenu -1;}
}



if(yPosition <=1600 && lock==0 && SW_state == 0){
  if(compteurmenu==1){
  if(compteurfan==0){
    compteurfan=0;
    }
    if(compteurfan>0){compteurfan=compteurfan -1;}
  }
  if(compteurmenu==2){
  if(compteurfan2==0){
    compteurfan2=0;
    }
    if(compteurfan2>0){compteurfan2=compteurfan2 -1;}
  }
 lock=1;
}
if(yPosition >=2000 && lock==0 && SW_state ==0){
    if (compteurmenu==1){
    if(compteurfan<=2){
    compteurfan=compteurfan +1;
    }
    if(compteurfan>2){compteurfan=3;}
    }
    if (compteurmenu==2){
    if(compteurfan2<=2){
    compteurfan2=compteurfan2 +1;
    }
    if(compteurfan2>2){compteurfan2=3;}
    }
     lock=1;
}
if(yPosition <2000 && yPosition >1600 && xPosition >1600 && xPosition <2000 && digitalRead(JOYbutt)==1){
  lock=0;
}


if(compteurmenu==3){
  if(digitalRead(JOYbutt)==0 && SW_state==0 && lock==0){
    lock=1;
    SW_state=1;
  }

  if(yPosition <=1000 && lock==0 && SW_state==1){
    SW_state=0;
  }

  if(xPosition >=2000&& lock==0&& SW_state == 1){
  lock=1;
  if(compteuropt==1){
    compteuropt=3;
    }
    if(compteuropt>1){compteuropt=compteuropt -1;}
}

  if(xPosition  <=1600 && lock==0 && SW_state == 1){
  lock=1;
  if(compteuropt<=2){
    compteuropt=compteuropt +1;
    }
    if(compteuropt>2){compteuropt=1;}
  }

  if (compteuropt==1){
   if(digitalRead(JOYbutt)==0 && SW_state==1 && lock==0){ 
   lock=1;
    if(compteurlang==1){
    compteurlang=0;
    }

    else if(compteurlang==0){
    compteurlang=1;
    }

    }
}
  if (compteuropt==2){
   if(digitalRead(JOYbutt)==0 && SW_state==1 && lock==0){ 
    lock=1;
    if(cf==1){
    cf=0;
    }

    else if(cf==0){
    cf=1;
    }

    }



}

 
}
vTaskDelay( pdMS_TO_TICKS( 200 ) );
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
  pinMode(JOYbutt,INPUT_PULLUP);


xTaskCreatePinnedToCore(vTask1, "vTask1", 10000, NULL, 3, NULL,0 );
xTaskCreatePinnedToCore(espconnect, "esptask", 10000, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(jsonadd, "jsonreceivetask", 10000, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(displaydata, "displaytask", 10000, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(dhtread, "dhttask", 10000, NULL, 2, NULL, 1);
xTaskCreatePinnedToCore(otherinfo, "lampe", 10000, NULL, 1, NULL, 1);
xTaskCreatePinnedToCore(joystickcontrol, "stickmenu", 10000, NULL, 2, NULL, 1);

}
void loop(){}