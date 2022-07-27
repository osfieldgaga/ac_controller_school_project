#ifndef IR_RECEIVER
#define IR_RECEIVER

class IRReceiver{
    public:
        uint16_t size;

        void initIR();
        uint16_t* decodeIR();
        uint16_t getArraySize();
        int getPinNumber();
        void turnOnSamsung();
        void turnOffSamsung();
        void turnOnAC();
        void turnOffAC();
        void turnOnTCL();
        void turnOffTCL();
        void setTCLTemp(uint8_t temp);
        void setACTemp(uint8_t temp);
        void turnOnMidea();
        void turnOffMidea();
    private:

};

#endif