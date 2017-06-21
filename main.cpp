/*
I2Cdev library collection - ADXL345 RPi example
Based on the example in Arduino/ADXL345/

==============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software isa
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
"ADXL345_example_1.cpp" 163L, 5859C 2,1           Top
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
#include "I2Cdev.h"
#include <bcm2835.h>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;


struct timeval start_t, end_t;

int msg_index = 1;
class Communicator *comm = NULL;
Json::FastWriter fw;
Json::Value root;
int16_t x, y, z;
long long diff;
int data_rate = 15;
int data_range = 0;
ADXL345 a;
ADXL345 b(ADXL345_ADDRESS_ALT_HIGH);


int main(int argc, char **argv) {

  if (a.testConnection()&& b.testConnection())
    printf("Both sensors' connection test successful\n");
  else {
    fprintf(stderr, "ADXL345 connection test failed! exiting ...\n");
    return 1;
  }

   cout << "current data rate of sensor_1 is " << int(a.getRate())<< endl;
   cout << "current data rate of sensor_2 is " << int(b.getRate()) << endl;
   a.setRate(data_rate);
   b.setRate(data_rate);
   cout << "data rate of sensor_1 after change " <<int(a.getRate()) << endl;
   cout << "data rate of sensor_2 after change " << int(b.getRate()) << endl;

   cout << "current data range of sensor_1 is " << int(a.getRange()) << endl;
   cout << "current data range of sensor_2 is " << int(b.getRange()) << endl;
   a.setRange(data_range);
   b.setRange(data_range);
   cout << "data range of sensor_1 after change" << int(a.getRange()) << endl;
   cout << "data range of sensor_2 after change" << int(b.getRange()) << endl;

  I2Cdev::initialize();
  a.initialize();
  b.initialize();
  comm = new Communicator("1", "192.168.1.115", 1883);
  gettimeofday(&start_t, NULL);
  printf("start time : %lld\n", start_t.tv_sec * (uint64_t)1000000+ start_t.tv_usec);
  
  while(1) {
	  for(int i=1; i<3; i++){
		  if(i==1){
	a.getAcceleration(&x, &y, &z);
		  }else{
	b.getAcceleration(&x, &y, &z);}
  gettimeofday(&end_t, NULL);
  diff = (end_t.tv_sec - start_t.tv_sec) * (uint64_t)1000000 +
           (end_t.tv_usec - start_t.tv_usec);
  root["rpi_id"] = 1;
  root["sensor_id"] = i;
  root["x"] = x;
  root["y"] = y;
  root["z"] = z;
  root["elapsed_time"] = diff;
  root["msg_index"] = msg_index;
// 	cout << fw.write(root);
	string json = fw.write(root);
	const char *j = json.c_str();
//	publish to broker
	comm->send_message(j);}

	msg_index++;

		
	}

}

