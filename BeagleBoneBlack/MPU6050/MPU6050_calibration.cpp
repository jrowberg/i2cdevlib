
// Arduino sketch that returns calibration offsets for MPU6050 //   Version 1.1  (31th January 2014)
// Done by Luis Ródenas <luisrodenaslorda@gmail.com>
// Based on the I2Cdev library and previous work by Jeff Rowberg <jeff@rowberg.net>
// Updates (of the library) should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//TODO: include author info

// These offsets were meant to calibrate MPU6050's internal DMP, but can be also useful for reading sensors.
// The effect of temperature has not been taken into account so I can't promise that it will work if you
// calibrate indoors and then use it outdoors. Best is to calibrate and use at the same room temperature.

/* ==========  LICENSE  ==================================
 I2Cdev device library code is placed under the MIT license
 Copyright (c) 2011 Jeff Rowberg
 
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
 =========================================================
 */

// I2Cdev and MPU6050 must be installed as libraries
#include "../I2Cdev/I2Cdev.h" //FIXME
#include "MPU6050.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_BUFFER_SIZE 1000 //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
#define DEFAULT_ACCEL_DEADZONE 8 //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
#define DEFAULT_GYRO_DEADZONE 1  //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

struct ImuValues
{
  int16_t ax = 0, ay = 0, az = 0;
  int16_t gx = 0, gy = 0, gz = 0;
};

void resetOffsets(MPU6050 *mpu);

//return mean values
ImuValues meansensors(MPU6050 *mpu, int buffersize);

//return offsets
ImuValues calibration(MPU6050 *mpu, int buffersize, int16_t accel_deadzone, int16_t gyro_deadzone);

int main()
{

  int buffersize = DEFAULT_BUFFER_SIZE;
  int16_t accel_deadzone = DEFAULT_ACCEL_DEADZONE;
  int16_t gyro_deadzone = DEFAULT_GYRO_DEADZONE;

  MPU6050 mpu;

  mpu.initialize();

  printf("Send any character to start sketch.\n\n");
  getchar();
  usleep(1500*1000);

  // start message
  printf("\nMPU6050 Calibration Sketch\n");
  sleep(2);
  printf("\nYour MPU6050 should be placed in horizontal position, with package letters facing up. \nDon't touch it until you see a finish message.\n\n");
  sleep(3);

  // verify connection
  if (mpu.testConnection())
  {
    printf("MPU6050 connection successful\n");
  }
  else
  {
    printf("MPU6050 connection failed\n");
    return -1;
  }

  sleep(1);

  resetOffsets(&mpu);

  printf("\nReading sensors for first time...\n");

  ImuValues mean = meansensors(&mpu, buffersize);
  sleep(1);

  printf("\nCalculating offsets...\n");
  ImuValues offsets = calibration(&mpu, buffersize, accel_deadzone, gyro_deadzone);
  sleep(1);

  mean = meansensors(&mpu, buffersize);
  printf("\nFINISHED\n");
  printf("Sensor readings with offsets:\t%d\t%d\t%d\t%d\n", mean.ax, mean.ay, mean.az, mean.gx, mean.gy, mean.gz);
  printf("Your offsets:\t%d\t%d\t%d\t%d\n", offsets.ax, offsets.ay, offsets.az, offsets.gx, offsets.gy, offsets.gz);
  printf("\nData is printed as: acelX acelY acelZ giroX giroY giroZ\n");
  printf("Check that your sensor readings are close to 0 0 16384 0 0 0\n");
  printf("If calibration was succesful write down your offsets so you can set them in your  projects using something similar to mpu.setXAccelOffset(youroffset)\n");

  return 0;
}

void resetOffsets(MPU6050 *imu)
{
  imu->setXAccelOffset(0);
  imu->setYAccelOffset(0);
  imu->setZAccelOffset(0);
  imu->setXGyroOffset(0);
  imu->setYGyroOffset(0);
  imu->setZGyroOffset(0);
}

//return mean values
ImuValues meansensors(MPU6050 *mpu, int buffersize)
{
  long i = 0, buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;
  ImuValues mean;

  while (i < (buffersize + 101))
  {
    // read raw accel/gyro measurements from device
    mpu->getMotion6(&mean.ax, &mean.ay, &mean.az, &mean.gx, &mean.gy, &mean.gz);

    if (i > 100 && i <= (buffersize + 100))
    { //First 100 measures are discarded
      buff_ax = buff_ax + mean.ax;
      buff_ay = buff_ay + mean.ay;
      buff_az = buff_az + mean.az;
      buff_gx = buff_gx + mean.gx;
      buff_gy = buff_gy + mean.gy;
      buff_gz = buff_gz + mean.gz;
    }
    if (i == (buffersize + 100))
    {
      mean.ax = buff_ax / buffersize;
      mean.ay = buff_ay / buffersize;
      mean.az = buff_az / buffersize;
      mean.gx = buff_gx / buffersize;
      mean.gy = buff_gy / buffersize;
      mean.gz = buff_gz / buffersize;
    }
    i++;
    usleep(2000); //Needed so we don't get repeated measures
  }

  return mean;
}

ImuValues calibration(MPU6050 *mpu, ImuValues mean, int buffersize, int16_t accel_deadzone, int16_t gyro_deadzone)
{
  ImuValues offset;

  offset.ax = -mean.ax / 8;
  offset.ay = -mean.ay / 8;
  offset.az = (16384 - mean.az) / 8;

  offset.gx = -mean.gx / 4;
  offset.gy = -mean.gy / 4;
  offset.gz = -mean.gz / 4;

  while (1)
  {
    int ready = 0;
    mpu->setXAccelOffset(offset.ax);
    mpu->setYAccelOffset(offset.ay);
    mpu->setZAccelOffset(offset.az);

    mpu->setXGyroOffset(offset.gx);
    mpu->setYGyroOffset(offset.gy);
    mpu->setZGyroOffset(offset.gz);

    mean = meansensors(mpu, buffersize);
    printf("...\n");

    if (abs(mean.ax) <= accel_deadzone)
      ready++;
    else
      offset.ax = offset.ax - mean.ax / accel_deadzone;

    if (abs(mean.ay) <= accel_deadzone)
      ready++;
    else
      offset.ay = offset.ay - mean.ay / accel_deadzone;

    if (abs(16384 - mean.az) <= accel_deadzone)
      ready++;
    else
      offset.az = offset.az + (16384 - mean.az) / accel_deadzone;

    if (abs(mean.gx) <= gyro_deadzone)
      ready++;
    else
      offset.gx = offset.gx - mean.gx / (gyro_deadzone + 1);

    if (abs(mean.gy) <= gyro_deadzone)
      ready++;
    else
      offset.gy = offset.gy - mean.gy / (gyro_deadzone + 1);

    if (abs(mean.gz) <= gyro_deadzone)
      ready++;
    else
      offset.gz = offset.gz - mean.gz / (gyro_deadzone + 1);

    if (ready == 6)
      break;
  }
}
