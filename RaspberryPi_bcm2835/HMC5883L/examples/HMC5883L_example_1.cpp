/*
I2Cdev library collection - HMC5883L RPi example
Based on the example in Arduino/HMC5883L/

==============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================

To compile on a Raspberry Pi (1 or 2)
  1. install the bcm2835 library, see http://www.airspayce.com/mikem/bcm2835/index.html
  2. enable i2c on your RPi , see https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c
  3. connect your i2c devices
  4. then from bash
      $ PATH_I2CDEVLIB=~/i2cdevlib/
      $ gcc -o HMC5883L_example_1 ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/HMC5883L/examples/HMC5883L_example_1.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
         -I ${PATH_I2CDEVLIB}/Arduino/HMC5883L/ ${PATH_I2CDEVLIB}/Arduino/HMC5883L/HMC5883L.cpp -l bcm2835 -l m
      $ sudo ./HMC5883L_example_1

*/

#include <stdio.h>
#include <bcm2835.h>
#include "I2Cdev.h"
#include "HMC5883L.h"
#include <math.h>

#define PI 3.14159265

int main(int argc, char **argv) {
  printf("HMC5883L 3-axis acceleromter example program\n");
  I2Cdev::initialize();
  HMC5883L mag ;
  if ( mag.testConnection() ) 
    printf("HMC5883L connection test successful\n") ;
  else {
    fprintf( stderr, "HMC5883L connection test failed! something maybe wrong, continueing anyway though ...\n");
    //return 1;
  }
  mag.initialize();
  mag.setSampleAveraging(HMC5883L_AVERAGING_8);
  mag.setGain(HMC5883L_GAIN_1090);
  int16_t mx, my, mz;
  float heading ;
  while (true) {
    mag.getHeading(&mx, &my, &mz);
    heading = atan2(my, mx)  * 180 / PI;
    printf("  mx:  %d       my:  %d      mz:  %d     heading:  %3.1f deg\r", mx, my, mz, heading);
    fflush(stdout);
    bcm2835_delay(200);
  }
  return 1; 
}
