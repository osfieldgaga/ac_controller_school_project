#include <WiFiHandler.hpp>
#include <WiFi.h>
#include <Arduino.h>
#include <Prefs.hpp>

const char* ssid1 = "Fido";
const char* ssid2 = "Osfield's iPhone";
const char* ssid3 = "ZEPHYRUS 4870";
//const char* password = "imbecileniais";
const char* password3 = "14R5<24L";

String ssid;
String password;
//Prefs prefs_wifi;

void WiFiHandler::establishWiFi() {
  ssid = Prefs::getWifiSSID();
  password = Prefs::getWifiPassword();

  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.print("...");
  Serial.println();

  while(WiFi.status() != WL_CONNECTED){
    delay(3000);
    Serial.println("Connecting...");

  }
  Serial.print("Connection established with ");
  Serial.print(ssid);
  Serial.println();
  Serial.print("IP Address is ");
  Serial.print(WiFi.localIP());

  
}