#include <Arduino.h>
#include <eeprom_manager.hpp>
#include <IR_Receiver.hpp>
#include <IR_Sender.hpp>
#include <WiFiHandler.hpp>
#include <UniqueIdentifiers.hpp>

#include <ESP8266WiFi.h>
//#include <FirebaseHandler.hpp>

//#include <Adafruit_Sensor.h>
#include <DHT.h>

//Constants
#define DHTPIN 12     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
int chk;
float hum;  //Stores humidity value
float temp;
int dht_millis; //Stores temperature value


EEPROM_Manager eeprom;
IRReceiver irReceiver;
IRSender irSender;

WiFiHandler wifiHandler;
//FirebaseHandler firebaseHandler;


const int kFirebaseRefreshRate = 2000; //2000ms = 2 seconds
unsigned long now = 0;
int prevACState;

void setup() {
  // put your setup code here, to run once:
  eeprom.initializeMemory(); //compulsory

  //set up stuff for IR stuff
  Serial.begin(9600);
  irReceiver.initIR(); // Start the receiver

  delay(5000);

  wifiHandler.establishWiFi();
  //firebaseHandler.connectFirebase();
  //firebaseHandler.setUpDatabase();

  //prevACState = firebaseHandler.obtainACState();
  
  dht.begin();
  dht_millis = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(millis() - dht_millis > 5000){
    dht_millis = millis();
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor

    
    
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
  }


  // if(firebaseHandler.checkStatus() && millis() - now > kFirebaseRefreshRate){
  //   now = millis();
  //   // do sonething related to firebase, this part is triggered only when the device is authenticated and when
  //   // the threshold time is reached


  //     //check if there has been a odification in the database data, which would mean a change in state
  //     //if it's the case, change the state
  //     if(firebaseHandler.obtainACState() != prevACState){
  //       if (firebaseHandler.obtainACState() == 1){
  //         //when 1, the state of the AC is on
  //         irReceiver.turnOnSamsung();
  //       }else {
  //         //normally if it's 0, Ac is off
  //         irReceiver.turnOnSamsung();
  //       }
        

  //       prevACState = firebaseHandler.obtainACState();
  //     }

      
  //   }

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