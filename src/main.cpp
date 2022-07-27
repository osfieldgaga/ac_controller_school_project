#include <Arduino.h>
//#include <eeprom_manager.hpp>
#include <IR_Receiver.hpp>
#include <WiFiHandler.hpp>
#include <UniqueIdentifiers.hpp>
#include <Prefs.hpp>
#include <LearnIR.hpp>

#include <BTManager.hpp>

//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <FirebaseHandler.hpp>

#include <string>

//#include <Adafruit_Sensor.h>
//#include <DHT.h> //TODO add DHT library

//Constants
#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//DHT dht(DHTPIN, DHTTYPE); ////TODO Initialize DHT sensor for normal 16mhz Arduino

//Variables

float hum, temp;  //Stores humidity value

int dht_millis; //Stores temperature value


//EEPROM_Manager eeprom;
IRReceiver irReceiver;
//IRSender irSender;
WiFiHandler wifiHandler;
FirebaseHandler firebaseHandler;
BTManager btManager;
LearnIR learnIR;


uint8_t currentState = 0;

//Prefs prefs_main;


const int kFirebaseRefreshRate = 5000; //2000ms = 2 seconds
unsigned long now = 0, now2 = 0;
uint8_t prevACState, prevACTemperature;
bool operationMode;

void setup() {
  //initialize preferences system
  Prefs::initPrefs();

  // put your setup code here, to run once:
  //eeprom.initializeMemory(); //compulsory

  //set up stuff for IR stuff
  Serial.begin(9600);
  irReceiver.initIR(); // Start the receiver

  btManager.initBT();

  //delay(5000);
  
  //TODO initialize dht
  //dht.begin();
  dht_millis = 0;

 

  //TODO let initial configuration happen here, check config status to decided when it's necessary
  
  //currentState = 0;
  if(!Prefs::hasSetPrefsFirstTime()){
    Prefs::setWiFiConfig(false);
    Prefs::setDeviceConfig(false);
    Prefs::setFirebaseConfig(false);

    Prefs::setPrefsFirstTime();
  }

  //if wifi is already conif, simply go to connect, else grab credentials using BT
  if(!Prefs::checkWiFiConfig()){
    currentState = 0;
  } else {
    currentState = 1;
  }
  

}

void loop() {
  // put your main code here, to run repeatedly:

  switch (currentState)
  {
  case 0: // wait for BT and grab credentials
    btManager.getWiFiCredentials();

    if (btManager.checkDone())
    {
      currentState = 1;
    }
    break;

  case 1: // connect wifi with received credentials

    if (!Prefs::checkWiFiConfig()) //if wifi not configured, set preference
    {
      Serial.println(F("Connecting WiFi using received credentials"));
      
      Prefs::setWifiSSID(btManager.getSSID());
      Prefs::setWifiPassword(btManager.getPassword());

      //credentials received, save preference
      Prefs::setWiFiConfig(true);
    }
    else
    {
      //if wifi already config when device boot, simply connect wifi
      wifiHandler.establishWiFi();
      currentState = 2;
    }

    break;

  case 2: //connect to firebase

    if(firebaseHandler.checkStatus()){
      if (!Prefs::checkFirebaseConfig()) //if first time, create database
      {
        Serial.println(F("Configuring Firebase"));
        firebaseHandler.testSetValue();
        firebaseHandler.setUpDatabase();
        Prefs::setFirebaseConfig(true);
      }
      else //if not first time, simply grab values there
      {
        prevACState = firebaseHandler.obtainACState();
        prevACTemperature = firebaseHandler.obtainACTemperature();

        if (!Prefs::checkDeviceConfig())
        {
          currentState = 3; //if first time, go get AC config
        }
        else
        {
          currentState = 4; //else, device was configured before and just go to ormal operation mode
        }
      }
    }else{
      firebaseHandler.connectFirebase();
      Serial.println(F("Connecting to Firebase, waiting for token"));
    }
    
  break;

  case 3: // wait for config from DB
    if (firebaseHandler.checkStatus())
    {
      if (!firebaseHandler.checkIfConfigDone())
      {
        Serial.print(F("config bool: "));
        Serial.print(firebaseHandler.checkIfConfigDone());
        Serial.println();
        Serial.println(F("Waiting for config..."));
        Serial.println();
        delay(5000);
      }
      else
      {
        firebaseHandler.obtainACConfiguration();
        Serial.println(F("Config received"));
        Serial.print(F("AC Brand: "));
        Serial.println(Prefs::getACBrand());
        Serial.print(F("AC Nickname: "));
        Serial.println(Prefs::getACNickname());
        Serial.print(F("Room Type: "));
        Serial.println(Prefs::getRoomType());
        Prefs::setDeviceConfig(true);
        currentState = 4;
      }
    }

    break;

  case 4:
    
    operationMode = firebaseHandler.onbtainOperationMode();

    if (operationMode == false)
    {
      if (firebaseHandler.checkStatus() && millis() - now > kFirebaseRefreshRate)
      {
        now = millis();
        // do something related to firebase, this part is triggered only when the device is authenticated and when
        // the threshold time is reached
        Serial.print(F("Checking Firebase... "));

        // check if there has been a modification in the database data, which would mean a change in state
        // if it's the case, change the state
        if (firebaseHandler.obtainACState() != prevACState)
        {
          Serial.print(F("State changed."));
          Serial.println();
          if (firebaseHandler.obtainACState() == 1)
          {
            // when 1, the state of the AC is on
            Serial.print(F("Turn AC ON"));
            irReceiver.turnOnAC();
            Serial.println();

            // TODO send signal to turn on AC
          }
          else if (firebaseHandler.obtainACState() == 0)
          {
            // normally if it's 0, Ac is off
            irReceiver.turnOnAC();
            Serial.print(F("Turn AC OFF"));
            Serial.println();

            // TODO send signal to turn off AC
          }
          else
          {
            Serial.print(F("Invalid State"));
            Serial.println();
          }

          prevACState = firebaseHandler.obtainACState();
        }
        else
        {
          Serial.print(F("No state change... "));
          Serial.println();
        }

        // check change in temperature
        if (firebaseHandler.obtainACTemperature() != prevACTemperature)
        {
          uint8_t ACTemperature;
          Serial.print(F("Temperature changed"));
          Serial.println();
          if (firebaseHandler.obtainACTemperature() >= 16 && firebaseHandler.obtainACTemperature() <= 30)
          {
            ACTemperature = firebaseHandler.obtainACTemperature();
            Serial.print(F("Setting temperature at "));
            Serial.print(ACTemperature);
            Serial.println();
            irReceiver.setACTemp(ACTemperature);
          }
          else
          {
            Serial.print(F("Temperature changed but is in an invalid range"));
            Serial.println();
          }
          prevACTemperature = ACTemperature;
          Serial.println();
        }
        else
        {
          Serial.print(F("No temperature change... "));
          Serial.println();
          Serial.println();
        }
      }
    }
    else
    {
      // TODO implement automatic behavior
      Serial.println(F("Automatic Mode activated."));
    }

    // check tmperature and humidity
    //send it to database
  // if(millis() - dht_millis > 5000){
  //   dht_millis = millis();
  //   hum = dht.readHumidity();
  //   temp= dht.readTemperature();
  //   //Print temp and humidity values to serial monitor

    
    
  //   Serial.print("Humidity: ");
  //   Serial.print(hum);
  //   Serial.print(" %, Temp: ");
  //   Serial.print(temp);
  //   Serial.println(" Celsius");

  //   firebaseHandler.sendRoomTelemetry(temp, hum);
  // }
  break;

  default:
    break;
  }

  if (millis() - now2 > 10000){
    now2 = millis();
    Serial.println();
    Serial.println(F("Free RAM: "));
    Serial.print(ESP.getFreeHeap());
    Serial.println();
    Serial.println();
  }
 
  irReceiver.decodeIR();
  yield();


  //change this!
  if(Serial.available()){
    
    String command = Serial.readStringUntil('\n');
    String temperature;
    if(command.equals(F("sam_on"))) {
      irReceiver.turnOnSamsung();
    }else if(command.equals(F("sam_off"))){
      irReceiver.turnOffSamsung();
    }else if(command.equals(F("tcl_on"))) {
      irReceiver.turnOnTCL();
    }else if(command.equals(F("tcl_off"))){
      irReceiver.turnOffTCL();
    }else if (command.equals(F("tcl_temp"))){
      Serial.print(F("Enter temperature: "));

      temperature = Serial.readStringUntil('\n');
      //int temp = temperature.toInt();
      uint8_t temp = 23;
      irReceiver.setTCLTemp(temp);
    

    }else if (command.equals(F("midea_on"))){
      irReceiver.turnOnMidea();

    }else if (command.equals(F("midea_off"))){
      irReceiver.turnOffMidea();
    
    }else if (command.equals(F("change_wifi"))){
      Prefs::setWiFiConfig(false);
      ESP.restart();
    }else if(command.equals(F("ac_on"))){
      irReceiver.turnOnAC();
    }else if(command.equals(F("ac_off"))){
      irReceiver.turnOffAC();
    }else if(command.toInt() >= 16 && command.toInt() <= 30){
      irReceiver.setACTemp(command.toInt());
    }else if(command.equals("learn")){
      learnIR.clearAll();
      learnIR.learnIRSequence(1);
    }
    

    else if(command.equals("reset_prefs")){
      Prefs::resetFirstTime();
      Serial.println(F("Reset preferences and restarting"));

      ESP.restart();
    }

    else if(command.equals("show_prefs")){
      Serial.println(F("Device configuration:"));
      Serial.print(F("AC Brand: "));
      Serial.println(Prefs::getACBrand());
      Serial.print(F("AC Nickname: "));
      Serial.println(Prefs::getACNickname());
      Serial.print(F("Room Type: "));
      Serial.println(Prefs::getRoomType());
    }
    
    else{
      Serial.println(F("Not a valid command"));
    }
  }
}

void LearnIR(){
  Serial.print(F("Initiating learning sequence..."));
  Serial.println(F("----------------"));
  //Serial.println("t")



  
}