// I2Cdev library collection - AK8975 I2C device class header file
// Based on AKM AK8975/B datasheet, 12/2009
// 8/27/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-08-27 - initial release

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

#include "AK8975.h"

#if (I2CDEV_IMPLEMENTATION == I2CDEV_MSP430)
#define delay(x) _nop()
#endif

/** Default constructor, uses default I2C address.
 * @see AK8975_DEFAULT_ADDRESS
 */
AK8975::AK8975() {
    devAddr = AK8975_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see AK8975_DEFAULT_ADDRESS
 * @see AK8975_ADDRESS_00
 */
AK8975::AK8975(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * No specific pre-configuration is necessary for this device.
 */
void AK8975::initialize() {
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool AK8975::testConnection() {
    if (I2Cdev::readByte(devAddr, AK8975_RA_WIA, buffer) == 1) {
        return (buffer[0] == 0x48);
    }
    return false;
}

// WIA register

uint8_t AK8975::getDeviceID() {
    I2Cdev::readByte(devAddr, AK8975_RA_WIA, buffer);
    return buffer[0];
}

// INFO register

uint8_t AK8975::getInfo() {
    I2Cdev::readByte(devAddr, AK8975_RA_INFO, buffer);
    return buffer[0];
}

// ST1 register

bool AK8975::getDataReady() {
    I2Cdev::readBit(devAddr, AK8975_RA_ST1, AK8975_ST1_DRDY_BIT, buffer);
    return buffer[0];
}

// H* registers
void AK8975::getHeading(int16_t *x, int16_t *y, int16_t *z) {
    I2Cdev::writeByte(devAddr, AK8975_RA_CNTL, AK8975_MODE_SINGLE);
    delay(10);
    I2Cdev::readBytes(devAddr, AK8975_RA_HXL, 6, buffer);
    *x = (((int16_t)buffer[1]) << 8) | buffer[0];
    *y = (((int16_t)buffer[3]) << 8) | buffer[2];
    *z = (((int16_t)buffer[5]) << 8) | buffer[4];
}
int16_t AK8975::getHeadingX() {
    I2Cdev::writeByte(devAddr, AK8975_RA_CNTL, AK8975_MODE_SINGLE);
    delay(10);
    I2Cdev::readBytes(devAddr, AK8975_RA_HXL, 2, buffer);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}
int16_t AK8975::getHeadingY() {
    I2Cdev::writeByte(devAddr, AK8975_RA_CNTL, AK8975_MODE_SINGLE);
    delay(10);
    I2Cdev::readBytes(devAddr, AK8975_RA_HYL, 2, buffer);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}
int16_t AK8975::getHeadingZ() {
    I2Cdev::writeByte(devAddr, AK8975_RA_CNTL, AK8975_MODE_SINGLE);
    delay(10);
    I2Cdev::readBytes(devAddr, AK8975_RA_HZL, 2, buffer);
    return (((int16_t)buffer[1]) << 8) | buffer[0];
}

// ST2 register
bool AK8975::getOverflowStatus() {
    I2Cdev::readBit(devAddr, AK8975_RA_ST2, AK8975_ST2_HOFL_BIT, buffer);
    return buffer[0];
}
bool AK8975::getDataError() {
    I2Cdev::readBit(devAddr, AK8975_RA_ST2, AK8975_ST2_DERR_BIT, buffer);
    return buffer[0];
}

// CNTL register
uint8_t AK8975::getMode() {
    I2Cdev::readBits(devAddr, AK8975_RA_CNTL, AK8975_CNTL_MODE_BIT, AK8975_CNTL_MODE_LENGTH, buffer);
    return buffer[0];
}
void AK8975::setMode(uint8_t mode) {
    I2Cdev::writeBits(devAddr, AK8975_RA_CNTL, AK8975_CNTL_MODE_BIT, AK8975_CNTL_MODE_LENGTH, mode);
}
void AK8975::reset() {
    I2Cdev::writeBits(devAddr, AK8975_RA_CNTL, AK8975_CNTL_MODE_BIT, AK8975_CNTL_MODE_LENGTH, AK8975_MODE_POWERDOWN);
}

// ASTC register
void AK8975::setSelfTest(bool enabled) {
    I2Cdev::writeBit(devAddr, AK8975_RA_ASTC, AK8975_ASTC_SELF_BIT, enabled);
}

// I2CDIS
void AK8975::disableI2C() {
    I2Cdev::writeBit(devAddr, AK8975_RA_I2CDIS, AK8975_I2CDIS_BIT, true);
}

// ASA* registers
void AK8975::getAdjustment(int8_t *x, int8_t *y, int8_t *z) {
    I2Cdev::readBytes(devAddr, AK8975_RA_ASAX, 3, buffer);
    *x = buffer[0];
    *y = buffer[1];
    *z = buffer[2];
}
void AK8975::setAdjustment(int8_t x, int8_t y, int8_t z) {
    buffer[0] = x;
    buffer[1] = y;
    buffer[2] = z;
    I2Cdev::writeBytes(devAddr, AK8975_RA_ASAX, 3, buffer);
}
uint8_t AK8975::getAdjustmentX() {
    I2Cdev::readByte(devAddr, AK8975_RA_ASAX, buffer);
    return buffer[0];
}
void AK8975::setAdjustmentX(uint8_t x) {
    I2Cdev::writeByte(devAddr, AK8975_RA_ASAX, x);
}
uint8_t AK8975::getAdjustmentY() {
    I2Cdev::readByte(devAddr, AK8975_RA_ASAY, buffer);
    return buffer[0];
}
void AK8975::setAdjustmentY(uint8_t y) {
    I2Cdev::writeByte(devAddr, AK8975_RA_ASAY, y);
}
uint8_t AK8975::getAdjustmentZ() {
    I2Cdev::readByte(devAddr, AK8975_RA_ASAZ, buffer);
    return buffer[0];
}
void AK8975::setAdjustmentZ(uint8_t z) {
    I2Cdev::writeByte(devAddr, AK8975_RA_ASAZ, z);
}
