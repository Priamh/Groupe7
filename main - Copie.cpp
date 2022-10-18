#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>


#define DHTPIN 26     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Prototypes
void sendData(String params);

const char *ssid = "Priamh";
const char *password = "123123123";

String GOOGLE_SCRIPT_ID = "AKfycbxElI9S6Q02Fb5D-CHEBbY6vNG9NVuJMhK1DhtZV52Tt_9appmZiNfQgTW2aOkl9TyF";
int value;
int state=0;
const int buttonPin =12;
const int ledpin = 2;
const int ldrPin = 39;

const char * root_ca=\
"-----BEGIN CERTIFICATE-----\n"
"MIIFljCCA36gAwIBAgINAgO8U1lrNMcY9QFQZjANBgkqhkiG9w0BAQsFADBHMQsw\n"
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
"MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMjAwODEzMDAwMDQyWhcNMjcwOTMwMDAw\n"
"MDQyWjBGMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
"Y2VzIExMQzETMBEGA1UEAxMKR1RTIENBIDFDMzCCASIwDQYJKoZIhvcNAQEBBQAD\n"
"ggEPADCCAQoCggEBAPWI3+dijB43+DdCkH9sh9D7ZYIl/ejLa6T/belaI+KZ9hzp\n"
"kgOZE3wJCor6QtZeViSqejOEH9Hpabu5dOxXTGZok3c3VVP+ORBNtzS7XyV3NzsX\n"
"lOo85Z3VvMO0Q+sup0fvsEQRY9i0QYXdQTBIkxu/t/bgRQIh4JZCF8/ZK2VWNAcm\n"
"BA2o/X3KLu/qSHw3TT8An4Pf73WELnlXXPxXbhqW//yMmqaZviXZf5YsBvcRKgKA\n"
"gOtjGDxQSYflispfGStZloEAoPtR28p3CwvJlk/vcEnHXG0g/Zm0tOLKLnf9LdwL\n"
"tmsTDIwZKxeWmLnwi/agJ7u2441Rj72ux5uxiZ0CAwEAAaOCAYAwggF8MA4GA1Ud\n"
"DwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0T\n"
"AQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUinR/r4XN7pXNPZzQ4kYU83E1HScwHwYD\n"
"VR0jBBgwFoAU5K8rJnEaK0gnhS9SZizv8IkTcT4waAYIKwYBBQUHAQEEXDBaMCYG\n"
"CCsGAQUFBzABhhpodHRwOi8vb2NzcC5wa2kuZ29vZy9ndHNyMTAwBggrBgEFBQcw\n"
"AoYkaHR0cDovL3BraS5nb29nL3JlcG8vY2VydHMvZ3RzcjEuZGVyMDQGA1UdHwQt\n"
"MCswKaAnoCWGI2h0dHA6Ly9jcmwucGtpLmdvb2cvZ3RzcjEvZ3RzcjEuY3JsMFcG\n"
"A1UdIARQME4wOAYKKwYBBAHWeQIFAzAqMCgGCCsGAQUFBwIBFhxodHRwczovL3Br\n"
"aS5nb29nL3JlcG9zaXRvcnkvMAgGBmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcN\n"
"AQELBQADggIBAIl9rCBcDDy+mqhXlRu0rvqrpXJxtDaV/d9AEQNMwkYUuxQkq/BQ\n"
"cSLbrcRuf8/xam/IgxvYzolfh2yHuKkMo5uhYpSTld9brmYZCwKWnvy15xBpPnrL\n"
"RklfRuFBsdeYTWU0AIAaP0+fbH9JAIFTQaSSIYKCGvGjRFsqUBITTcFTNvNCCK9U\n"
"+o53UxtkOCcXCb1YyRt8OS1b887U7ZfbFAO/CVMkH8IMBHmYJvJh8VNS/UKMG2Yr\n"
"PxWhu//2m+OBmgEGcYk1KCTd4b3rGS3hSMs9WYNRtHTGnXzGsYZbr8w0xNPM1IER\n"
"lQCh9BIiAfq0g3GvjLeMcySsN1PCAJA/Ef5c7TaUEDu9Ka7ixzpiO2xj2YC/WXGs\n"
"Yye5TBeg2vZzFb8q3o/zpWwygTMD0IZRcZk0upONXbVRWPeyk+gB9lm+cZv9TSjO\n"
"z23HFtz30dZGm6fKa+l3D/2gthsjgx0QGtkJAITgRNOidSOzNIb2ILCkXhAd4FJG\n"
"AJ2xDx8hcFH1mt0G/FX0Kw4zd8NLQsLxdxP8c4CU6x+7Nz/OAipmsHMdMqUybDKw\n"
"juDEI/9bfU1lcKwrmz3O2+BtjjKAvpafkmO8l7tdufThcV4q5O8DIrGKZTqPwJNl\n"
"1IXNDw9bg1kWRxYtnCQ6yICmJhSFm/Y3m6xv+cXDBlHz4n/FsRC6UfTd\n"
"-----END CERTIFICATE-----\n";


WiFiClientSecure client;

void setup() {
  Serial.begin(9600);
  delay(10);

  dht.begin();

  pinMode(buttonPin, INPUT);
  pinMode(ledpin,OUTPUT);
  pinMode(ldrPin, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.println("Started");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(analogRead(0));
  Serial.println("TP3 GoogleSheets ready...");
}

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String strTemp, strHum, strParameter ,strLDR;
void loop() {
   if (value==HIGH){state=1;}
  value = digitalRead(buttonPin);


  //Send an HTTP POST request every delay
  if (state==1) {
  int ldrStatus = analogRead(ldrPin);
    delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int t = dht.readTemperature();
    strTemp = String(t, DEC);
    strHum = String(h, DEC);
    strLDR = String(ldrStatus,DEC);
    Serial.println(strTemp);
    Serial.println(strHum);
     Serial.println(strLDR);
     Serial.println(String(state));
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      strParameter = "temperature=" + strTemp + "&humidity=" + strHum+ "&ldr=" + strLDR ;
      
      sendData(strParameter);
      state=0;
    
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void sendData(String params) {
  HTTPClient http;
  String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+params;
  Serial.println(url);
  Serial.println("Making a request");
  // Your Domain name with URL path or IP address with path
  http.begin(url, root_ca); //Specify the URL and certificate
  // Send HTTP GET request
  int httpCode = http.GET();
  if (httpCode > 0) {
    digitalWrite(ledpin,HIGH);
    delay(100);
    digitalWrite(ledpin,LOW);
    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);
    String payload = http.getString();
    Serial.println(payload);


  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpCode);
  }
  // Free resources
  http.end();
}