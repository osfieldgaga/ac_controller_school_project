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
    static std::vector<std::string> getWifiCredentials();
    static bool hasSetPrefsFirstTime();
    static void setPrefsFirstTime();
};

#endif // 