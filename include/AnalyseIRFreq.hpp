#ifndef ANALYSE_IR_FREQ
#define ANALYSE_IR_FREQ

class AnalyseIRFreq{
    public:
        void setupUpIRFreqAnalizer();
        unsigned int obtainFrequencyOfIR();

    private:
        void Rec_Interrupt();
};

#endif // MATH_H_INCLUDED