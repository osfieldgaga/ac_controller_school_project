#include <BodyDetection.hpp>
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE], matrix_mask[AMG88xx_PIXEL_ARRAY_SIZE];

float temperature_offset = 0;
float lower_human_surface_temp = 32.5, higher_human_surface_temp = 37, lower_temp = 30;
uint8_t mask_sum;

uint8_t count = 12;
int sum = 0;

float roomTemperatureTolerance = 0.1;

float k_delta = 3;

void BodyDetection::initializeBodyDetector(){
    bool status = amg.begin();
    if (!status)
    {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        // while (1);
    }
    Serial.println();
    delay(100);
}

int BodyDetection::createMaskMatrix(float temperature){
    amg.readPixels(pixels);
    mask_sum = 0;
    Serial.println();
    Serial.println(F("Body detection's Mask Matrix"));
    //Serial.print("[");
    for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
    {
        //Serial.print(pixels[i - 1]);
        // if higher than threshold and lower than adjusted human surface temperature
        if (pixels[i - 1] >= (temperature - temperature_offset) && pixels[i - 1] <= (higher_human_surface_temp))
        {
            matrix_mask[i - 1] = 1;
            mask_sum++;
        }
        else
        {
            // anything else whether higher or lower is zero
            // this technocally makes sure that the measurement isn't affected by
            // hotter objects like appliances, laptops etc
            matrix_mask[i - 1] = 0;
        }
       // Serial.print(matrix_mask[i - 1]);
       // Serial.print(", ");
       // if (i % 8 == 0)
           // Serial.println();
    }
    //Serial.print("]");
    //Serial.println();
    //Serial.println();

    Serial.println(F("Matrix sum: "));
    Serial.print(mask_sum);
    Serial.print(F("      "));
    Serial.print((mask_sum * 100) / 64);
    Serial.print(F(" %"));

    Serial.println();

    return mask_sum;
}

bool BodyDetection::detectPresence(float temperature){
    float pixel_sum, pixel_avg;
    amg.readPixels(pixels);
    Serial.println();
    Serial.println(F("Body detection's (average method)"));
    //Serial.print("[");
    for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
    {
       pixel_sum += pixels[i - 1];
    }
    pixel_avg = pixel_sum / AMG88xx_PIXEL_ARRAY_SIZE;

    if(pixel_avg >= ((1 - roomTemperatureTolerance) * temperature) && pixel_avg <= ((1 + roomTemperatureTolerance) * temperature)){
        //average is within room temperature limits, seeing only stuff at
        //room temperature
        Serial.print(F("Average is "));
        Serial.print(pixel_avg);
        Serial.print(F(", room temperature is "));
        Serial.print(temperature);
        Serial.println();
        Serial.print(F("No one there."));
        Serial.println();
        Serial.println();
        return false;
    } else if (pixel_avg > ((1 + roomTemperatureTolerance) * temperature)){
        //average is higher than room temperature limits
        //likely seeing someone
        Serial.print(F("Average is "));
        Serial.print(pixel_avg);
        Serial.print(F(", room temperature is "));
        Serial.print(temperature);
        Serial.println();
        Serial.print(F("Someone is here."));
        Serial.println();
        Serial.println();
        return true;
    } else {
        // case where average is below the room temperature lower threshold
        //do nothing for now
        Serial.print(F("Average is "));
        Serial.print(pixel_avg);
        Serial.print(F(", room temperature is "));
        Serial.print(temperature);
        Serial.println();
        Serial.print(F("However, average is below the room temperature lower threshold\n"));
        Serial.print(F("Don't really know what to do here"));
        Serial.println();
        Serial.println();
        return false;
    }
    
}

bool BodyDetection::detectPresenceV2(){
    float pixel_sum, pixel_avg, higher_sum, higher_avg;
    float pixel_copy[64];
    int higher_count = 0;
    amg.readPixels(pixels);
    Serial.println();
    Serial.println(F("Body detection's (average method V2.0)"));
    //Serial.print("[");
    float min =  pixels[0], max =  pixels[0]; // set to the very first value
    for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
    {
       pixel_sum += pixels[i - 1];
        pixel_copy[i - 1] = pixels[i - 1];

        if(pixels[i - 1] >= max) {
            max = pixels[i - 1];
        }

        if(pixels[i - 1] <= min) {
            min = pixels[i - 1];
        }

    }
    pixel_avg = pixel_sum / AMG88xx_PIXEL_ARRAY_SIZE;

    for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
    {
       
       if(pixel_copy[i - 1] >= pixel_avg){
           Serial.print(pixel_copy[i - 1]);
           Serial.print(" ");
           higher_sum += pixel_copy[i - 1];
           higher_count++;
       }
    }
    Serial.println(higher_count);
    Serial.println();
    if(higher_count > 0){
        higher_avg = higher_sum / higher_count;
    }
    

    Serial.print("Delta is ");
    Serial.print(higher_avg - pixel_avg);
    Serial.println();
    Serial.print("Min is ");
    Serial.print(min);
    Serial.println();
    Serial.print("Max is ");
    Serial.print(max);
    Serial.println();
    Serial.print("Max - Min ");
    Serial.print(max - min);
    Serial.println();
    Serial.print("Higher avg is ");
    Serial.print(higher_avg);
    Serial.println();
    Serial.print("Avg is ");
    Serial.print(pixel_avg);
    Serial.println();

    return false; //dummy
}

