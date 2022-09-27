// #include <Wire.h>
// #include <Adafruit_AMG88xx.h>

// Adafruit_AMG88xx amg;
// float pixels[AMG88xx_PIXEL_ARRAY_SIZE], pixels_minus_noise[AMG88xx_PIXEL_ARRAY_SIZE], matrix_mask[AMG88xx_PIXEL_ARRAY_SIZE];

// #include "DHT.h"

// #define DHTPIN 4
// #define DHTTYPE DHT22

// DHT dht(DHTPIN, DHTTYPE);

// // TODO change it to check for the range
// //   30 > temp > 37 => matrix 1
// //    else matrix 0
// float temperature_offset = 1;
// float sensor_inaccuracy = 2.0;
// float lower_human_surface_temp = 32.5, higher_human_surface_temp = 37, lower_temp = 30;
// float deviation;
// uint8_t mask_sum;

// uint8_t count = 12;
// int sum = 0;

// float matrix_mean = 0, denoised_matrix_mean = 0, normalized_mean = 0, prevMean, t_prev, temp_adjust;

// void setup()
// {
//     Serial.begin(9600);
//     Serial.println(F("AMG88xx pixels"));

//     bool status;
//     dht.begin();
//     // default settings
//     status = amg.begin();
//     if (!status)
//     {
//         Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
//         // while (1);
//     }

//     Serial.println();

//     prevMean = 0;

//     delay(100);
// }

// void loop()
// {

//     float h = dht.readHumidity();
//     float t = dht.readTemperature();

//     amg.readPixels(pixels);
//     mask_sum = 0;
//     Serial.println(F("Raw Matrix"));
//     Serial.print("[");
//     for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
//     {
//         Serial.print(pixels[i - 1]);
//         // if higher than threshold and lower than adjusted human surface temperature
//         if (pixels[i - 1] >= (t - temperature_offset) && pixels[i - 1] <= (higher_human_surface_temp))
//         {
//             matrix_mask[i - 1] = 1;
//             mask_sum++;
//         }
//         else
//         {
//             // anything else whether higher or lower is zero
//             // this technocally makes sure that the measurement isn't affected by
//             // hotter objects like appliances, laptops etc
//             matrix_mask[i - 1] = 0;
//         }

//         Serial.print(", ");
//         if (i % 8 == 0)
//             Serial.println();
//     }
//     Serial.print("]");
//     Serial.println();
//     Serial.println();

//     matrix_mean = matrix_mean / 64;

//     Serial.println(F("Mask Matrix"));
//     for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
//     {
//         // pixels_minus_noise[i - 1] = pixels_minus_noise[i - 1] + (t-matrix_mean);
//         // Serial.print(pixels_minus_noise[i - 1]);
//         Serial.print(matrix_mask[i - 1]);
//         Serial.print(", ");

//         if (i % 8 == 0)
//             Serial.println();
//     }

//     Serial.print("]");
//     Serial.println();
//     Serial.println();
//     denoised_matrix_mean = denoised_matrix_mean / 64;
//     temp_adjust = t - matrix_mean;
//     normalized_mean = matrix_mean + temp_adjust;

//     Serial.println(F("Matrix sum: "));
//     Serial.print(mask_sum);
//     Serial.print(F("      "));
//     Serial.print((mask_sum * 100) / 64);
//     Serial.print(F(" %"));

//     Serial.println();

//     Serial.println(F("Matrix mean: "));
//     Serial.print(matrix_mean);
//     //
//     Serial.println();
//     Serial.println();
//     //
//     Serial.println(F("Adjusted temperature"));
//     Serial.print(t - sensor_inaccuracy);

//     Serial.println();
//     Serial.println();

//     Serial.println(F("Deviation: "));
//     Serial.print(matrix_mean - (t - sensor_inaccuracy));
//     Serial.print("   ");
//     Serial.print(((matrix_mean - (t - sensor_inaccuracy)) / (t - sensor_inaccuracy)) * 100);
//     Serial.print(" %");

//     Serial.println();
//     Serial.println();

//     // delay a second

//     Serial.print(F("Humidity: "));
//     Serial.print(h);
//     Serial.print(F("%  Temperature: "));
//     Serial.print(t);
//     Serial.print(F("°C "));

//     prevMean = denoised_matrix_mean;
//     t_prev = t;

//     Serial.println();
//     Serial.println();

//     sum += mask_sum;
//     count--;
//     Serial.println();
//     Serial.print("Count: ");
//     Serial.print(count);
//     Serial.print("    Sum Mask Mat: ");
//     Serial.print(sum);
//     Serial.println();

//     if (count == 0)
//     {
//         count = 12;
//         // float sum_mean = sum / (12 * 64);
//         Serial.println(F(" ------- Cumulated Matrix sum: ------- "));
//         Serial.print(sum);
//         Serial.print(F("      "));
//         Serial.print((sum * 100) / (64 * count));
//         Serial.print(F(" %"));

//         sum = 0;
//     }

//     Serial.println();
//     Serial.println();
//     delay(5000);
// }