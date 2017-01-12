/*
I2Cdev library collection - ADXL345 RPi example
Based on the example in Arduino/ADXL345/

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
  1. install the bcm2835 library, see
http://www.airspayce.com/mikem/bcm2835/index.html
  2. enable i2c on your RPi , see
https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c
  3. connect your i2c devices
  4. then from bash
      $ PATH_I2CDEVLIB=~/i2cdevlib/
      $ gcc -o ADXL345_example_1
${PATH_I2CDEVLIB}RaspberryPi_bcm2835/ADXL345/examples/ADXL345_example_1.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev
${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
         -I ${PATH_I2CDEVLIB}/Arduino/ADXL345/
${PATH_I2CDEVLIB}/Arduino/ADXL345/ADXL345.cpp -l bcm2835 -l m
      $ sudo ./ADXL345_example_1

*/

#include "ADXL345.h"
#include "Communicator.h"
#include "FileUtil.h"
#include "I2Cdev.h"
#include <bcm2835.h>
#include <iostream>
#include <json/json.h>
#include <stdio.h>
#include <sys/time.h>

struct timeval start_t, end_t;
int diff;
int msg_index = 1;
int port;
class Communicator *comm = NULL;
FileUtil fileUtil;
Json::FastWriter fw;
Json::Value root;

int main(int argc, char **argv) {
  printf("ADXL345 3-axis acceleromter example program\n");
  I2Cdev::initialize();
  ADXL345 accel;
  if (accel.testConnection())
    printf("ADXL345 connection test successful\n");
  else {
    fprintf(stderr, "ADXL345 connection test failed! exiting ...\n");
    return 1;
  }
  accel.initialize();
  int16_t ax, ay, az;
  const char *rpi_id = fileUtil.getRpiID().c_str();
  const char *host = fileUtil.getHost().c_str();
  comm = new Communicator(rpi_id, host, port);
  while (true) {
    gettimeofday(&start_t, NULL);
    accel.getAcceleration(&ax, &ay, &az);
    printf("  x_raw:  %d       y_raw:  %d      z_raw:  %d\n", ax, ay, az);
    fflush(stdout);
    gettimeofday(&end_t, NULL);
    diff = (end_t.tv_sec - start_t.tv_sec) * 1000000 +
           (end_t.tv_usec - start_t.tv_usec);
    printf("The time difference is %d ns\n", diff);
    root["rPi_id"] = rpi_id;
    root["x_axis"] = ax;
    root["y_axis"] = ay;
    root["z_aixs"] = az;
    root["elapse_time"] = diff;
    root["msg_index"] = msg_index;
    cout << fw.write(root);
    string json = fw.write(root);
    const char *j = json.c_str();
    //	publish to broker
    comm->send_message(j);
    // pthread_mutex_lock(&qlock);
    msg_index++;
  }
  return 1;
}
