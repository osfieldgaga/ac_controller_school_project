#ifndef IR_RECEIVER
#define IR_RECEIVER

class IRReceiver{
    public:
        void initIR();
        void decodeIR();
        int getPinNumber();
        void turnOnSamsung();
        void turnOffSamsung();

        void turnOnTCL();
        void turnOffTCL();
        void setTCLTemp(uint8_t temp);
        void turnOnMidea();
        void turnOffMidea();
    private:

};

#endif