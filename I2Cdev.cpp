// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// 7/25/2011 by Jeff Rowberg <jeff@rowberg.net>

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

#ifndef LUFA
    #include "WProgram.h"
#endif

#include "I2Cdev.h"

#ifdef round
    #undef round
#endif

//#define I2CDEV_SERIAL_DEBUG

/** Default constructor.
 * Note that you MUST specifically set the device address to make anything work
 * if you use this constructor.
 * @param address I2C bus device address
 * @see setDeviceAddress()
 */
I2Cdev::I2Cdev() {
    deviceAddress = 0;
}

/** Specific address constructor.
 * @param address I2C bus device address
 */
I2Cdev::I2Cdev(uint8_t address) {
    deviceAddress = address;
}

/** Read a single bit from a device register.
 * @param address Register address to read from
 * @param bit Bit position to read (0-7)
 * @return Single bit value
 */
uint8_t I2Cdev::readBit(uint8_t address, uint8_t bit) {
    return (readByte(address) & (1 << bit));
}

/** Read multiple bits from a device register.
 * @param address Register address to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @return Right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 */
uint8_t I2Cdev::readBits(uint8_t address, uint8_t bitStart, uint8_t length) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t b = readByte(address), r = 0;
    for (uint8_t i = bitStart; i > bitStart - length; i--) {
        r |= (b & (1 << i));
    }
    r >>= (bitStart - length + 1);
    return r;
}

/** Read single byte from a device register.
 * @param address Register address to read from
 * @return Byte value read from device
 */
uint8_t I2Cdev::readByte(uint8_t address) {
    uint8_t b;
    readBytes(address, 1, &b);
    return b;
}

/** Read multiple bytes from a device register.
 * @param address First register address to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 */
void I2Cdev::readBytes(uint8_t address, uint8_t length, uint8_t *data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(deviceAddress, HEX);
        Serial.print(") reading ");
        Serial.print(length, HEX);
        Serial.print(" bytes from 0x");
        Serial.print(address, HEX);
        Serial.print("...");
    #endif

    Wire.beginTransmission(deviceAddress);
    Wire.send(address);
    Wire.endTransmission();

    Wire.beginTransmission(deviceAddress);
    Wire.requestFrom(deviceAddress, length);    // request 6 bytes from device

    for (uint8_t i = 0; Wire.available(); i++) {
        data[i] = Wire.receive();
        #ifdef I2CDEV_SERIAL_DEBUG
            Serial.print(data[i], HEX);
            Serial.print(" ");
        #endif
    }

    Wire.endTransmission();

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.println("done.");
    #endif
}

/** write a single bit from a device register.
 * @param address Register address to write to
 * @param bit Bit position to write (0-7)
 * @param value New bit value to write
 */
void I2Cdev::writeBit(uint8_t address, uint8_t bit, uint8_t value) {
    uint8_t b = readByte(address);
    b = value ? (b | (1 << bit)) : (b & ~(1 << bit));
    writeByte(address, b);
}

/** Write multiple bits to a device register.
 * @param address Register address to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param value Right-aligned value to write
 */
void I2Cdev::writeBits(uint8_t address, uint8_t bitStart, uint8_t length, uint8_t value) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 01000000 shift left (8 - length)    ]
    // 00001000 shift right (7 - bitStart) ] --- two shifts ensure all non-important bits are 0
    // 11100011 mask byte
    // 10101111 original value (sample)
    // 10100011 original & mask
    // 10101011 masked | value
    uint8_t b = readByte(address);
    uint8_t mask = (0xFF << (8 - length)) | (0xFF >> (bitStart + length - 1));
    value <<= (8 - length);
    value >>= (7 - bitStart);
    b &= mask;
    b |= value;
    writeByte(address, b);
}

/** Write single byte to a device register.
 * @param address Register address to write to
 * @param value New byte value write
 */
void I2Cdev::writeByte(uint8_t address, uint8_t value) {
    writeBytes(address, 1, &value);
}

/** Write multiple bytes to a device register.
 * @param address First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 */
void I2Cdev::writeBytes(uint8_t address, uint8_t length, uint8_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(deviceAddress, HEX);
        Serial.print(") w ");
        Serial.print(length, HEX);
        Serial.print(" bytes to 0x");
        Serial.print(address, HEX);
        Serial.print("...");
    #endif
    Wire.beginTransmission(deviceAddress);
    for (uint8_t i = 0; i < length; i++) {
        Wire.send(address + i);
        Wire.send(data[i]);
        #ifdef I2CDEV_SERIAL_DEBUG
            Serial.print(data[i], HEX);
            Serial.print(" ");
        #endif
    }
    Wire.endTransmission();
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.println("done.");
    #endif
}

/** Get current object's I2C bus device address.
 * @return Device address
 */
uint8_t I2Cdev::getDeviceAddress() {
    return deviceAddress;
}

/** Set current object's I2C bus device address.
 * @param New device address
 */
void I2Cdev::setDeviceAddress(uint8_t address) {
    deviceAddress = address;
}
