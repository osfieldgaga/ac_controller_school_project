#include <BTManager.hpp>
#include <BluetoothSerial.h>

BluetoothSerial bluetoothSerial;
uint8_t message_length = 0;
String wifi_ssid, wifi_password;
//String delimiter = "$";

bool isDone = false;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void BTManager::initBT(){
    bluetoothSerial.begin(F("Smart AC Controller"));
    Serial.println(F("The device started, now you can pair it with bluetooth!"));
}

void BTManager::getWiFiCredentials(){
    Serial.println(F("Getting wifi credentials from BT"));

    String message;

    if (bluetoothSerial.available())
    {
        message = bluetoothSerial.readString();
        Serial.println(message);

        uint8_t pos = message.indexOf(F("$"));
        Serial.println(pos);
        wifi_ssid = message.substring(4, pos);
        // message.remove(0, pos);
        wifi_password = message.substring(pos + 6);

        Serial.println(wifi_ssid);
        Serial.println(wifi_password);

        isDone = true;

    }

    
delay(1000);
}

String BTManager::getSSID(){
    return wifi_ssid.c_str();
}

String BTManager::getPassword(){
    return wifi_password;
}

bool BTManager::checkDone(){
    return isDone;
}