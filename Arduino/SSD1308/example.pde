// SSD1308 device demonstration Arduino sketch
// 9/02/2011 by Andrew Schamp <schamp@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib

/* ============================================
SSD1308 device library code is placed under the MIT license
Copyright (c) 2011 Andrew Schamp

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

#define SSD1308_USE_FONT
#include "SSD1308.h"
#undef SSD1308_USE_FONT

SSD1308 oled;

void setup() {
    // join I2C bus
    Wire.begin();
    
    // initialize serial communication
    Serial.begin(38400);
    
    // initialize all devices
    Serial.println("Initializing I2C devices...");
    oled.initialize();

}

void loop() {
  oled.setDisplayOff();
  Serial.println("display off.");
  delay(1000);
  oled.setDisplayOn();
  Serial.println("display on.");
  delay(1000);
  oled.clearDisplay();
  Serial.println("display cleared.");
  delay(1000);
  oled.fillDisplay();
  Serial.println("display filled.");
  delay(1000);
  oled.clearDisplay();
  Serial.println("display cleared.");
  delay(1000);
  oled.writeString(0, 0, 8, "Foobar!!");
  Serial.println("printed something.");
  delay(5000);
  oled.writeString(1, 8, 8, "baz quux");
  Serial.println("printed something.");
  delay(5000);
  oled.writeString(2, 0, 272, "a long, rather lengthy, extended string passage thing, eh, that just goes on, and on, and on, and on, and on, and on, and on, yes, further, continuing, extending, expanding beyond all reason or sanity!!!!! and yet, there's more!  so much more!  for ever and ever, oh yeah");
  Serial.println("printed something.");
  delay(5000);
  delay(3000);
}
