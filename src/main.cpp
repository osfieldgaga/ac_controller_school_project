#include <Arduino.h>
//#include <eeprom_manager.hpp>
#include <IR_Receiver.hpp>
#include <WiFiHandler.hpp>
#include <UniqueIdentifiers.hpp>
#include <Prefs.hpp>
#include <LearnIR.hpp>
#include <SmartTemperature.hpp>
#include <BodyDetection.hpp>
#include <IRsend.h>

#include <BTManager.hpp>

//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <FirebaseHandler.hpp>

#include <string>

//#include <Adafruit_Sensor.h>
#include <DHT.h>

//Constants
#define DHTPIN 4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

//Variables

const uint8_t kFrequency = 38;
uint16_t size = 868;

float hum, temp;  //Stores humidity value

int dht_millis; //Stores temperature delay value
int smartTemp_millis; //Stores temperature delay value
int bodyDetect_millis; //Stores body detector delay value
int bodyDetect_count = 12;
int bodyDetect_matrixSum = 0;

bool auto_check;
//EEPROM_Manager eeprom;
IRReceiver irReceiver;
//IRSender irSender;
WiFiHandler wifiHandler;
FirebaseHandler firebaseHandler;
BTManager btManager;
LearnIR learnIR;
SmartTemperature smartTemp;
BodyDetection bodyDetector;
IRsend irSend(16);


uint8_t currentState = 0;

//Prefs prefs_main;


const int kFirebaseRefreshRate = 5000; //2000ms = 2 seconds
unsigned long now = 0, now2 = 0;
uint8_t prevACState, prevACTemperature, currentACTemp;
bool operationMode, operationMode_temp;

int ACTemperature;

void setup() {
  auto_check = false;
  //initialize preferences system
  Prefs::initPrefs();

  // put your setup code here, to run once:

  //set up stuff for IR stuff
  Serial.begin(9600);
  irReceiver.initIR(); // Start the receiver

  btManager.initBT();
  bodyDetector.initializeBodyDetector();

  //delay(5000);
  
  //TODO initialize dht
  dht.begin();
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
  
  currentACTemp = 24; //default value
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
          currentState = 4; //else, device was configured before and just go to normal operation mode
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
    
    //operationMode = firebaseHandler.onbtainOperationMode();
    operationMode = true;

    if (operationMode == false)
    {
      auto_check = false;
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
      if(!auto_check){
        Serial.println(F("Automatic Mode activated.\n"));
        Serial.println(F("Setting AC temperature at median point 24.0"));
        firebaseHandler.writeACTemp(24);
        Serial.print(24);
        auto_check = true;
      }
      
      
      if (millis() - smartTemp_millis > 600000) //check every 10 minutes
                                                // to give time for the AC o set the temperature
      {
        smartTemp_millis = millis();
        ACTemperature = firebaseHandler.obtainACTemperature();
        
        if(firebaseHandler.obtainACTemperature() > 0){
          ACTemperature = firebaseHandler.obtainACTemperature();
        }
        hum = dht.readHumidity();    
        temp = dht.readTemperature();

        float newTemp = smartTemp.getSmartTemp(temp, hum, ACTemperature);

        if (newTemp != ACTemperature)
        {
          firebaseHandler.writeACTemp(newTemp);
          Serial.print("Current AC Temperature: ");
          Serial.print(ACTemperature);
          Serial.println();
          Serial.print("Changed temperature in DB to: ");
          Serial.print(newTemp);
          Serial.println();
          irReceiver.setACTemp(newTemp);
        } else {
          Serial.print("Current AC Temperature is ");
          Serial.print(ACTemperature);
          Serial.print(", no change needed.");
          Serial.println();
        }
      }

      if(millis() - bodyDetect_millis > 5000){ //check every five seconds, 12 times
          bodyDetect_millis = millis();
          //bodyDetect_matrixSum += bodyDetector.createMaskMatrix(dht.readTemperature());
          // bodyDetect_count--;

          // if(bodyDetect_count == 0){
          //   bodyDetect_count = 12;
          //   Serial.println();
          //   Serial.print("Assessing room occupancy");
          //   Serial.println();
          //   Serial.print("Mask Matrix sum: ");
          //   Serial.print(bodyDetect_matrixSum);
          //   Serial.print("      ");
          //   Serial.print((bodyDetect_matrixSum * 100) / (64*12));
          //   Serial.print("%");
          //   Serial.println();
          //   bodyDetect_matrixSum = 0;
          // }
      }


      
    //Print temp and humidity values to serial monitor
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
    
    else if(command.equals("pmv")){
      float h = dht.readHumidity();
      float t = dht.readTemperature();

      if (isnan(h) || isnan(t))
      {
        Serial.println(F("Failed to read from DHT sensor!"));
      }
      else
      {
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println();

        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.println();

        smartTemp.getPMV(t, h);
      }
    }

    else if(command.equals("matrix")) {
      bodyDetector.createMaskMatrix(dht.readTemperature());
    }

    else if(command.equals("eyes")){
      bodyDetector.detectPresence(dht.readTemperature());
    }
    else if(command.equals("eyes2")){
      bodyDetector.detectPresenceV2();
    }

    else if(command.equals("test_on")){
      Serial.print("Sending test IR");
      uint16_t* raw = learnIR.readIRCode(1, "on");
      uint16_t raw_arr[868];
      int i = 0;
      for(i = 0; i< 868; i++){
        raw_arr[i] = raw[i];
      }
      irSend.sendRaw(raw_arr, size, kFrequency);
    }

    else if(command.equals("check_ir_on")){
      Serial.print("Sending test IR");
      Serial.println();
      uint16_t* arr = learnIR.readIRCode(1, "on");
      delay(2000);
      irSend.sendRaw(arr, size, kFrequency);
      Serial.print("Test IR Sent");
      Serial.println();
    }
    
    else{
      Serial.println(F("Not a valid command"));
    }
  }
}