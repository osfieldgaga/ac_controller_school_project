#include <Arduino.h>
#include <eeprom_manager.hpp>
#include <IR_Receiver.hpp>
#include <IR_Sender.hpp>
#include <WiFiHandler.hpp>
#include <UniqueIdentifiers.hpp>
#include <Prefs.hpp>

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
int chk;
float hum, temp;  //Stores humidity value

int dht_millis; //Stores temperature value


EEPROM_Manager eeprom;
IRReceiver irReceiver;
IRSender irSender;
WiFiHandler wifiHandler;
FirebaseHandler firebaseHandler;

int currentState = 0;

//Prefs prefs_main;


const int kFirebaseRefreshRate = 5000; //2000ms = 2 seconds
unsigned long now = 0;
int prevACState, prevACTemperature;
bool operationMode;

void setup() {
  //initialize preferences system
  Prefs::initPrefs();

  // put your setup code here, to run once:
  eeprom.initializeMemory(); //compulsory

  //set up stuff for IR stuff
  Serial.begin(9600);
  irReceiver.initIR(); // Start the receiver

  //delay(5000);
  
  //TODO initialize dht
  //dht.begin();
  dht_millis = 0;

 

  //TODO let initial configuration happen here, check config status to decided when it's necessary

  
  //first time boot, run this just once
  //once setPrefsFirstTIme returns true, this guy isn't ran anymore
 
  if(Prefs::hasSetPrefsFirstTime() != true){
    Prefs::setWiFiConfig(false);
    Prefs::setDeviceConfig(false);

    Prefs::setPrefsFirstTime();
  }
  
  //Set wifi config to true
  if(!Prefs::checkWiFiConfig()){

    //TODO do what is necessary to set up wifi
    
    Prefs::setWifiSSID("ZEPHYRUS 4870");
    Prefs::setWifiPassword("14R5<24L");
    Prefs::setWiFiConfig(true);
    
  }else {
    wifiHandler.establishWiFi();
  }

  firebaseHandler.connectFirebase();

  //get the current state and temperature in the DB
  prevACState = firebaseHandler.obtainACState();
  prevACTemperature = firebaseHandler.obtainACTemperature();
  

  //TODO setup database automatically

  //firebaseHandler.setUpDatabase();

  // if(!Prefs::checkDeviceConfig()){

  //   //firebaseHandler.setUpDatabase();
  //   Prefs::setDeviceConfig(true);
  // }else {
    
    
  //}
  
  

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
  case 0: //wait for BT
    /* code */
    break;
  
  default:
    break;
  }

  operationMode = firebaseHandler.onbtainOperationMode();

  if (operationMode == false)
  {
    if (firebaseHandler.checkStatus() && millis() - now > kFirebaseRefreshRate)
    {
      now = millis();
      // do sonething related to firebase, this part is triggered only when the device is authenticated and when
      // the threshold time is reached
      Serial.print("Checking Firebase... ");

      // check if there has been a modification in the database data, which would mean a change in state
      // if it's the case, change the state
      if (firebaseHandler.obtainACState() != prevACState)
      {
        Serial.print("State changed."); 
        Serial.println();
        if (firebaseHandler.obtainACState() == 1)
        {
          // when 1, the state of the AC is on
          Serial.print("Turn AC ON");
          Serial.println();

          // TODO send signal to turn off AC
        }
        else if (firebaseHandler.obtainACState() == 0)
        {
          // normally if it's 0, Ac is off
          Serial.print("Turn AC OFF");
          Serial.println();

          // TODO send signal to turn off AC
        }
        else
        {
          Serial.print("Invalid State");
          Serial.println();
        }

        prevACState = firebaseHandler.obtainACState();
      }
      else
      {
        Serial.print("No state change... ");
        Serial.println();
      }

      // check change in temperature
      if (firebaseHandler.obtainACTemperature() != prevACTemperature)
      {
        int ACTemperature;
        Serial.print("Temperature changed");
        Serial.println();
        if (firebaseHandler.obtainACTemperature() >= 16 && firebaseHandler.obtainACTemperature() <= 30)
        {
          ACTemperature = firebaseHandler.obtainACTemperature();
          Serial.print("Setting temperature at ");
          Serial.print(ACTemperature);
          Serial.println();
        }
        else
        {
          Serial.print("Temperature changed but is in an invalid range");
          Serial.println();
        }
        prevACTemperature = ACTemperature;
        Serial.println();
      }
      else
      {
        Serial.print("No temperature change... ");
        Serial.println();
        Serial.println();
      }
    }
  }else {
    //TODO implement automatic behavior
  }

  irReceiver.decodeIR();
  yield();

  //change this!
  if(Serial.available()){
    
    String command = Serial.readStringUntil('\n');
    String temperature;
    if(command.equals("sam_on")) {
      irReceiver.turnOnSamsung();
    }else if(command.equals("sam_off")){
      irReceiver.turnOffSamsung();
    }else if(command.equals("tcl_on")) {
      irReceiver.turnOnTCL();
    }else if(command.equals("tcl_off")){
      irReceiver.turnOffTCL();
    }else if (command.equals("tcl_temp")){
      Serial.print("Enter temperature: ");

      temperature = Serial.readStringUntil('\n');
      //int temp = temperature.toInt();
      int temp = 23;
      irReceiver.setTCLTemp(temp);
    

    }else if (command.equals("midea_on")){
      irReceiver.turnOnMidea();

    }else if (command.equals("midea_off")){
      irReceiver.turnOffMidea();
    }
    
    else{
      Serial.println("Not a valid command");
    }
  }
}

void LearnIR(){
  Serial.print("Initiating learning sequence...");
  Serial.println("----------------");
  //Serial.println("t")



  
}