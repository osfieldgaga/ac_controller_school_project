#include <Prefs.hpp>
#include <LearnIR.hpp>
#include <IR_Receiver.hpp>


#include <string>

#define k_NumberOfAC 1
#define k_NumberOfInstructions 17

Prefs fsIR; //fsIR -> file system for IR
IRReceiver irRcv;

void LearnIR::storeIRCode (const uint16_t ir_array[], const uint16_t ir_size, std::string instruction_id,  int ac_id){
    
    
    std::string ir_array_to_store;
    for (int i = 0; i < ir_size; i++)
    {
        //simply copy each element to a local array
        ir_array_to_store.append(std::to_string((int)(ir_array[i])));
        ir_array_to_store.append(" ");
        // if(i == ir_size - 1){
        //     ir_array_to_store.append(";");        
        // }
    }


    //instruction_id.erase(((instruction_id.length())-1));
    
    // if(instruction_id != "on" || instruction_id != "off" || instruction_id != "temp16"){
    //     Serial.println();
    //     Serial.print(F("ERROR: Couldn't save instruction. Instruction ID isn't correct.\nInstruction received: "));
    //     Serial.print(F("'"));
    //     Serial.print(instruction_id.c_str());
    //     Serial.print(F("'"));
    //     Serial.println();
    // } else {

    Serial.print(F("\nInstruction received: "));
    Serial.print(F("'"));
    Serial.print(instruction_id.c_str());
    Serial.print(F("'"));
    std::string file_path = "/ac" + std::to_string(ac_id) + "_" + instruction_id;
    std::string size_file_path = "/ac" + std::to_string(ac_id) + "_" + instruction_id + "_size";

    
    Serial.println();
    Serial.print(F("Storing IR code at "));
    Serial.print(file_path.c_str());
    Serial.println();
    // Serial.print("IR code: ");
    // Serial.print(ir_array_to_store.c_str());
    // Serial.println();
    //}

    fsIR.writeFile(SPIFFS, file_path.c_str(), ir_array_to_store.c_str());
    fsIR.writeFile(SPIFFS, size_file_path.c_str(), (std::to_string(ir_size) + ";").c_str());
}

uint16_t* LearnIR::readIRCode(int ac_id, std::string instruction_id){
    std::string file_path = "/ac" + std::to_string(ac_id) + "_" + instruction_id;
    std::string size_file_path = "/ac" + std::to_string(ac_id) + "_" + instruction_id + "_size";

    String file_content = fsIR.readFile(SPIFFS, file_path.c_str());
    Serial.println();
    //int array_size = fsIR.readFile(SPIFFS, size_file_path.c_str()).toInt();

    String array_size = fsIR.readFile(SPIFFS, size_file_path.c_str());
    array_size.remove(array_size.length() - 1);
    Serial.println();
    Serial.print("\nArray Size: ");
    Serial.print(array_size.toInt());
    Serial.println();


    //terrible things happenin here
    uint16_t raw_array[array_size.toInt()];
    for (int i = 0; i < array_size.toInt(); i++)
    {
        int pos = file_content.indexOf(" ");
        Serial.println();
       
    //uncomment if you want to visualize how it works
        // Serial.print("Content of file, one by one: ");
        // Serial.print(file_content);
        // Serial.println();
        // Serial.print("Index: ");
        // Serial.print(i);
        // Serial.println();
        // Serial.print("Content: ");
        // Serial.print((uint16_t)file_content.substring(0, pos).toInt());

        //simplyput the content in the array that will ne used to send the IR code
        raw_array[i] = (uint16_t)file_content.substring(0, pos).toInt();

        file_content.remove(0, pos + 1);
    }   
    
    //test return value

    return raw_array;


}

//this part of the code is the least optimized lmao sorry
//should normally use a for loop (i think?) but my brain is too tired, bear with me!
//it works at least :p
void LearnIR::learnIRSequence(int ac_id)
{
    Serial.print("Initiating IR learning sequence\nWaiting for IR.");
    bool done = false;
    int current_state = 0; // 0 = on, 1 = off, 2 to 15 = temp16 to temp30
    int array_size;
    do
    {
        uint16_t *raw_array = irRcv.decodeIR();
    
        if (raw_array != NULL)
        {
            Serial.print("IR Received.\n");
            array_size = irRcv.getArraySize();
            switch (current_state)
            {
            case 0: // reading on code, waiting for user to input it
                Serial.print("Received and storing ON code");
                storeIRCode(raw_array, array_size, "on", ac_id);
                Serial.print("Stored ON\n\n");
                current_state = 1;
                Serial.print("Next up, OFF signal\n");
                break;
            case 1: 
                Serial.print("Received and storing OFF code");
                storeIRCode(raw_array, array_size, "off", ac_id);
                Serial.print("Stored OFF\n\n");
                current_state = 2;
                Serial.print("Next up, temp16 signal\n");
                break;
            case 2:
                Serial.print("Received and storing temp16 code");
                storeIRCode(raw_array, array_size, "temp16", ac_id);
                Serial.print("Stored temp16\n\n");
                current_state = 3;
                Serial.print("Next up, temp17 signal\n");
                // done = true;
                break;
            case 3:
                Serial.print("Received and storing temp17 code");
                storeIRCode(raw_array, array_size, "temp17", ac_id);
                Serial.print("Stored temp17\n\n");
                current_state = 4;
                Serial.print("Next up, temp18 signal\n");
                // done = true;
                break;
            case 4:
                Serial.print("Received and storing temp18 code");
                storeIRCode(raw_array, array_size, "temp18", ac_id);
                Serial.print("Stored temp18\n\n");
                current_state = 5;
                Serial.print("Next up, temp19 signal\n");
                // done = true;
                break;
             case 5:
                Serial.print("Received and storing temp19 code");
                storeIRCode(raw_array, array_size, "temp19", ac_id);
                Serial.print("Stored temp19\n\n");
                current_state = 6;
                Serial.print("Next up, temp20 signal\n");
                // done = true;
                break;
             case 6:
                Serial.print("Received and storing temp20 code");
                storeIRCode(raw_array, array_size, "temp20", ac_id);
                Serial.print("Stored temp20\n\n");
                current_state = 7;
                Serial.print("Next up, temp21 signal\n");
                // done = true;
                break;
             case 7:
                Serial.print("Received and storing temp21 code");
                storeIRCode(raw_array, array_size, "temp21", ac_id);
                Serial.print("Stored temp21\n\n");
                current_state = 8;
                Serial.print("Next up, temp22 signal\n");
                // done = true;
                break;
             case 8:
                Serial.print("Received and storing temp22 code");
                storeIRCode(raw_array, array_size, "temp22", ac_id);
                Serial.print("Stored temp22\n\n");
                current_state = 9;
                Serial.print("Next up, temp23 signal\n");
                // done = true;
                break;
             case 9:
                Serial.print("Received and storing temp23 code");
                storeIRCode(raw_array, array_size, "temp23", ac_id);
                Serial.print("Stored temp23\n\n");
                current_state = 10;
                Serial.print("Next up, temp24 signal\n");
                // done = true;
                break;
             case 10:
                Serial.print("Received and storing temp24 code");
                storeIRCode(raw_array, array_size, "temp24", ac_id);
                Serial.print("Stored temp24\n\n");
                current_state = 11;
                Serial.print("Next up, temp25 signal\n");
                // done = true;
                break;
             case 11:
                Serial.print("Received and storing temp25 code");
                storeIRCode(raw_array, array_size, "temp25", ac_id);
                Serial.print("Stored temp25\n\n");
                current_state = 12;
                Serial.print("Next up, temp26 signal\n");
                // done = true;
                break;
             case 12:
                Serial.print("Received and storing temp26 code");
                storeIRCode(raw_array, array_size, "temp26", ac_id);
                Serial.print("Stored temp26\n\n");
                current_state = 13;
                Serial.print("Next up, temp27 signal\n");
                // done = true;
                break;
             case 13:
                Serial.print("Received and storing temp27 code");
                storeIRCode(raw_array, array_size, "temp27", ac_id);
                Serial.print("Stored temp27\n\n");
                current_state = 14;
                Serial.print("Next up, temp28 signal\n");
                // done = true;
                break;
             case 14:
                Serial.print("Received and storing temp28 code");
                storeIRCode(raw_array, array_size, "temp28", ac_id);
                Serial.print("Stored temp28\n\n");
                current_state = 15;
                Serial.print("Next up, temp29 signal\n");
                // done = true;
                break;
             case 15:
                Serial.print("Received and storing temp29 code");
                storeIRCode(raw_array, array_size, "temp29", ac_id);
                Serial.print("Stored temp29\n\n");
                current_state = 16;
                Serial.print("Next up, temp30 signal\n");
                // done = true;
                break;
             case 16:
                Serial.print("Received and storing temp30 code");
                storeIRCode(raw_array, array_size, "temp30", ac_id);
                Serial.print("Stored temp30\n\n");
                current_state = 99;
                Serial.print("Next up, nothing.\n");
                done = true;
                break;

            default:

                break;
            }
        }

    } while (!done);

    Serial.println();
    Serial.print("Done learning, get back to your shit\nHere's the list of files: ");
    fsIR.listDir(SPIFFS, "/", 0);
}

void LearnIR::clearAll(){
    for (int i = 1; i < k_NumberOfAC + 1; i++)
    {
        for (int j = 0; j < k_NumberOfInstructions; j++)
        {
            std::string file_path = "/ac" + std::to_string(i) + "_" + generateInstructionIDString(j);
            std::string size_file_path = "/ac" + std::to_string(i) + "_" +  generateInstructionIDString(j) + "_size";
            fsIR.deleteFile(SPIFFS, file_path.c_str());
            fsIR.deleteFile(SPIFFS, size_file_path.c_str());
        }     
    }   
}

std::string LearnIR::generateInstructionIDString(int instruction_id){

    std::string instr_id_str;
    switch (instruction_id)
    {
    case 0:
        instr_id_str = "on";
        break;
    case 1:
        instr_id_str = "off";
        break;
    case 2:
        instr_id_str = "temp16";
        break;
    case 3:
        instr_id_str = "temp17";
        break;
    case 4:
        instr_id_str = "temp18";
        break;
    case 5:
        instr_id_str = "temp19";
        break;
    case 6:
        instr_id_str = "temp20";
        break;
    case 7:
        instr_id_str = "temp21";
        break;
    case 8:
        instr_id_str = "temp22";
        break;
    case 9:
        instr_id_str = "temp23";
        break;
    case 10:
        instr_id_str = "temp24";
        break;
    case 11:
        instr_id_str = "temp25";
        break;
    case 12:
        instr_id_str = "temp26";
        break;
    case 13:
        instr_id_str = "temp27";
        break;
    case 14:
        instr_id_str = "temp28";
        break;
    case 15:
        instr_id_str = "temp29";
        break;
    case 16:
        instr_id_str = "temp30";
        break;
    
    
    default:
        break;
    }

    return instr_id_str;
}