#include <iostream>
#include <string>
#include <fstream>
#include <ios>
#include <istream>
#include <json/json.h>
#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>
#include <mutex>
#include "FileUtil.h"
#include <sys/types.h>
#include <cstring>
#include <sys/time.h>
#include "ADXL345.h"
#include "I2Cdev.h"
#include <bcm2835.h>

using namespace std;

#ifndef SENSOR_H
#define SENSOR_H



class Sensor
{
public:
    Sensor(int i);   //initialize the sensor with a index

    int get_x();
    int get_y();
    int get_z();
        
  
private:
   int x;
   int y;
   int z;
   

};

#endif
