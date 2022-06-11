#include <WiFiHandler.hpp>
#include <WiFi.h>
#include <Arduino.h>
#include <Prefs.hpp>

// const char* ssid1 = "Fido";
// const char* ssid2 = "Osfield's iPhone";
// const char* ssid3 = "ZEPHYRUS 4870";
// //const char* password = "imbecileniais";
// const char* password3 = "14R5<24L";

// String ssid = "ZEPHYRUS 4870";
// String password = "14R5<24L";

String ssid;
String password;
//Prefs prefs_wifi;

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