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
#include <fstream>
#include <json/json.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>


using namespace std;
char buffer[50000000];
char *bufferp = buffer;
struct timeval start_t, end_t, stop_t, copy_t;
long long diff, diff2, diff3;
int msg_index = 1;
int16_t ax, ay, az;
//int port;
//class Communicator *comm = NULL;
//FileUtil fileUtil;
//Json::FastWriter fw;
//Json::Value root;

void timmer_handler();

void timer_handler(){
gettimeofday(&end_t, NULL);	
ADXL345 a;
  a.getAcceleration(&ax, &ay, &az);
//   fflush(stdout);
//  printf("readings: %d, %d, %d\n", ax, ay, az);

  diff = (end_t.tv_sec - start_t.tv_sec) * (uint64_t)1000000 +
           (end_t.tv_usec - start_t.tv_usec);
   // printf("The time difference is %d us\n", diff);
    root["rPi_id"] = rpi_id;
    root["x_axis"] = ax;
    root["y_axis"] = ay;
    root["z_aixs"] = az;
    root["elapsed_time"] = diff;
    root["msg_index"] = msg_index;
//    cout << fw.write(root);
//     printf("%d,%lld,%d,%d,%d\n", msg_index, diff, ax, ay, az);
//      bufferp += sprintf (bufferp, "%d,%lld,%d,%d,%d\n",msg_index, diff, ax, ay, az);
//     outputFile << msg_index << "," << diff << "," << ax << "," << ay << "," << az
//              << endl;
//    string json = fw.write(root);
   const char *j = json.c_str();
    //	publish to broker
   comm->send_message(j);
//     pthread_mutex_lock(&qlock);
    msg_index++;
 // printf("diff: %lld\n", diff);
}


int main(int argc, char **argv) {
  I2Cdev::initialize();
  ADXL345 accel;
  if (accel.testConnection())
    printf("ADXL345 connection test successful\n");
  else {
    fprintf(stderr, "ADXL345 connection test failed! exiting ...\n");
    return 1;
  }
  accel.initialize();
  cout << "current data rate is " << int(accel.getRate())<< endl;
  accel.setRate(15); 
  cout << "data rate after change " <<int(accel.getRate()) << endl;
  cout << "current data range" << int(accel.getRange()) << endl;
  accel.setRange(0);
  cout << "data range after change" << int(accel.getRate()) << endl;

  const char *rpi_id = fileUtil.getRpiID().c_str();
  const char *host = fileUtil.getHost().c_str();
  comm = new Communicator(rpi_id, host, port);

//   ofstream outputFile;
//   outputFile.open("result.txt");
//   outputFile << "msg_index"
//              << ","
//              << "elapsed_time"
//              << ","
//              << "x"
//              << ","
//              << "y"
//              << ","
//              << "z" << endl;
//   printf("start time : %lld\n", start_t.tv_sec * (uint64_t)1000000+ start_t.tv_usec);
    gettimeofday(&start_t, NULL);
    struct itimerval value;
    value.it_value.tv_sec=0;                
    value.it_value.tv_usec=300;
    value.it_interval= value.it_value;
    signal(SIGALRM, (void (*)(int))timer_handler);
    setitimer(ITIMER_REAL, &value, NULL);  
//     while (1);
  while (msg_index < 1000000);
  gettimeofday(&stop_t, NULL);
  printf( "total running time(seconds): %d\n" ,(stop_t.tv_sec- start_t.tv_sec) );
  diff2 = (stop_t.tv_sec - start_t.tv_sec) * (uint64_t)1000000 +
           (stop_t.tv_usec - start_t.tv_usec);
  printf( "average time elapse: %d\n", diff2/100000 );
  ofstream outputFile;
  outputFile.open("result.txt");
  outputFile << buffer << '\n';
  outputFile.close();
  gettimeofday(&copy_t, NULL);
  diff3 = (copy_t.tv_sec - stop_t.tv_sec) * (uint64_t)1000000 +
           (copy_t.tv_usec - stop_t.tv_usec);
  printf("output time: %lld\n", diff3);
//   cou"length is " << n << endl;

  return 1;
}


