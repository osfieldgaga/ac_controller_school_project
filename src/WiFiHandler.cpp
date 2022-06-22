#include <WiFiHandler.hpp>
#include <WiFi.h>
#include <Arduino.h>
#include <Prefs.hpp>

String ssid;
String password;

void WiFiHandler::establishWiFi() {
  ssid = Prefs::getWifiSSID();
  password = Prefs::getWifiPassword();

  //to remove any unwated space and \n
  ssid.trim();
  password.trim();

  Serial.print(F("Connecting to "));
  Serial.print(ssid);
  Serial.print(F(" with password "));
  Serial.print(password);
  Serial.print("...");
  Serial.println();

  WiFi.begin(ssid.c_str(), password.c_str());

  while(WiFi.status() != WL_CONNECTED){
    delay(3000);
    Serial.println(F("Connecting..."));

  }
  Serial.print(F("Connection established with "));
  Serial.print(ssid);
  Serial.println();
  Serial.print(F("IP Address is "));
  Serial.println(WiFi.localIP());

  
}