#include <SoftwareSerial.h>
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
SoftwareSerial swSerial(D5, D7); //Define hardware connections RX, TX

String DataString = "";
DynamicJsonBuffer jsonBuffer;
void setup() {
  pinMode(D5, INPUT);
  pinMode(D7, OUTPUT);
  Serial.begin(115200);   //Initialize hardware serial with baudrate of 115200
  swSerial.begin(4800);    //Initialize software serial with baudrate of 115200
  Serial.println("\nSoftware serial test started");
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
