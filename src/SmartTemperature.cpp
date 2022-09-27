#include <SmartTemperature.hpp>
#include <stdlib.h>
//#include <pmv_model.h>
#include <Arduino.h>
//#include <pmv_tree_model_lite.h>
#include <pmv_tree_model_optimized.h>


Eloquent::ML::Port::DecisionTreeRegressor clf;


#define k_lowerTempLimit 21
#define k_upperTempLimit 27
#define k_PMVLowerLimit -0.5
#define k_PMVUpperLimit 0.5

float k_ClothingInsulation = 0.55;
float k_MonthlyTemp = 27.5; //81.5F
float k_AirVelocity = 0.1;

float pmv_array[5] = {0, 0, 0, 0, 0};
float pmv_array_buff [4] = {0, 0, 0, 0};
int number_of_pmv_checks = 0;

float SmartTemperature::pmv(float temperature, float humidity){
    //float data[5] = {temperature, humidity, k_ClothingInsulation, k_AirVelocity, k_MonthlyTemp};
    float data[4] = {temperature, humidity, k_ClothingInsulation, k_MonthlyTemp};
    float pmv = clf.predict(data);

   // test variable
    // float dummy_pmv = 0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.0))); //subtract three to offset it to the left
    // pmv = dummy_pmv - 3;

    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println(F("Failed to read from DHT sensor!\nNo PMV either!"));
    }
    else
    {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println();

        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println();

        Serial.println();
        Serial.print("PMV is: ");
        Serial.print(pmv);
        Serial.println();
        return pmv;
    }
}

//Adjust temperature based on the PMV calculated. If too hot, reduce temperature by 1 C, if too cold, increase temerature by 1 C
float SmartTemperature::determineTemperature(float t, float h, float currentACTemp) {
    float smartTemp = currentACTemp;

    
    float pmv_var = pmv(t, h);
    /*
        do some magic
    */
   
//    Serial.print("Current PMV is ");
//    Serial.print(pmv_var);
   Serial.print("Previous PMVs are ");
    
   for(int i = 0; i < 4; i++){
       pmv_array_buff[i] = pmv_array[i];   
   }
   for(int j = 1; j < 5; j++){
       pmv_array[j] = pmv_array_buff[j - 1];

       Serial.print(pmv_array[j]);
       if(j != 4) { Serial.print(", "); }
   }
   pmv_array[0] = pmv_var;
   Serial.println();
   Serial.print("PMV array: [");
   Serial.print(pmv_array[0]);
   Serial.print(", ");
   Serial.print(pmv_array[1]);
   Serial.print(", ");
   Serial.print(pmv_array[2]);
   Serial.print(", ");
   Serial.print(pmv_array[3]);
   Serial.print(", ");
   Serial.print(pmv_array[4]);
   Serial.print("]\n");
   number_of_pmv_checks++;

   if(pmv_array[0] > k_PMVUpperLimit){ //if PMV > 0.5
       smartTemp -= 1;

       if (smartTemp < k_lowerTempLimit){
           smartTemp = k_lowerTempLimit;
       }

   } else if (pmv_array[0] < k_PMVLowerLimit){ //if PMV < -0.5
       smartTemp += 1;

        if (smartTemp > k_upperTempLimit){
            smartTemp = k_upperTempLimit;
       }
   } else {

       if (number_of_pmv_checks >= 3) {
           //it is implied that pmv_array[0] is within the required range
           if (pmv_array[1] >= k_PMVLowerLimit && pmv_array[1] <= k_PMVUpperLimit) {
               if (pmv_array[2] >= k_PMVLowerLimit && pmv_array[2] <= k_PMVUpperLimit) {
                   // last three PMV were within stable limit so increase temp
                   // if PMV is stable for long enough, increase temperature in an attempt
                   // to reduce consumption
                   smartTemp += 1;
                   if (smartTemp > k_upperTempLimit)
                   {
                       smartTemp = k_upperTempLimit;
                   }
                   Serial.print("Stable PMV for 3 measurements, changing temp from ");
                   Serial.print(currentACTemp);
                   Serial.print(" to ");
                   Serial.print(smartTemp);
                   Serial.println();
               }
               else{
                   Serial.print("PMV not stable yet, temperature remains the same.");
                   Serial.println();
                   smartTemp = currentACTemp;
               }
           } else {
               Serial.print("PMV not stable yet, temperature remains the same.");
               Serial.println();
               smartTemp = currentACTemp;
           }
       } else {
           Serial.print("PMV but not stable yet, temperature remains the same.");
           Serial.println();
           smartTemp = currentACTemp;
       }
   }


   return smartTemp;
}


/*--------- Public functions (just to return stuff) --------*/

float SmartTemperature::getSmartTemp(float t, float h, float currentACTemp){
    /*
        do some magic
        use float pmv = pmv(t, h) and
        determineTemperature(currentACTemp, pmv);
    */

   // calculate PMV and store it somwhere
   //b_temp = determineTemperature(currentACTemp, pmv_var);
   return determineTemperature(t, h, currentACTemp);
   

//    smartTemp = b_temp;
//    return smartTemp;
}

float SmartTemperature::getPMV(float t, float h){
    return pmv(t, h);
}