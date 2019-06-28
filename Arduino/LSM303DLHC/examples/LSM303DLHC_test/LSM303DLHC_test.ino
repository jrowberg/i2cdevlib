// I2C device class (I2Cdev) demonstration Arduino sketch for LSM303DLHC class
// 3/10/2015 by Nate Costello <natecostello at gmail dot com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2015-03-10 - initial release

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

// I2Cdev and LSM303DLHC must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <I2Cdev.h>
#include <LSM303DLHC.h>

// default address is 105
// specific I2C address may be passed here
LSM303DLHC accelMag;

int16_t ax, ay, az;
int16_t mx, my, mz; 

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
    accelMag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelMag.testConnection() ? "LSM303DLHC connection successful" : "LSM303DLHC connection failed");

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);

    // set scale to 2Gs
    accelMag.setAccelFullScale(2);

    // set accel data rate to 200Hz
    accelMag.setAccelOutputDataRate(200);

    // test scale
    Serial.print("Accel Scale: ");
    Serial.println(accelMag.getAccelFullScale());

    // test data rate
    Serial.print("Accel Output Data Rate: ");
    Serial.println(accelMag.getAccelOutputDataRate());

    // set mag data rate to 220Hz
    accelMag.setMagOutputDataRate(220);

    // test mag data rate
    Serial.print("Mag Output Data Rate: ");
    Serial.println(accelMag.getMagOutputDataRate());

    //set mag gain
    accelMag.setMagGain(1100);

    // test mag gain
    Serial.print("Mag Gain: ");
    Serial.println(accelMag.getMagGain());

    //enable mag
    accelMag.setMagMode(LSM303DLHC_MD_CONTINUOUS);
    Serial.println(accelMag.getMagMode());


}

void loop() {
    // read raw angular velocity measurements from device
    accelMag.getAcceleration(&ax, &ay, &az);
    accelMag.getMag(&mx, &my, &mz);

    // Serial.print("Acceleration:\t");
    // Serial.print(ax,HEX); Serial.print("\t");
    // Serial.print(ay,HEX); Serial.print("\t");
    // Serial.print(az,HEX);

    // Serial.print("  Magnetic Field:\t");
    // Serial.print(mx,HEX); Serial.print("\t");
    // Serial.print(my,HEX); Serial.print("\t");
    // Serial.println(mz,HEX);

    // Serial.print("Acceleration:\t");
    // Serial.print(ax); Serial.print("\t");
    // Serial.print(ay); Serial.print("\t");
    // Serial.print(az);

    // Serial.print("  Magnetic Field:\t");
    // Serial.print(mx); Serial.print("\t");
    // Serial.print(my); Serial.print("\t");
    // Serial.println(mz);

    Serial.print("Acceleration:\t");
    Serial.print(ax*0.0000625F,4); Serial.print("\t");
    Serial.print(ay*0.0000625F,4); Serial.print("\t");
    Serial.print(az*0.0000625F,4);

    Serial.print("  Magnetic Field:\t");
    Serial.print(mx); Serial.print("\t");
    Serial.print(my); Serial.print("\t");
    Serial.println(mz);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);

    delay(1000);
}


