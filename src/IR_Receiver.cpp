#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <IR_Receiver.hpp>
#include <Prefs.hpp>
#include <LearnIR.hpp>

#include <ir_Samsung.h>
#include <ir_Tcl.h>
#include <ir_Midea.h>
#include <ir_Gree.h>


//TODO import supported AC libraries

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.
#ifdef ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 10;  // 14 on a ESP32-C3 causes a boot loop.
#else  // ARDUINO_ESP32C3_DEV
const uint8_t kRecvPin = 17;

#endif  // ARDUINO_ESP32C3_DEV

const uint8_t kSendPin = 16;

LearnIR irLearner;
IRsend irsend(kSendPin);

const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 50;  // Milli-Seconds

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout);

IRSamsungAc irSamsung(kSendPin);
IRTcl112Ac irTCL(kSendPin);
IRMideaAC irMidea(kSendPin);

const uint8_t kFrequency = 38;

decode_results results;

bool hasAlreadySent = false;

int IRReceiver::getPinNumber(){
    return kRecvPin;
}



void IRReceiver::initIR(){
    irrecv.enableIRIn(); 
    irsend.begin();

    //initialize the senders for each brand
    irSamsung.begin();
    irTCL.begin();
    irMidea.begin();
}

void IRReceiver::turnOnSamsung(){
    irSamsung.sendOn();
    Serial.println(F("Samsung AC On"));
    Serial.println();
}

void IRReceiver::turnOffSamsung(){
    irSamsung.sendOff();
    Serial.println(F("Samsung AC off"));
    Serial.println();
    
}

//List of the different brands and their prefs ID: (all of them will always be in small letters)
//Samsung = samsung
//TCL = tcl
//Midea = midea


//quick and easy solution is to use an if else tree
//whenever you want to add the corresponding fonctin of an AC,
//create an extra else if block and add the code you need.
void IRReceiver::turnOnAC()
{
    if (Prefs::getACBrand().equals("samsung"))
    {
        irSamsung.sendOn();
        Serial.print(F("Samsung")); // this will complete the last prints at the end of the case statement
                                    // to for a complete sentence with the AC brand
    }

    else if (Prefs::getACBrand().equals("tcl"))
    {
        irTCL.on();
        irTCL.send();
        Serial.print(F("TCL"));
    }

    else
    {
        Serial.print(F("ERROR: Trying to turn ON AC but no case fir the Prefs ID provided, it seems to be wrong, check it. ID is: "));
        Serial.print(Prefs::getACBrand());
        Serial.println();
    }

    Serial.println(F(" AC ON"));
    Serial.println();
}

//quick and easy solution is to use an if else tree
//whenever you want to add the corresponding fonctin of an AC,
//create an extra else if block and add the code you need.
void IRReceiver::turnOffAC()
{ 

    if (Prefs::getACBrand().equals("samsung"))
    {
        irSamsung.sendOff();
        Serial.print(F("Samsung")); // this will complete the last prints at the end of the case statement
                                    // to for a complete sentence with the AC brand
    }

    else if (Prefs::getACBrand().equals("tcl"))
    {
        irTCL.off();
        irTCL.send();
        Serial.print(F("TCL"));
    }

    else
    {
        Serial.print(F("ERROR: Trying to turn OFF AC but no case fir the Prefs ID provided, it seems to be wrong, check it. ID is: "));
        Serial.print(Prefs::getACBrand());
        Serial.println();
    }

    Serial.println(F(" AC OFF"));
    Serial.println();
}


//quick and easy solution is to use an if else tree
//whenever you want to add the corresponding fonctin of an AC,
//create an extra else if block and add the code you need.
void IRReceiver::setACTemp(uint8_t temp)
{

    // check AC Brand from saved preferences
    if (Prefs::getACBrand().equals("samsung"))
    {
        irSamsung.setTemp(temp);
        irSamsung.send();
        Serial.print(F("Samsung")); // this will complete the last prints at the end of the case statement
                                    // to for a complete sentence with the AC brand
    }

    else if (Prefs::getACBrand().equals("tcl"))
    {
        irTCL.setTemp(temp);
        irTCL.send();
        Serial.print(F("TCL"));
    }

    else
    {
        Serial.print(F("ERROR: Trying to change temperature but no case fir the Prefs ID provided, it seems to be wrong, check it. ID is"));
        Serial.print(Prefs::getACBrand());
        Serial.println();
    }

    Serial.print(F(" AC Temperature set at "));
    Serial.print(temp);
    Serial.println(F(" degrees"));

    Serial.println();
}

void IRReceiver::turnOnTCL(){
    irTCL.on();
    irTCL.send();
    Serial.println(F("TCL AC On"));
    Serial.println();
}

void IRReceiver::turnOffTCL(){
    irTCL.off();
    irTCL.send();
    Serial.println(F("TCL AC off"));
    Serial.println();
    
}

void IRReceiver::setTCLTemp(uint8_t temp){
    irTCL.setTemp(temp);
    irTCL.send();
    Serial.print(F("TCL Temperature set at "));
    Serial.print(temp);
    Serial.println(F(" degrees"));

    Serial.println();
}

void IRReceiver::turnOnMidea(){
    irMidea.on();
    irMidea.send();
    Serial.println(F("Midea AC on"));
    Serial.println();
}

void IRReceiver::turnOffMidea(){
    irMidea.off();
    irMidea.send();
    Serial.println(F("Midea AC off"));
    Serial.println();
}

//draft of function control the AC
// void IRReceiver::turnOnAC(string brand){
//     switch (brand)
//     {
//     case kSamsungBrand:
//         //code to turn on Samsung AC
//         break;

//     case kTCLBrand:
//         //code to turn on TCL AC
//         break;
    
//     default:
//         break;
//     }
// }


uint16_t* IRReceiver::decodeIR(){
  uint16_t *raw_array;
  if (irrecv.decode(&results)) {  // Grab an IR code
        // Check if the buffer overflowed
        if (results.overflow) {
            Serial.println(F("Overflow detected"));
            //Serial.println("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__);
            // see also https://github.com/Arduino-IRremote/Arduino-IRremote#modifying-compile-options-with-sloeber-ide
        } else {
            
            Serial.println();
            Serial.println(F("Results"));
            
            Serial.print(F("Bits: "));
            Serial.print(results.bits);
            Serial.println();
            Serial.print(F("Timing info: "));
            Serial.print(resultToTimingInfo(&results));

             delay(5000);
          
             Serial.println();
             Serial.print(F("Sending IR received"));
             Serial.println();
            raw_array = resultToRawArray(&results);
            size = getCorrectedRawLength(&results);

            // for(int i = 0; i<size; i++){
            //     Serial.print(raw_array[i]);
            //     Serial.print(" ");
            // }

            //irLearner.storeIRCode(raw_array, size, "on", 1);
            
          
            irsend.sendRaw(raw_array, size, kFrequency); //send 38kHZ  
            Serial.print(F("IR Sent"));
            Serial.println();
            //irsend.sendRaw(irLearner.readIRCode(1, "on"), size, kFrequency); //send 38kHZ  
            
        }
        irrecv.resume();   
        return raw_array;
        
    }else {
        return NULL;
    }
        

}

uint16_t IRReceiver::getArraySize(){
    return size;
}