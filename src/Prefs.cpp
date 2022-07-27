#include <Preferences.h>
#include <Prefs.hpp>

using namespace std;

#define FORMAT_SPIFFS_IF_FAILED false

Preferences prefs;

void Prefs::initPrefs(){
    prefs.begin("ac_controller", false);

    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
}




void Prefs::clearAllPrefs(){
    prefs.clear();
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

void Prefs::setACConfig(String ac_brand, String nickname, String room_type){
    prefs.putString("ac_brand", ac_brand);
    prefs.putString("nickname", nickname);
    prefs.putString("room_type", room_type);
}

String Prefs::getACBrand(){
    return prefs.getString("ac_brand");

}
String Prefs::getACNickname(){
    return prefs.getString("nickname");
}
String Prefs::getRoomType(){
    return prefs.getString("room_type");
    
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



//SPIFFS helper functions
void Prefs::listDir(fs::FS &fs, const char * dirname, uint8_t levels){
   Serial.printf("Listing directory: %s\r\n", dirname);

   File root = fs.open(dirname);
   if(!root){
      Serial.println("− failed to open directory");
      return;
   }
   if(!root.isDirectory()){
      Serial.println(" − not a directory");
      return;
   }

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if(levels){
            listDir(fs, file.name(), levels -1);
         }
      } else {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file = root.openNextFile();
   }
}

String Prefs::readFile(fs::FS &fs, const char * path){
   Serial.printf("Reading file: %s\r\n", path);

   File file = fs.open(path);
   if(!file || file.isDirectory()){
       Serial.println("− failed to open file for reading");
       //return;
   }
//this was uncommented and the readString() below didn't work
//that was because there was nothing left to read. it's like it
//takes the content out of the file to store it in a variable
//which mean this function can only read a file once and give its output
//when it's ready

   // Serial.println("− read from file:");
   // while(file.available()){
   //    Serial.write(file.read());
   // }

   String file_content = file.readString();

   // Serial.print("\nContent straight from string: \n");
   // Serial.println(file_content);

   return file_content;
}

void Prefs::writeFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Writing file: %s\r\n", path);

   File file = fs.open(path, FILE_WRITE);
   if(!file){
      Serial.println("− failed to open file for writing");
      return;
   }
   if(file.print(message)){
      Serial.println("− file written");
   }else {
      Serial.println("− frite failed");
   }
}

void Prefs::appendFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Appending to file: %s\r\n", path);

   File file = fs.open(path, FILE_APPEND);
   if(!file){
      Serial.println("− failed to open file for appending");
      return;
   }
   if(file.print(message)){
      Serial.println("− message appended");
   } else {
      Serial.println("− append failed");
   }
}

void Prefs::renameFile(fs::FS &fs, const char * path1, const char * path2){
   Serial.printf("Renaming file %s to %s\r\n", path1, path2);
   if (fs.rename(path1, path2)) {
      Serial.println("− file renamed");
   } else {
      Serial.println("− rename failed");
   }
}

void Prefs::deleteFile(fs::FS &fs, const char * path){
   Serial.printf("Deleting file: %s\r\n", path);
   if(fs.remove(path)){
      Serial.println("− file deleted");
   } else {
      Serial.println("− delete failed");
   }
}