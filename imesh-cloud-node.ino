#include <SoftwareSerial.h>
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "PubSubClient.h"

//ThingSpeak params--->
const char* ssid = "ssid";
const char* pwd = "pwd";
//char* topic="channels/<channelsID>/publish/<WriteAPIKey>";
char* mqttServer = "mqtt.thingspeak.com";
//<---
//WiFiSetting params--->
WiFiClient wifiClient;
PubSubClient client(mqttServer, 1883, wifiClient);
//<---
SoftwareSerial swSerial(D5, D7); //Define hardware connections RX, TX

String DataString = "";
DynamicJsonBuffer jsonBuffer;

void setup() {
  pinMode(D5, INPUT);
  pinMode(D7, OUTPUT);
  Serial.begin(115200);   //Initialize hardware serial with baudrate of 115200
  //WiFi Connection Initialization -->
  Serial.printf("\n\n");
  Serial.print("WIFI: connecting");
  WiFi.begin(ssid, pwd);
  while(WiFi.status() != WL_CONNECTED)  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WIFI: connected!");
  //<---
  
  //MQTT CONNECTION TEST -->
  Serial.println("MQTT: connecting");
  String clientName = "ESP-Thingspeak";
  if(client.connect((char*) clientName.c_str())) {
    Serial.println("MQTT: connected");
    Serial.println("MQTT[\"TOPIC\"]: " + String(topic));

    if(client.publish(topic, "hello from ESP8266")) {
      Serial.println("MQTT: Publish OK!");
    } else {
      Serial.println("MQTT: Publish falied");
    }
  } else {
    Serial.println("MQTT: connection failed");
    abort();
  }
  //<---
  
  //SerialCommunication initialization -->
  Serial.println("sw-SERIAL: Initializing Software Serial");
  swSerial.begin(4800);    
  Serial.println("sw-SERIAL: Software serial Inintialized --> Start Gathering Data");
  //<---
  
}
void loop() {
  while (swSerial.available() > 0)
  {
      char c = swSerial.read();  //gets one byte from serial buffer⸮
      DataString += c; //makes the string DataString
  }
  if (DataString != "" && DataString.startsWith("{"))
    {
      Serial.println("SERIAL: " + DataString);
      Serial.println("SERIAL: data length: [" + String(DataString.length()) + "]");
      JsonObject& payload = jsonBuffer.parseObject(DataString);
      if(payload.containsKey("topic") && payload.containsKey("temp") && payload.containsKey("humid")) {
        String str = "";
        payload.printTo(str);
        Serial.printf("JSON: %s", str.c_str());
      }
    } else {
      Serial.println(".");
    }
  DataString = "";
  delay(2000);
}
