#include <FirebaseHandler.hpp>
#include <Firebase_ESP_Client.h>
#include <UniqueIdentifiers.hpp>
// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>


// Define the Firebase Data object
FirebaseData fbdo;

// Define the FirebaseAuth data for authentication data
FirebaseAuth auth;

// Define the FirebaseConfig data for config data
FirebaseConfig config;


bool testBool = false;

String path_to_DB = DEVICE_ID_1;


void FirebaseHandler::connectFirebase(){
    Serial.println();
    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // Assign the project host and api key 
    // config.host = FIREBASE_HOST;

    config.api_key = API_KEY;
    /* Assign the RTDB URL (required) */
    config.database_url = DATABASE_URL;

    Firebase.begin(&config, &auth);

    // Comment or pass false value when WiFi reconnection will control by your code or third party library
    Firebase.reconnectWiFi(true);
}

void FirebaseHandler::setUpDatabase(){
    
    //path_to_DB = path_to_DB + "/";

    //create state of the device
    String state_path = "state";
    int currentStatOfAc = 0;
    Firebase.RTDB.setInt(&fbdo, path_to_DB + "/" + "state", currentStatOfAc);

    String temp_path = "temperature";
    int currentTemp;
    //just make sure you're getting an aactual value, and always start with the previous state
    if(Firebase.RTDB.getInt(&fbdo, path_to_DB + "/" + "temperature") == NULL){
        currentTemp = 25;
    }else {
        currentTemp = Firebase.RTDB.getInt(&fbdo, path_to_DB + "/" + "temperature");
    }
    
    Firebase.RTDB.setInt(&fbdo, path_to_DB + "/" + "temperature", currentTemp);

    Serial.println("Created database (if not already there)");
}

bool FirebaseHandler::checkStatus(){
    return Firebase.ready();
    
}

void FirebaseHandler::signUpUSer(){
    Firebase.signUp <std::string, std::string>(&config, &auth, USER_EMAIL, USER_PASSWORD);

}

void FirebaseHandler::testSetValue(){
    testBool = !testBool;
    Firebase.RTDB.setBool(&fbdo, "test/bool", testBool);
    Serial.println();
    Serial.print("Wrote a value of ");
    Serial.println(testBool);
}

int FirebaseHandler::obtainACState(){
    int currentState = Firebase.RTDB.getInt(&fbdo, path_to_DB + "/" + "state");
    // if(currentState == 1){
    //     Serial.println("AC is currently on");
    // }else {
    //      Serial.println("AC is currently off");
    // }
    //Serial.print(currentState);
    return currentState;
}
