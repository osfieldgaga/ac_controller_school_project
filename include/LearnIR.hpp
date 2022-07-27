#ifndef LEARNIR
#define LEARNIR



#endif 

class LearnIR
{
private:
    std::string generateInstructionIDString(int instruction_id);
public:
    void storeIRCode (const uint16_t ir_array[], const uint16_t ir_size, std::string instruction_id,  int ac_id);
    uint16_t* readIRCode (int ac_id, std::string instruction_id);
    void learnIRSequence(int ac_id);
    void clearAll();
    //declare the getIRCode function
};
