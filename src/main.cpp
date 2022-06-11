#include <Arduino.h>
//#include <eeprom_manager.hpp>
#include <IR_Receiver.hpp>
#include <WiFiHandler.hpp>
#include <UniqueIdentifiers.hpp>
#include <Prefs.hpp>

#include <BTManager.hpp>

//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <FirebaseHandler.hpp>

#include <string>

//#include <Adafruit_Sensor.h>
#//include <DHT.h> //TODO add DHT library

//Constants
#define DHTPIN 12     // what pin we're connected to
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

uint8_t currentState = 0;

//Prefs prefs_main;


const int kFirebaseRefreshRate = 5000; //2000ms = 2 seconds
unsigned long now = 0;
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

  
  //first time boot, run this just once
  //once setPrefsFirstTIme returns true, this guy isn't ran anymore
 
  // if(Prefs::hasSetPrefsFirstTime() != true){
  //   Prefs::setWiFiConfig(false);
  //   Prefs::setDeviceConfig(false);

  //   Prefs::setPrefsFirstTime();
  // }
  
  //Set wifi config to true
  // if(!Prefs::checkWiFiConfig()){

  //   //TODO do what is necessary to set up wifi
    
  //   Prefs::setWifiSSID("ZEPHYRUS 4870");
  //   Prefs::setWifiPassword("14R5<24L");
  //   Prefs::setWiFiConfig(true);
    
  // }else {
  //   wifiHandler.establishWiFi();
  // }

  //firebaseHandler.connectFirebase();

  //get the current state and temperature in the DB
  // prevACState = firebaseHandler.obtainACState();
  // prevACTemperature = firebaseHandler.obtainACTemperature();
  

  //TODO setup database automatically

  //firebaseHandler.setUpDatabase();

  // if(!Prefs::checkDeviceConfig()){

  //   //firebaseHandler.setUpDatabase();
  //   Prefs::setDeviceConfig(true);
  // }else {
    
    
  //}
  
  currentState = 0;
  Prefs::setWiFiConfig(false);
  Prefs::setDeviceConfig(false);
  // firebaseHandler.connectFirebase();
  // firebaseHandler.signUpUSer();
  // firebaseHandler.testSetValue();

}

void loop() {
  // put your main code here, to run repeatedly:
  
  //TODO Implement dht code
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
  // }

  switch (currentState)
  {
  case 0: // wait for BT
    btManager.getWiFiCredentials();

    if (btManager.checkDone())
    {
      currentState = 1;
    }
    break;

  case 1: // connect wifi

    if (!Prefs::checkWiFiConfig())
    {

      // TODO do what is necessary to set up wifi
      Serial.println(F("Connecting WiFi using received credentials"));
      
      Prefs::setWifiSSID(btManager.getSSID());
      Prefs::setWifiPassword(btManager.getPassword());
      Prefs::setWiFiConfig(true);
    }
    else
    {

      wifiHandler.establishWiFi();
      currentState = 2;
    }

    break;

  case 2: //connect to firebase
    firebaseHandler.connectFirebase();
    if(firebaseHandler.checkStatus()){
      if (!Prefs::checkDeviceConfig())
      {
        Serial.println(F("Configuring Firebase"));
        // firebaseHandler.connectFirebase();
        firebaseHandler.signUpUSer();
        //firebaseHandler.testSetValue();
        firebaseHandler.setUpDatabase();
        Prefs::setDeviceConfig(true);
      }
      else
      {
        Serial.println(F("Connecting to Firebase"));
        // firebaseHandler.connectFirebase();

        prevACState = firebaseHandler.obtainACState();
        prevACTemperature = firebaseHandler.obtainACTemperature();

        currentState = 3;
      }
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
        Serial.println(F("Config received"));
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
        // do sonething related to firebase, this part is triggered only when the device is authenticated and when
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
            Serial.println();

            // TODO send signal to turn off AC
          }
          else if (firebaseHandler.obtainACState() == 0)
          {
            // normally if it's 0, Ac is off
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
  break;

  default:
    break;
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