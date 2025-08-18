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
#include <LIS3MDL.h>

// default address is 105
// specific I2C address may be passed here
LIS3MDL mag;

int16_t magx, magy, magz;

bool blinkState = false;

void setup() {
    Wire.begin(SDA, SCL);

    Serial.begin(9600);

    // initialize device
    Serial.print("Initializing I2C devices...");
    if (!mag.initialize()) {
        Serial.println("Failed to find LIS3MDL!");
    }
    Serial.println("done!");

    // verify connection
    Serial.print("Testing device connections...");
    Serial.println(mag.testConnection() ? "L3G4200D connection successful" : "L3G4200D connection failed");

    // configure LED for output
    pinMode(LED_BUILTIN, OUTPUT);

    // Configure device
    mag.setXYOpMode(LIS3MDL_OM_HP_MODE); // Set XY-axes to High Performance mode
    mag.setZOpMode(LIS3MDL_OMZ_HP_MODE); // Set Z-axis to High Performance mode
    mag.setDataRate(LIS3MDL_DATA_RATE_80_HZ); // Set sample rate to 80 Hz
    mag.setFullScale(LIS3MDL_FULL_SCALE_4_G); // Set scale to ±4 Gauss
}

void loop() {
    // read raw angular velocity measurements from device
    mag.getMagneticField(&magx, &magy, &magz);

    Serial.print("Magnetic field \t");
    Serial.print("X: "); Serial.print(magx); Serial.print("\t");
    Serial.print("Y: "); Serial.print(magy); Serial.print("\t");
    Serial.print("Z: "); Serial.println(magz);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);

    delay(1000/80); // Try to run every 1/80 of a second
}


