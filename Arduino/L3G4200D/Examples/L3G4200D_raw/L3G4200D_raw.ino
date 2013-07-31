// I2C device class (I2Cdev) demonstration Arduino sketch for L3G4200D class
// 7/31/2013 by Jonathan Arnett <j3rn@j3rn.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-07-31 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jonathan Arnett, Jeff Rowberg

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
#include <Wire.h>

// I2Cdev and L3G4200D must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <I2Cdev.h>
#include <L3G4200D.h>

// default address is 105
// specific I2C address may be passed here
L3G4200D gyro;

int16_t avx, avy, avz;

#define LED_PIN 13 // (Arduino is 13, Teensy is 6)
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(9600);

    // initialize device
    Serial.println("Initializing I2C devices...");
    gyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(gyro.testConnection() ? "L3G4200D connection successful" : "L3G4200D connection failed");

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);

    // data seems to be best when full scale is 2000
    gyro.setFullScale(2000);
}

void loop() {
    // read raw angular velocity measurements from device
    gyro.getAngularVelocity(&avx, &avy, &avz);

    Serial.print("angular velocity:\t");
    Serial.print(avx); Serial.print("\t");
    Serial.print(avy); Serial.print("\t");
    Serial.println(avz);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}


