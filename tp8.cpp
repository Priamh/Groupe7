#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
//ajouter callback et blynk
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define butt 12

// Insert your network credentials
#define WIFI_SSID "Priamh"
#define WIFI_PASSWORD "123123123"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAaqueEieLyQVlejALoMnpzYWya2GYDg-Q "

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://tp8systemembarque-default-rtdb.europe-west1.firebasedatabase.app/" 


//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int sw=0;
bool signupOK = false;
int valueled1=0;
int valueled2=0;
int valuebutt=0;

void setup(){
  
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(19,OUTPUT);
  pinMode(23,OUTPUT);
  pinMode(butt,INPUT);
}

void loop(){

  
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/sw", valuebutt)){
      Serial.println("PASSED");
      Serial.printf("PATH: %s\n",  fbdo.dataPath());
      Serial.printf("TYPE: %s\n", fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.printf("REASON: %s\n",  fbdo.errorReason());
    }
    Firebase.RTDB.getInt(&fbdo, "test/led1",&valueled1);
      Serial.println(String(valueled1));
    Firebase.RTDB.getInt(&fbdo, "test/led2",&valueled2);
      Serial.println(String(valueled2));
  }

  if(digitalRead(butt)==LOW){
    valuebutt=0;
  }
  if(digitalRead(butt)==HIGH){
    valuebutt=1;
  }
    digitalWrite(19,valueled2);

  
    digitalWrite(23,valueled1);
  }

