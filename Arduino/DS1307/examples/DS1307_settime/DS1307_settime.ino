// I2C device class (I2Cdev) demonstration Arduino sketch for DS1307 class
// 
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2016-04-15 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2016 Jeff Rowberg

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

// I2Cdev and DS1307 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "DS1307.h"


DS1307 rtc;

void setup()  {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    // Note that the DS1307 doesn't support 400Khz i2c 
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


    // Use "the compiler's time" to set time. Make sure the sketch is recompiled before uploading.
    DateTime compilerNow (__DATE__, __TIME__);
    rtc.setDateTime(compilerNow);   
    rtc.setClockRunning(true);
    Serial.print(" Clock is set ");
    Serial.println(rtc.getClockRunning()?"and running":"but it is NOT running");
}

void printDigits(int digits, const char* prefix="") {
  // utility function: prints prefix and leading 0 before the number
  Serial.print(prefix);
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void loop() {
  // ISO 8601 FTW!
  
  uint16_t year = 0;
  uint8_t month, day, dow, hours, minutes, seconds = 0;
  static uint32_t tick = 0;

  rtc.getDateTime24(&year, &month, &day, &hours, &minutes, &seconds);
  Serial.print(tick++);
  printDigits(year, ": ");
  printDigits(month,"-");
  printDigits(day,"-");
  printDigits(hours," ");
  printDigits(minutes,":");
  printDigits(seconds, ":");
 
  Serial.println();
  delay(1000);
}



