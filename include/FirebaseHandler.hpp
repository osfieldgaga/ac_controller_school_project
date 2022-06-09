#ifndef FIREBASE_HANDLER
#define FIREBASE_HANDLER

#include <string>

// #define FIREBASE_HOST "smart-ac-controller-edf1e-default-rtdb.firebaseio.com/"
#define DATABASE_URL "smart-ac-controller-edf1e-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "ktup889h9bEcm00vaYgPqlBAvCZpNRHpkPgbbY7T"
#define API_KEY "AIzaSyDl7N2gMPRtImEnUrBgZ5dSBp4_2v3Xj58"

#define USER_EMAIL "testdevice1@s.ac.controller.com"
#define USER_PASSWORD "1234567890"

class FirebaseHandler{
    public:
        void connectFirebase();
        bool checkStatus();
        void signUpUSer();
        void testSetValue();
        void setUpDatabase();
        int obtainACState();
        int obtainACTemperature();
};

#endif // MATH_H_INCLUDED