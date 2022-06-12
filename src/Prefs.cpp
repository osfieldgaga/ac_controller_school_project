#include <Preferences.h>
#include <Prefs.hpp>

using namespace std;

Preferences prefs;

void Prefs::initPrefs(){
    prefs.begin("ac_controller", false);
}

void Prefs::clearAllPrefs(){
    prefs.clear();
}

void Prefs::clearPref(){
    //TODO add type and function definition
    //preferences.remove(prefKey);
}


//WiFi preferences
bool Prefs::checkWiFiConfig(){
    return prefs.getBool("isWiFiConfig");
}

String Prefs::getWifiSSID(){
    return prefs.getString("wifi_ssid");
}

String Prefs::getWifiPassword(){
    return prefs.getString("wifi_password");
}

void Prefs::setWifiSSID(String ssid) {
    prefs.putString("wifi_ssid", ssid);
}

void Prefs::setWifiPassword(String password){
    prefs.putString("wifi_password", password);
}

void Prefs::setWiFiConfig(bool wifiStatus){

    prefs.putBool("isWiFiConfig", wifiStatus);
}

void Prefs::setFirebaseConfig(bool firebaseStatus){

    prefs.putBool("isFBConfig", firebaseStatus);
    Serial.println(F("Set fb config true"));
}

void Prefs::setPrefsFirstTime(){
    prefs.putBool("hasSetPrefs", true);
}

bool Prefs::hasSetPrefsFirstTime(){
    return prefs.getBool("hasSetPrefs");
}

void Prefs::resetFirstTime(){
    prefs.putBool("hasSetPrefs", false);
}



//Device configurartion
bool Prefs::checkFirebaseConfig(){
    return prefs.getBool("isFBConfig");
}

bool Prefs::checkDeviceConfig(){
    return prefs.getBool("isDeviceConfig");
}
void Prefs::setDeviceConfig(bool deviceStatus){
    prefs.putBool("isDeviceConfig", deviceStatus);
}