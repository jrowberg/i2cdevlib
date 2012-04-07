// I2Cdev library collection - AD7746 I2C device class
// Based on Analog Devices AD7746 Datasheet, Revision 0, 2005
// 2012-04-01 by Peteris Skorovs <pskorovs@gmail.com>
//
// This I2C device library is using (and submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-04-01 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Peteris Skorovs

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

#include "AD7746.h"

/** Default constructor, uses default I2C address.
 * @see AD7746_DEFAULT_ADDRESS
 */
AD7746::AD7746() {
    devAddr = AD7746_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see AD7746_DEFAULT_ADDRESS
 * @see AD7746_ADDRESS
 */
AD7746::AD7746(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 */
void AD7746::initialize() {
    reset();
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool AD7746::testConnection() {
    if (I2Cdev::readByte(devAddr, AD7746_RA_STATUS, buffer)) {
        return true;
    }
    return false;
}

void AD7746::reset() {   
    
#ifdef I2CDEV_SERIAL_DEBUG
    Serial.print("I2C (0x");
    Serial.print(devAddr, HEX);
    Serial.print(") resetting");
    Serial.print("...");
#endif    

#if ((I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO < 100) || I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_NBWIRE)
    Wire.beginTransmission(devAddr);
    Wire.send((uint8_t) AD7746_RESET); // send reset
#elif (I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO >= 100)
    Wire.beginTransmission(devAddr);
    Wire.write((uint8_t) AD7746_RESET); // send reset
#endif
#if ((I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO < 100) || I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_NBWIRE)
    Wire.endTransmission();
#elif (I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE && ARDUINO >= 100)
    Wire.endTransmission();
#endif
    
#ifdef I2CDEV_SERIAL_DEBUG
    Serial.println(". Done.");
#endif
    
delay(1); //wait a tad for reboot
}


uint32_t AD7746::getCapacitance() {
    uint32_t capacitance;
    I2Cdev::readBytes(devAddr, 0, 4, buffer);
    capacitance = ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];
    
    return capacitance;
}


void AD7746::writeCapSetupRegister(uint8_t data) {
    I2Cdev::writeByte(devAddr, AD7746_RA_CAP_SETUP, data);
}

void AD7746::writeVtSetupRegister(uint8_t data) {
    I2Cdev::writeByte(devAddr, AD7746_RA_VT_SETUP, data);
}


void AD7746::writeExcSetupRegister(uint8_t data) {
    I2Cdev::writeByte(devAddr, AD7746_RA_EXC_SETUP, data);
}


void AD7746::writeConfigurationRegister(uint8_t data) {
    I2Cdev::writeByte(devAddr, AD7746_RA_CONFIGURATION, data);
}


void AD7746::writeCapDacARegister(uint8_t data) {
    I2Cdev::writeByte(devAddr, AD7746_RA_CAP_DAC_A, data);
}

void AD7746::writeCapDacBRegister(uint8_t data) {
    I2Cdev::writeByte(devAddr, AD7746_RA_CAP_DAC_B, data);
}
