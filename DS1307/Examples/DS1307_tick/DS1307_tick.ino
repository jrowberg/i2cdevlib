// I2C device class (I2Cdev) demonstration Arduino sketch for DS1307 class
// 11/12/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2011-11-12 - initial release

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

// I2Cdev and DS1307 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "DS1307.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// but this device only supports one I2C address (0x68)
DS1307 rtc;

uint16_t year;
uint8_t month, day, dow, hours, minutes, seconds;

#define LED_PIN 13
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    rtc.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(rtc.testConnection() ? "DS1307 connection successful" : "DS1307 connection failed");

    // configure LED pin for output
    pinMode(LED_PIN, OUTPUT);
    
    // set sample time
    rtc.setDateTime24(2011, 11, 12, 13, 45, 0);
}

void loop() {
    // read all clock info from device
    rtc.getDateTime24(&year, &month, &day, &hours, &minutes, &seconds);

    // display YYYY-MM-DD hh:mm:ss time
    Serial.print("rtc:\t");
    Serial.print(year); Serial.print("-");
    if (month < 10) Serial.print("0");
    Serial.print(month); Serial.print("-");
    if (day < 10) Serial.print("0");
    Serial.print(day); Serial.print(" ");
    if (hours < 10) Serial.print("0");
    Serial.print(hours); Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.print(minutes); Serial.print(":");
    if (seconds < 10) Serial.print("0");
    Serial.println(seconds);

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);

    // wait one second so the next reading will be different
    delay(1000);
}
