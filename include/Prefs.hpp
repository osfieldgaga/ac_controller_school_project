#ifndef PPREFS
#define PPREFS

#include <string>
#include <vector>


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
};

#endif // 