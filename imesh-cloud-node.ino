//#define BLYNK_PRINT Serial
#include <SoftwareSerial.h>
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "PubSubClient.h"
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include "ifarm.h"
//Blynk Params-->
SimpleTimer timer;
WidgetTerminal terminal(V51);
//<---
//ThingSpeak params--->
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
  //BLYNK TEST-->
  Blynk.begin(blynkAuth, ssid, pwd);
  terminal.println("BLYNK[OK]: Blynk Initialized!");
  //<---
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
  String clientName = "imesh";
  if(client.connect((char*) clientName.c_str())) {
    Serial.println("MQTT: connected");
    Serial.println("MQTT[\"TOPIC\"]: " + String(topic));
    terminal.println("BLYNK[OK]: MQTT connected!");
    if(client.publish(topic, "hello from ESP8266")) {
      Serial.println("MQTT: Publish OK!");
      terminal.println("BLYNK[OK]: MQTT Initialized!");
    } else {
      Serial.println("MQTT: Publish falied");
      terminal.println("BLYNK[ERR]: MQTT Initialization failed!");
    }
  } else {
    Serial.println("MQTT: connection failed");
    terminal.println("BLYNK[ERR]: MQTT connection failed!");
    abort();
  }
  //<---
  
  //SerialCommunication initialization -->
  Serial.println("sw-SERIAL: Initializing Software Serial");
  swSerial.begin(4800);    
  Serial.println("sw-SERIAL: Software serial Inintialized --> Start Gathering Data");
  //<---
  terminal.println("BLYNK[OK]: BRIDGE data trasfering started!");
  terminal.flush();
  timer.setInterval(2000, mqttDataReader);
}
void loop() {
  Blynk.run();
  timer.run();
}

//Additional Function-->
void mqttDataReader() {
  while (swSerial.available() > 0)
  {
      char c = swSerial.read();  //gets one byte from serial buffer⸮
      DataString += c; //makes the string DataString
  }
  if (DataString != "" && DataString.startsWith("{"))
    {
      Serial.println("SERIAL: " + DataString);
      Serial.println("SERIAL: data length: [" + String(DataString.length()) + "]");
      terminal.println("BLYNK[OK]: Incoming Data: " + DataString);
      terminal.flush();
      JsonObject& payload = jsonBuffer.parseObject(DataString);
      if(payload.containsKey("topic") && payload.containsKey("temp") && payload.containsKey("humid")) {
        String str = "";
        payload.printTo(str);
        Serial.printf("JSON: %s\n", str.c_str());
        String temp = "";
        payload["temp"].printTo(temp);
        String humid = "";
        payload["humid"].printTo(humid);
        humid.replace("\"", "");
        temp.replace("\"", "");
        String mqttMsg = "field1=";
        mqttMsg += temp;
        mqttMsg += "&field2=";
        mqttMsg += humid;
//        mqttMsg += "&status=MQTTPUBLISH";
        if(client.connected()) {
          Serial.println("MQTT: Sending " + mqttMsg);
          if(client.publish(topic, (char*) mqttMsg.c_str())) {
            Serial.println("MQTT: Publish OK!");
            Serial.println("BLYNK[OK]: Sending data to BLYNK");
            terminal.println("TEMP: " + temp);
            terminal.println("HUMID: " + humid);
          } else {
            Serial.println("MQTT: Publish Failed");
            terminal.println("BLYNK[ERR]: Data Sending Failed!");
          }
        }
      }
    } else {
      Serial.println(".");
    }
  DataString = "";
}
//<--
