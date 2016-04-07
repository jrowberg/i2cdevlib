// I2Cdev library collection - BMP085 basic Arduino example sketch
// Based on register information stored in the I2Cdevlib internal database
// 2012-06-28 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-06-28 - initial release, dynamically built

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

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

// I2Cdev and BMP085 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "BMP085.h"

// class default I2C address is 0x77
// specific I2C addresses may be passed as a parameter here
// (though the BMP085 supports only one address)
BMP085 barometer;

float temperature;
float pressure;
float altitude;
int32_t lastMicros;

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
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
    barometer.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(barometer.testConnection() ? "BMP085 connection successful" : "BMP085 connection failed");

    // configure LED pin for activity indication
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // request temperature
    barometer.setControl(BMP085_MODE_TEMPERATURE);
    
    // wait appropriate time for conversion (4.5ms delay)
    lastMicros = micros();
    while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());

    // read calibrated temperature value in degrees Celsius
    temperature = barometer.getTemperatureC();

    // request pressure (3x oversampling mode, high detail, 23.5ms delay)
    barometer.setControl(BMP085_MODE_PRESSURE_3);
    while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());

    // read calibrated pressure value in Pascals (Pa)
    pressure = barometer.getPressure();

    // calculate absolute altitude in meters based on known pressure
    // (may pass a second "sea level pressure" parameter here,
    // otherwise uses the standard value of 101325 Pa)
    altitude = barometer.getAltitude(pressure);

    // display measured values if appropriate
    Serial.print("T/P/A\t");
    Serial.print(temperature); Serial.print("\t");
    Serial.print(pressure); Serial.print("\t");
    Serial.print(altitude);
    Serial.println("");

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    
    // delay 100 msec to allow visually parsing blink and any serial output
    delay(100);
}