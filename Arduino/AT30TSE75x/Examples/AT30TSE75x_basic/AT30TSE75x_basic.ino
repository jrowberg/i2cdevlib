// I2Cdev library collection - AT30TSE752/754/758 I2C device class header file
// Based on ATMEL AT30TSE75 datasheet, 2013 Rev. Atmel-8751F-DTS-AT30TSE752-754-758-Datasheet_092013
// 2014-02-16 by Bartosz Kryza <bkryza@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2014-02-16 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2014 Bartosz Kryza, Jeff Rowberg

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
#include "AT30TSE75x.h"

//
// Initialize the sensor with A2-A0 pins grounded
// and variant with 8Kb of EEPROM memory.
//
AT30TSE75x tempSensor(0x00, AT30TSE75x_758);

void setup() {
  
  //
  // Initialize Wire and Serial
  //
  Wire.begin(); 
  Serial.begin(9600);
  Serial.println("Initializing temp sensor");
  
  //
  // Initialize the sensor with 12 bit conversion resolution
  //
  tempSensor.initialize();
  tempSensor.setConversionResolution(AT30TSE75x_RES_12BIT);

}



void loop() {
  
  //
  // Read Celcius temperature
  //
  float tempValue = tempSensor.getTemperatureCelcius();
  Serial.print("Current temperature: ");
  Serial.print(tempValue);
  Serial.println("C");
  
  //
  // Read Fahrenheit temperature
  //
  tempValue = tempSensor.getTemperatureFahrenheit();
  Serial.print("Current temperature: ");
  Serial.print(tempValue);
  Serial.println("F");
  
  //
  // Write/read EEPROM byte
  //
  uint8_t page = 16;
  uint8_t byteInPage = 5;
  tempSensor.writeEEPROMByte(16*page+byteInPage, 13);
  uint8_t eepromValue = tempSensor.readEEPROMByte(16*page+byteInPage);
  Serial.print("Read EEPROM byte: ");
  Serial.println(eepromValue);
  
  //
  // Write/read EEPROM page
  //
  uint8_t pageBytes[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  page = 3;
  tempSensor.writeEEPROMPage(16*page, pageBytes);
  
  // AT30TSE buffers pages before writing them to the EEPROM thus
  // delay is necessary between writing and reading a page from EEPROM
  // to ensure we get new values
  delay(10);
  
  uint8_t pageBytesRead[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
  tempSensor.readEEPROMPage(16*page, pageBytesRead);
  
  Serial.print("Read EEPROM page: ");
  for(int i=0; i<16; i++) {
     Serial.print(pageBytesRead[i]); Serial.print(" ");
  }
  Serial.println("");
  
  delay(2000);
  
}
