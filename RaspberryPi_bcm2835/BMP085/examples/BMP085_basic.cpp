/*
I2Cdev library collection - BMP085 RPi example
Based on the example in Arduino/BMP085/

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
      $ gcc -o BMP085_basic_example ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/BMP085/examples/BMP085_basic.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
         -I ${PATH_I2CDEVLIB}/Arduino/BMP085/ ${PATH_I2CDEVLIB}/Arduino/BMP085/BMP085.cpp -l bcm2835 -l m
      $ sudo ./BMP085_basic_example

*/

#include <stdio.h>
#include <bcm2835.h>
#include "I2Cdev.h"
#include "BMP085.h"

int main(int argc, char **argv) {
  printf("BMPO85 examples program\n");
  I2Cdev::initialize();
  BMP085 barometer ; // BMP085 class default I2C address is 0x77, specific I2C addresses may be passed as a parameter here
  if ( barometer.testConnection() ) 
    printf("BMP085 connection test successful\n") ;
  else {
    fprintf( stderr, "BMP085 connection test failed! exiting ...\n");
    return 1;
  }
  barometer.loadCalibration();
  float temperature;
  float pressure;
  float altitude;
  while (true) {
    barometer.setControl(BMP085_MODE_TEMPERATURE);
    bcm2835_delay(5); // wait 5 ms for conversion 
    temperature = barometer.getTemperatureC();
    barometer.setControl(BMP085_MODE_PRESSURE_3) ; //taking reading in highest accuracy measurement mode
    bcm2835_delay( barometer.getMeasureDelayMicroseconds() / 1000 );
    pressure = barometer.getPressure();
    altitude = barometer.getAltitude(pressure);
    printf("  Temperature: %3.1f deg C      Pressure %3.3f kPa      altitude %3.1f m\r", temperature, pressure/1000.0, altitude);
    fflush(stdout);
    bcm2835_delay(200);
  }
  return 1; 
}
