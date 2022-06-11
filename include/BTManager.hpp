#ifndef BT_MANAGER
#define BT_MANAGER

#include <Arduino.h>

class BTManager
{
private:
    /* data */
public:
    void initBT();
    void getWiFiCredentials();
    String getSSID();
    String getPassword();
    bool checkDone();
};

#endif 