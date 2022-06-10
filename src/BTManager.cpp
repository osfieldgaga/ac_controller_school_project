#include <BTManager.hpp>
#include <BluetoothSerial.h>

BluetoothSerial bluetoothSerial;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void BTManager::initBT(){
    bluetoothSerial.begin("Smart AC Controller");
    Serial.println("The device started, now you can pair it with bluetooth!");
}

void BTManager::getWiFiCredentials(){
    
}