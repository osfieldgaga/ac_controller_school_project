#ifndef THRESHOLD_BODY_DETECTION
#define THRESHOLD_BODY_DETECTION

class BodyDetection
{
private:
    /* data */
    
public:

    void initializeBodyDetector();
    int createMaskMatrix(float temperature);
    bool detectPresence(float temperature);
    bool detectPresenceV2();


};


#endif 
