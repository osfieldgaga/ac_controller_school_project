#ifndef SMARTTEMP
#define SMARTTEMP

class SmartTemperature
{
private:
    float pmv (float temperature, float humidity);
    float determineTemperature(float t, float h, float currentACTemp);
public:
    float getSmartTemp(float t, float h, float currentACTemp);
    float getPMV(float t, float h);
};



#endif
