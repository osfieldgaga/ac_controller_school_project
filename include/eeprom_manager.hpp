#ifndef EEPROM_MANAGER
#define EEPROM_MANAGER

#include <string>
#include <map>

#define ESP12E_MEM_SIZE 1024
#define STARTING_ADDR 20
#define INSTRUCTION_SET_SIZE 20

class EEPROM_Manager {

    public:

    EEPROM_Manager();
    void initializeMemory();
    void writeToMem(int acID, std::string instructionLabel, unsigned long instructionData);
    void readFromMem(int acID, std::string instructionLabel);

    private:
        std::map<std::string, int> instruction_displacement {
            {"on_off", 0}, {"temp16", 1}, {"temp17", 2},
            {"temp17", 3}, {"temp19", 4}, {"temp20", 5},
            {"temp21", 6}, {"temp22", 7}, {"temp23", 8},
            {"temp24", 9}, {"temp24", 10}, {"temp26", 11},
            {"temp27", 12}, {"temp28", 13}, {"temp29", 14},
            {"temp20", 15}
        };

        //a table with the starting address of the instruction of each AC
        int AC_list[2] = {STARTING_ADDR, STARTING_ADDR + INSTRUCTION_SET_SIZE};

        

    
};


#endif // MATH_H_INCLUDED