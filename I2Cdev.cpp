// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// 7/31/2011 by Jeff Rowberg <jeff@rowberg.net>

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

#include "I2Cdev.h"

/** Default constructor.
 */
I2Cdev::I2Cdev() {
}

/** Read a single bit from a device register.
 * @param regAddr Register regAddr to read from
 * @param bit Bit position to read (0-7)
 * @return Single bit value
 */
bool I2Cdev::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from a device register.
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @return Right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 */
bool I2Cdev::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b, r = 0;
    if ((count = readByte(devAddr, regAddr, &b)) != 0) {
        for (uint8_t i = bitStart; i > bitStart - length; i--) {
            r |= (b & (1 << i));
        }
        r >>= (bitStart - length + 1);
        *data = r;
    }
    return count;
}

/** Read single byte from a device register.
 * @param regAddr Register regAddr to read from
 * @return Byte value read from device
 */
bool I2Cdev::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
    return readBytes(devAddr, regAddr, 1, data);
}

/** Read multiple bytes from a device register.
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 */
uint8_t I2Cdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") reading ");
        Serial.print(length, HEX);
        Serial.print(" bytes from 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif

    uint8_t count = 0;

    Wire.beginTransmission(devAddr);
    Wire.send(regAddr);
    Wire.endTransmission();

    Wire.beginTransmission(devAddr);
    Wire.requestFrom(devAddr, length);    // request 6 bytes from device

    for (; Wire.available(); count++) {
        data[count] = Wire.receive();
        #ifdef I2CDEV_SERIAL_DEBUG
            Serial.print(data[count], HEX);
            Serial.print(" ");
        #endif
    }

    Wire.endTransmission();

    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print(". Done (");
        Serial.print(count, DEC);
        Serial.println(" read).");
    #endif
    
    return count;
}

/** write a single bit from a device register.
 * @param regAddr Register regAddr to write to
 * @param bit Bit position to write (0-7)
 * @param value New bit value to write
 */
bool I2Cdev::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(devAddr, regAddr, b);
}

/** Write multiple bits to a device register.
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param value Right-aligned value to write
 */
bool I2Cdev::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 01000000 shift left (8 - length)    ]
    // 00001000 shift right (7 - bitStart) ] --- two shifts ensure all non-important bits are 0
    // 11100011 mask byte
    // 10101111 original value (sample)
    // 10100011 original & mask
    // 10101011 masked | value
    uint8_t b;
    if (readByte(devAddr, regAddr, &b) != 0) {
        uint8_t mask = (0xFF << (8 - length)) | (0xFF >> (bitStart + length - 1));
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return writeByte(devAddr, regAddr, b);
    } else {
        return false;
    }
}

/** Write single byte to a device register.
 * @param regAddr Register address to write to
 * @param value New byte value write
 */
bool I2Cdev::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return writeBytes(devAddr, regAddr, 1, &data);
}

/** Write multiple bytes to a device register.
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 */
bool I2Cdev::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.print("I2C (0x");
        Serial.print(devAddr, HEX);
        Serial.print(") writing ");
        Serial.print(length, HEX);
        Serial.print(" bytes to 0x");
        Serial.print(regAddr, HEX);
        Serial.print("...");
    #endif
    Wire.beginTransmission(devAddr);
    for (uint8_t i = 0; i < length; i++) {
        Wire.send(regAddr + i);
        Wire.send(data[i]);
        #ifdef I2CDEV_SERIAL_DEBUG
            Serial.print(data[i], HEX);
            Serial.print(" ");
        #endif
    }
    // docs say this returns a byte, but it causes a compiler error
    //uint8_t status = Wire.endTransmission();
    Wire.endTransmission();
    #ifdef I2CDEV_SERIAL_DEBUG
        Serial.println(". Done.");
    #endif
    return true; //status == 0;
}
