#include <WiFiHandler.hpp>
#include <ESP8266WiFi.h>
#include <Arduino.h>

const char* ssid1 = "Fido";
const char* ssid2 = "Osfield's iPhone";
const char* ssid3 = "ZEPHYRUS 4870";
const char* password = "imbecileniais";
const char* password3 = "14R5<24L";

void WiFiHandler::establishWiFi() {
  WiFi.begin(ssid3, password3);
  Serial.print("Connecting to ");
  Serial.print(ssid3);
  Serial.print("...");
  Serial.println();

  while(WiFi.status() != WL_CONNECTED){
    delay(3000);
    Serial.println("Connecting...");

  }
  Serial.print("Connection established with ");
  Serial.print(ssid3);
  Serial.println();
  Serial.print("IP Address is ");
  Serial.print(WiFi.localIP());

  
}