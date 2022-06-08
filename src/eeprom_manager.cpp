#include <Arduino.h>
#include <eeprom_manager.hpp>
#include <EEPROM.h>
#include <iostream>
#include <string>
#include <map>
#include <constants.hpp>

using namespace std;

// #define ESP12E_MEM_SIZE 4096
// #define STARTING_ADDR 20
// #define INSTRUCTION_SET_SIZE 20



EEPROM_Manager::EEPROM_Manager(){
    
}

void EEPROM_Manager::initializeMemory(){
    EEPROM.begin(ESP12E_MEM_SIZE);
        //Serial.print("Memory Initialized");
    

    EEPROM.commit(); //copy changes from RAM to actual flash memoryi
}


void EEPROM_Manager::writeToMem(int acID, string instructionLabel, unsigned long instructionData){
        EEPROM.write(AC_list[acID + instruction_displacement[instructionLabel]], instructionData);
        //what does this do exactly?
        //the starting address of the instruction of each AC is stored in the table AC_list
        //the map instruction_displacement contains the distance from the first instruction
        //we write at the starting address+distance, which is the slot for that specific instruction
}

void EEPROM_Manager::readFromMem(int acID, string instructionLabel){
        EEPROM.read(AC_list[acID + instruction_displacement[instructionLabel]]);
}


        


