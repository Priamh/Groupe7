/**

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 **/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL1AVMHHeF" 
#define BLYNK_DEVICE_NAME "TP5deux" 
#define BLYNK_AUTH_TOKEN "nnRoKzQ6evTGSYlvFsSo-sk6cPgUxx0h"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
#define DHTPIN 26
#define DHTTYPE DHT11
#define LDR_PIN 39



#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

const int bpin=12 ;
int bp=0;
int state=0;
char auth[] = BLYNK_AUTH_TOKEN;
#define DHTTYPE    DHT11     // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Priamh";
char pass[] = "123123123";

BlynkTimer timer;

WidgetLED led1(V1);
uint32_t delayMS;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png%22");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png%22");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made%22");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()

{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  pinMode(bpin,INPUT);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  dht.begin();
   delayMS = sensor.min_delay / 1000;
}

void loop()
{

  if(digitalRead(bpin)==HIGH && state==0){

    bp=1;
    state=1;
  }
  if(digitalRead(bpin)==LOW && state==1){

    state=0;
  }

  if( bp==1 && led1.getValue()){
    led1.off();
    Serial.print("off");
    bp=0;
  }
  if( bp==1 && !led1.getValue()){
    led1.on();
    Serial.print("on");
    bp=0;
  }

   Blynk.virtualWrite(V3, analogRead(LDR_PIN));
  delay(delayMS);
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    Blynk.virtualWrite(V5, event.temperature);
    dht.humidity().getEvent(&event);
    Blynk.virtualWrite(V6, event.temperature);


  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
