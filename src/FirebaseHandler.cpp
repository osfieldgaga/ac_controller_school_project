#include <FirebaseHandler.hpp>
#include <Firebase_ESP_Client.h>
#include <UniqueIdentifiers.hpp>
#include <Prefs.hpp>
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
bool configDone = false;
String uid;

String path_to_DB = DEVICE_ID_1;
String path_to_config = path_to_DB + "/" + "configuration/";
String path_to_parameters = path_to_DB + "/parameters/";


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

    //sign up right after initialization to prevent annoying errors
    signUpUSer();
}

void FirebaseHandler::setUpDatabase(){

    //create state of the device
    String state_path = F("state");
    uint8_t currentStatOfAc = 0;
    Firebase.RTDB.setInt(&fbdo, path_to_DB + "/" + "state", currentStatOfAc);

    String temp_path = F("temperature");
    uint8_t currentTemp;
  
    Firebase.RTDB.setInt(&fbdo, path_to_DB + "/" + "temperature", currentTemp);

    String operationMode_path =  F("operation_mode");
    bool currentOpMode = false; //false = manual, true = auto
    Firebase.RTDB.setBool(&fbdo, path_to_DB + "/" + operationMode_path, currentOpMode);

    //set tup configuration path
    Firebase.RTDB.setString(&fbdo, path_to_config + "nickname", "[empty]");
    Firebase.RTDB.setString(&fbdo, path_to_config + "ac_brand", "[empty]");
    Firebase.RTDB.setString(&fbdo, path_to_config + "room_type", "[empty]");

    Firebase.RTDB.setBool(&fbdo, path_to_config + "configDone", configDone);

    //set tup parameters path
    Firebase.RTDB.setFloat(&fbdo, path_to_parameters + "room_temperature", 0.0f);
    Firebase.RTDB.setFloat(&fbdo, path_to_parameters + "outside_temperature", 0.0f);
    Firebase.RTDB.setFloat(&fbdo, path_to_parameters + "room_humidity", 0.0f);
    Firebase.RTDB.setBool(&fbdo, path_to_parameters + "room_occupancy", false);

    

    Serial.println(F("Created database (if not already there)"));
}

bool FirebaseHandler::checkStatus(){
    return Firebase.ready();
    
}

bool FirebaseHandler::checkIfConfigDone(){
    if (Firebase.RTDB.getBool(&fbdo, path_to_DB + "/configuration/configDone"))
    {
        configDone = fbdo.to<bool>();
    }
    else
    {
        Serial.println(F("Could not check config status. Reason: "));
        Serial.println(fbdo.errorReason());
        configDone = NULL;
    }

    return configDone; 
}

void FirebaseHandler::signUpUSer(){
    Firebase.signUp <std::string, std::string>(&config, &auth, USER_EMAIL, USER_PASSWORD);

    Serial.println(F("Getting User UID"));
    while ((auth.token.uid) == "")
    {
        Serial.print('.');
        delay(1000);
    }
    // Print user UID
    uid = auth.token.uid.c_str();
    Serial.print(F("User UID: "));
    Serial.print(uid);
    Serial.println();

    Serial.println(F("Signed up user"));
}

void FirebaseHandler::testSetValue(){
    testBool = !testBool;
    if(Firebase.RTDB.setBool(&fbdo, "test/bool", testBool)){

    }else{
        Serial.println(F("Could not write test value. Reason: "));
        Serial.println(fbdo.errorReason());
    }
    Serial.println();
    Serial.print(F("Wrote a value of "));
    Serial.println(testBool);
}

uint8_t FirebaseHandler::obtainACState(){
    uint8_t result_state;
    if (Firebase.RTDB.getInt(&fbdo, path_to_DB + "/" + "state"))
    {
        result_state = fbdo.to<int>();
    }
    else
    {
        Serial.println(F("Could not get AC state. Reason: "));
        Serial.println(fbdo.errorReason());
        result_state = NULL;
    }

    return result_state;
}

uint8_t FirebaseHandler::obtainACTemperature(){
    uint8_t temperature;
    if(Firebase.RTDB.getInt(&fbdo, path_to_DB + "/" + "temperature")){
        temperature = fbdo.to<int>();
    }else{
        Serial.println(F("Could not get AC temperature. Reason: "));
        Serial.println(fbdo.errorReason());
        temperature = -1;
    }

    return temperature;
}

void FirebaseHandler::writeACTemp(int temperature){
    if(Firebase.RTDB.setInt(&fbdo, path_to_DB + "/" + "temperature", temperature)){
       
    }else{
        Serial.println(F("Could not set AC temperature. Reason: "));
        Serial.println(fbdo.errorReason());
    }
}

bool FirebaseHandler::onbtainOperationMode(){
    bool opMode;
    if(Firebase.RTDB.getBool(&fbdo, path_to_DB + "/" + "operation_mode")){
        opMode = fbdo.to<bool>();
    }else{
        Serial.println(F("Could not get AC operation mode. Reason: "));
        Serial.println(fbdo.errorReason());
        opMode = NULL;
    }

    return opMode;
}

void FirebaseHandler::obtainACConfiguration(){
    String ac_brand, nickname, room_type;
    if(Firebase.RTDB.getString(&fbdo, path_to_config + "ac_brand")){
        ac_brand = fbdo.to<String>();

    } else {
        Serial.print(F("Couldn't fetch AC Brand. Reason: "));
        ac_brand = "";
        Serial.println(fbdo.errorReason());
    }

    if(Firebase.RTDB.getString(&fbdo, path_to_config + "nickname")){
        nickname = fbdo.to<String>();
        
    } else {
        Serial.print(F("Couldn't fetch AC Nickname. Reason: "));
        nickname = "";
        Serial.println(fbdo.errorReason());
    }

    if(Firebase.RTDB.getString(&fbdo, path_to_config + "room_type")){
        room_type = fbdo.to<String>();

    } else {
        room_type = "";
        Serial.print(F("Couldn't fetch Room Type. Reason: "));
        Serial.println(fbdo.errorReason());
    }

    Prefs::setACConfig(ac_brand, nickname, room_type);
    
}

void FirebaseHandler::sendRoomTelemetry(float temperature, float humidity){

    //TODO check when it wasn't able to write to DB

    Firebase.RTDB.setFloat(&fbdo, path_to_parameters + "room_temperature", temperature);
    //Firebase.RTDB.setFloat(&fbdo, path_to_parameters + "outside_temperature", 0.0f);
    Firebase.RTDB.setFloat(&fbdo, path_to_parameters + "room_humidity", humidity);
    //Firebase.RTDB.setBool(&fbdo, path_to_parameters + "room_occupancy", false);
}
