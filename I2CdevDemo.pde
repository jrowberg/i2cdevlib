// I2C device class (I2Cdev) demonstration Arduino sketch
// 7/25/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib

/* ============================================
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
===============================================
*/

#include <Wire.h>

//#define I2CDEV_SERIAL_DEBUG

#include "ADXL345.h"
#include "ITG3200.h"
#include "HMC5883L.h"

ADXL345 accelerometer;
ITG3200 gyroscope;
HMC5883L magnetometer;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

#define LED_PIN 13
bool blinkState = false;

void setup() {
    // join I2C bus
    Wire.begin();
    
    // initialize serial communication
    Serial.begin(38400);

    // initialize all devices
    accelerometer.initialize();
    gyroscope.initialize();
    magnetometer.initialize();

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // read measurements from accel, gryo, and magnetometer
    accelerometer.getAcceleration(&ax, &ay, &az);
    gyroscope.getRotation(&gx, &gy, &gz);
    magnetometer.getHeading(&mx, &my, &mz);
  
    Serial.print("a/g/m:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.print(gz); Serial.print("\t");
    Serial.print(mx); Serial.print("\t");
    Serial.print(my); Serial.print("\t");
    Serial.println(mz);

    delay(100);
    
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}
