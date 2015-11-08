// I2C device class (I2Cdev) demonstration Arduino sketch for IAQ2000 class
// 2012-04-01 by Peteris Skorovs <pskorovs@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2012-04-01 - initial release
//	   2015-11-08 - added TVoc and Status

/* ============================================
  IAQ2000 device library code is placed under the MIT license
  Copyright (c) 2012 Peteris Skorovs, Jeff Rowberg

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

// I2Cdev and IAQ2000 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "IAQ2000.h"


// class default I2C address is 0x5A
// specific I2C addresses may be passed as a parameter here
// but this device only supports one I2C address (0x5A)
IAQ2000 iaq;

uint16_t airQuality;
uint8_t iAQstatus;
uint16_t airTvoc;

unsigned long startTime;
uint16_t oldairQuality = 0;
uint8_t oldiAQstatus = 0;
uint16_t oldairTvoc = 0;
uint8_t result = 0;


#define LED_PIN 13
bool blinkState = false;


void setup() {
  // configure LED pin for output
  pinMode(LED_PIN, OUTPUT);
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(38400);
  delay(1000);
  Serial.println("Initializing Serial...");
  delay(1000);

  while (!Serial) {
    ; // wait for serial port to connect
  }

  startTime = millis();

  // initialize device
  // Serial.println("Initializing I2C devices...");


  iaq.initialize();
  if (!iaq.testConnection()) {
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    delay(500);
  }
}

void loop() {

  // display predicted CO2 concentration based on human induced volatile organic compounds (VOC) detection (in ppm VOC + CO2 equivalents)
  // during the first 6 hours of continuous operation (burn-in) the module will display 450ppm
  // the successful burn-in is saved to the EEPROM, the run-in time after restart is 15min
  Serial.print("CO2 = ");
  Serial.print(airQuality);
  Serial.print(" ");
  Serial.print("[ppm] 450-2000");

  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);

  // wait five seconds
  delay(5000);

  // read seansor
  iAQstatus = iaq.getIaqstatus();

  // display predicted CO2 concentration based on human induced volatile organic compounds (VOC) detection (in ppm VOC + CO2 equivalents)
  // during the first 6 hours of continuous operation (burn-in) the module will display 450ppm
  // the successful burn-in is saved to the EEPROM, the run-in time after restart is 15min
  Serial.print("  --  Status = ");
  if (iAQstatus == 0)
      {
           Serial.print("OK");
      }
      if (iAQstatus == 1)
      {
           Serial.print("BUSY");
      }
      if (iAQstatus == 16)
      {
           Serial.print("WARUMUP");
      }
      if (iAQstatus == 128)
      {
           Serial.print("ERROR");
      }

  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);

  // wait five seconds
  delay(5000);

  // read seansor
  airTvoc = iaq.getIaqtvoc();

  // display predicted CO2 concentration based on human induced volatile organic compounds (VOC) detection (in ppm VOC + CO2 equivalents)
  // during the first 6 hours of continuous operation (burn-in) the module will display 450ppm
  // the successful burn-in is saved to the EEPROM, the run-in time after restart is 15min
  Serial.print("  --  TVoc = ");
  Serial.print(airTvoc);
  Serial.print(" ");
  Serial.println("[ppb] 125-600");

  // blink LED to indicate activity
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);

  // wait five seconds
  delay(5000);

}
