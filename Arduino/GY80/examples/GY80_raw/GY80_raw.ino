// I2C device class (I2Cdev) demonstration Arduino sketch for GY80 and GY801 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// 6/7/2014 by Jon Gallant <jon@jongallant.com>

// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-10-07 - initial release
//     2016-06-07 - add GY80 components into one file.

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

// I2Cdev and ADXL345 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "ADXL345.h"
#include "L3G4200D.h"
#include "BMP085.h"
#include "HMC5883L.h"

// accel
ADXL345 accel;
int16_t ax, ay, az;

// gyro
L3G4200D gyro;
int16_t avx, avy, avz;

// baro
BMP085 baro;
float temperature;
float pressure;
float altitude;
int32_t lastMicros;

// magneto
HMC5883L mag;
int16_t mx, my, mz;

#define LED_PIN 0 // (Arduino is 13, Teensy is 6)
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accel.initialize();
    gyro.initialize();
    baro.initialize();
    mag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accel.testConnection() ? "ADXL345 connection successful" : "ADXL345 connection failed");
    Serial.println(gyro.testConnection() ? "L3G4200D connection successful" : "L3G4200D connection failed");
    Serial.println(baro.testConnection() ? "BMP085 connection successful" : "BMP085 connection failed");
    Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
    
    gyro.setFullScale(2000);
}

void loop() {
    
    accelerometer();
    gyroscope();
    barometer();
    magneto();
    
    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    
    // delay 100 msec to allow visually parsing blink and any serial output
    delay(100);
}

void accelerometer(){
    // read raw accel measurements from device
    accel.getAcceleration(&ax, &ay, &az);

    // display tab-separated accel x/y/z values
    Serial.print("accel:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.println(az);
}

void gyroscope(){
    gyro.getAngularVelocity(&avx, &avy, &avz);

    Serial.print("angular velocity:\t");
    Serial.print(avx); Serial.print("\t");
    Serial.print(avy); Serial.print("\t");
    Serial.println(avz);  
}

void barometer(){
    // request temperature
    baro.setControl(BMP085_MODE_TEMPERATURE);
    
    // wait appropriate time for conversion (4.5ms delay)
    lastMicros = micros();
    while (micros() - lastMicros < baro.getMeasureDelayMicroseconds());

    // read calibrated temperature value in degrees Celsius
    temperature = baro.getTemperatureC();

    // request pressure (3x oversampling mode, high detail, 23.5ms delay)
    baro.setControl(BMP085_MODE_PRESSURE_3);
    while (micros() - lastMicros < baro.getMeasureDelayMicroseconds());

    // read calibrated pressure value in Pascals (Pa)
    pressure = baro.getPressure();

    // calculate absolute altitude in meters based on known pressure
    // (may pass a second "sea level pressure" parameter here,
    // otherwise uses the standard value of 101325 Pa)
    altitude = baro.getAltitude(pressure);

    // display measured values if appropriate
    Serial.print("T/P/A\t");
    Serial.print(temperature); Serial.print("\t");
    Serial.print(pressure); Serial.print("\t");
    Serial.print(altitude);
    Serial.println("");
}

void magneto(){
   // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);

    // display tab-separated gyro x/y/z values
    Serial.print("mag:\t");
    Serial.print(mx); Serial.print("\t");
    Serial.print(my); Serial.print("\t");
    Serial.print(mz); Serial.print("\t");
    
    // To calculate heading in degrees. 0 degree indicates North
    float heading = atan2(my, mx);
    if(heading < 0)
      heading += 2 * M_PI;
    Serial.print("heading:\t");
    Serial.println(heading * 180/M_PI);
}
