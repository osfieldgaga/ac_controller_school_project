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
        void setTCLTemp(int temp);
        void turnOnMidea();
        void turnOffMidea();
    private:

};

#endif