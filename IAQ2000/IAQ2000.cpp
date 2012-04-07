// I2Cdev library collection - iAQ-2000 I2C device class
// Based on AppliedSensor iAQ-2000 Interface Description, Version PA1, 2009
// 2012-04-01 by Peteris Skorovs <pskorovs@gmail.com>
//
// This I2C device library is using (and submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-04-01 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
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

#include "IAQ2000.h"

/** Default constructor, uses default I2C address.
 * @see IAQ2000_DEFAULT_ADDRESS
 */
IAQ2000::IAQ2000() {
    devAddr = IAQ2000_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see IAQ2000_DEFAULT_ADDRESS
 * @see IAQ2000_ADDRESS
 */
IAQ2000::IAQ2000(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 */
void IAQ2000::initialize() {
    // Nothing is required, but
    // the method should exist anyway.
}

/** Very primitive method o verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool IAQ2000::testConnection() {
    if (getIaq() >= 450) {
        return true;
    }
    else {
    return false;
    }
}

/** Read iAQ-2000 indoor air quality sensor.
 * @return Predicted CO2 concentration based on human induced volatile organic compounds (VOC) detection (in ppm VOC + CO2 equivalents)
 */
uint16_t IAQ2000::getIaq() {
  // read bytes from the DATA1 AND DATA2 registers and bit-shifting them into a 16-bit value
  readAllBytes(devAddr, 2, buffer);
  return ((buffer[0] << 8) | buffer[1]);
}

/**  Read bytes from a slave device.
 * This is a "stripped-down" version of the standard Jeff Rowberg's I2Cdev::readBytes method
 * intended to provide compatibility with iAQ-2000,
 * which apparently does not support setting of an address pointer to indicate from which position is to start read from.
 * @param devAddr Address of the slave device to read bytes from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (0 indicates failure)
 */
int8_t IAQ2000::readAllBytes(uint8_t devAddr, uint8_t length, uint8_t *data, uint16_t timeout) {
#ifdef I2CDEV_SERIAL_DEBUG
    Serial.print("I2C (0x");
    Serial.print(devAddr, HEX);
    Serial.print(") reading ");
    Serial.print(length, DEC);
    Serial.print(" bytes...");
#endif
    
    int8_t count = 0;

    Wire.requestFrom(devAddr, length);
    
    uint32_t t1 = millis();
    for (; Wire.available() && (timeout == 0 || millis() - t1 < timeout); count++) {
#if ((I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO < 100) || I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_NBWIRE)
        data[count] = Wire.receive();
#elif (I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO >= 100)
        data[count] = Wire.read();
#endif
#ifdef I2CDEV_SERIAL_DEBUG
        Serial.print(data[count], HEX);
        if (count + 1 < length) Serial.print(" ");
#endif
    }
    if (timeout > 0 && millis() - t1 >= timeout && count < length) count = -1; // timeout
    
#ifdef I2CDEV_SERIAL_DEBUG
    Serial.print(". Done (");
    Serial.print(count, DEC);
    Serial.println(" read).");
#endif
    
    return count;
}
