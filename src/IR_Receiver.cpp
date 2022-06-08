#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <IR_Receiver.hpp>

#include <ir_Samsung.h>
#include <ir_Tcl.h>
#include <ir_Midea.h>



// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
// Note: GPIO 14 won't work on the ESP32-C3 as it causes the board to reboot.
#ifdef ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 10;  // 14 on a ESP32-C3 causes a boot loop.
#else  // ARDUINO_ESP32C3_DEV
const uint16_t kRecvPin = 13;

#endif  // ARDUINO_ESP32C3_DEV

const uint16_t kSendPin = 14;


IRsend irsend(kSendPin);

const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 50;  // Milli-Seconds

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout);

IRSamsungAc irSamsung(kSendPin);
IRTcl112Ac irTCL(kSendPin);
IRMideaAC irMidea(kSendPin);

const uint16_t kFrequency = 38;

decode_results results, results2;

bool hasAlreadySent = false;

int IRReceiver::getPinNumber(){
    return kRecvPin;
}

void IRReceiver::initIR(){
    irrecv.enableIRIn(); 
    irsend.begin();
    irSamsung.begin();
    irTCL.begin();
    irMidea.begin();
}

void IRReceiver::turnOnSamsung(){
    irSamsung.sendOn();
    Serial.println("Samsung AC On");
    Serial.println();
}

void IRReceiver::turnOffSamsung(){
    irSamsung.sendOff();
    Serial.println("Samsung AC off");
    Serial.println();
    
}

void IRReceiver::turnOnTCL(){
    irTCL.on();
    irTCL.send();
    Serial.println("TCL AC On");
    Serial.println();
}

void IRReceiver::turnOffTCL(){
    irTCL.off();
    irTCL.send();
    Serial.println("TCL AC off");
    Serial.println();
    
}

void IRReceiver::setTCLTemp(int temp){
    irTCL.setTemp(temp);
    irTCL.send();
    Serial.print("TCL Temperature set at ");
    Serial.print(temp);
    Serial.println(" degrees");

    Serial.println();
}

void IRReceiver::turnOnMidea(){
    irMidea.on();
    irMidea.send();
    Serial.println("Midea AC on");
    Serial.println();
}

void IRReceiver::turnOffMidea(){
    irMidea.off();
    irMidea.send();
    Serial.println("Midea AC off");
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


void IRReceiver::decodeIR(){
  
  if (irrecv.decode(&results)) {  // Grab an IR code
        // Check if the buffer overflowed
        if (results.overflow || results2.overflow) {
            Serial.println(F("Overflow detected"));
            //Serial.println("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__);
            // see also https://github.com/Arduino-IRremote/Arduino-IRremote#modifying-compile-options-with-sloeber-ide
        } else {

           
            Serial.println();                               // 2 blank lines between entries
            Serial.println();
            Serial.println("Results 1");
            
            Serial.print("Bits: ");
            Serial.print(results.bits);
            Serial.println();
            Serial.print("Timing info: ");
            Serial.print(resultToTimingInfo(&results));

            delay(5000);
          
            Serial.println();
            Serial.print("Sending IR received");
            Serial.println();
            uint16_t *raw_array = resultToRawArray(&results);
  
            uint16_t size = getCorrectedRawLength(&results);
            irsend.sendRaw(raw_array, size, kFrequency); //send 38kHZ  

        }
        irrecv.resume();   
        
        
    }
        

}