//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#endif

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char* ssid = "Priamh";
const char* password = "123123123";
float humidity=5;
float temperature=13;
int LED;
int BP;

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {


  if(!LittleFS.begin(true)){
    Serial.println("An Error has occurred while mounting LITTLEFS");
}
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());


    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html","text/html", false); 
    });
        server.on("/function.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/function.js","text/js", false); 
    });
        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css","text/css", false); 
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
    DynamicJsonDocument doc(256);
  doc["LED"] =LED;
  doc["BP"] =BP;
  doc["HUMIDITY"] =humidity;
  doc["TEMPERATURE"] =temperature;
 
  char json_string[256];
  serializeJson(doc,json_string);
  ws.textAll(json_string);

}