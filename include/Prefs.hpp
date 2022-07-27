#ifndef PPREFS
#define PPREFS

#include <string>
#include <vector>

#include <FS.h>
#include <SPIFFS.h>



class Prefs{
    public:
    static void initPrefs();
    static void clearAllPrefs();
    static void clearPref(); //TODO add type
    static bool checkWiFiConfig();
    static void setWiFiConfig(bool wifiStatus);
    static String getWifiSSID();
    static String getWifiPassword();
    static void setWifiSSID(String ssid);
    static void setWifiPassword(String password);
    static bool checkDeviceConfig();
    static void setDeviceConfig(bool deviceStatus);
    static bool hasSetPrefsFirstTime();
    static void setPrefsFirstTime();
    static void resetFirstTime();
    static void setFirebaseConfig(bool firebaseStatus);
    static bool checkFirebaseConfig();
    static void setACConfig(String ac_brand, String nickname, String room_type);
    static String getACBrand();
    static String getACNickname();
    static String getRoomType();

    
    void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    String readFile(fs::FS &fs, const char * path);
    void writeFile(fs::FS &fs, const char * path, const char * message);
    void appendFile(fs::FS &fs, const char * path, const char * message);
    void renameFile(fs::FS &fs, const char * path1, const char * path2);
    void deleteFile(fs::FS &fs, const char * path);

    private:
};

#endif // 