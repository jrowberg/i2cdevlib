
#include "Sensor.h"
//The constructor initialize sensor_id, temperature, clock and date



Sensor::Sensor(int i):index(i)
{
//     if(index==0)
//     {ADXL345 sensor;}
//     else
//     {ADXL345 sensor(ADXL345_ADDRESS_ALT_HIGH);}
    ADXL345 sensor;
    sensor.getAcceleration(&x, &y, &z);

}





int Sensor::get_x()
{
    return x;
}

int Sensor::get_y()
{
    return y;
}

int Sensor::get_z()
{
    return z;
}
