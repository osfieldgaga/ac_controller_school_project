# Smart AC Controller
A Smart AC Controller for my final year project.

The project aims to reduce the energy consumption of Air Conditioners. 

The device comprises a microcontroller, a temperature sensor, an infrared thermal sensor and an infrared transmitter. 
The behaviour of the device is based on the fact that increasing temperature reduces the energy consumption of the AC unit (21°C with an optimum setpoint of 25°C, with 25°C providing the best energy savings for a 1-star AC unit). 

The device increases the temperature in the room by 1 degree at regular intervals and assesses the thermal comfort when the temperature settles using the Machine Learning model trained with the data from ASHRAE. 

If the thermal comfort is still within the desired limits (PMV should be greater than -0.5 and less than 0.5), the device attempts to increase the temperature again. 
When the PMV goes out of the desired range 0f -0.5 to 0.5, the temperature is adjusted (increased or decreased) to reach thermal comfort again. 
The device also checks for room occupancy and turns off the AC when the room is period for a certain period of time.
