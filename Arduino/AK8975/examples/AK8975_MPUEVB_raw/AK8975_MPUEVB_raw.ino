// I2C device class (I2Cdev) demonstration Arduino sketch for AK8975 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// This example uses the AK8975 as mounted on the InvenSense MPU-6050 Evaluation
// Board, and as such also depends (minimally) on the MPU6050 library from the
// I2Cdevlib collection. It initializes the MPU6050 and immediately enables its
// "I2C Bypass" mode, which allows the sketch to communicate with the AK8975
// that is attached to the MPU's AUX SDA/SCL lines. The AK8975 is configured on
// this board to use the 0x0E address.
//
// Note that this small demo does not make use of any of the MPU's amazing
// motion processing capabilities (the DMP); it only provides raw sensor access
// to the compass as mounted on that particular evaluation board.
//
// For more info on the MPU-6050 and some more impressive demos, check out the
// device page on the I2Cdevlib website:
//     http://www.i2cdevlib.com/devices/mpu6050
//
// Changelog:
//     2011-10-07 - initial release

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

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev, AK8975, and MPU6050 must be installed as libraries, or else the
// .cpp/.h files for all classes must be in the include path of your project
#include "I2Cdev.h"
#include "AK8975.h"
#include "MPU6050.h"

// class default I2C address is 0x0C
// specific I2C addresses may be passed as a parameter here
// Addr pins low/low = 0x0C
// Addr pins low/high = 0x0D
// Addr pins high/low = 0x0E (default for InvenSense MPU6050 evaluation board)
// Addr pins high/high = 0x0F
AK8975 mag(0x0E);
MPU6050 accelgyro; // address = 0x68, the default, on MPU6050 EVB

int16_t mx, my, mz;

#define LED_PIN 13
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize devices
    Serial.println("Initializing I2C devices...");
    
    // initialize MPU first so we can switch to the AUX lines
    accelgyro.initialize();
    accelgyro.setI2CBypassEnabled(true);
    mag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "AK8975 connection successful" : "AK8975 connection failed");

    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);

    // display tab-separated gyro x/y/z values
    Serial.print("mag:\t");
    Serial.print(mx); Serial.print("\t");
    Serial.print(my); Serial.print("\t");
    Serial.println(mz);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}
