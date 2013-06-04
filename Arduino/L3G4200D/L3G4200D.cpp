// I2Cdev library collection - L3G4200D I2C device class header file
// Based on STMicroelectronics L3G4200D datasheet rev. 3, 12/2010
// TODO Add release date here
// [current release date] by Jonathan "j3rn" Arnett <j3rn@j3rn.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// TODO Add initial release date here as well
// Changelog:
//     [YYYY-mm-dd] - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jonathan Arnett, Jeff Rowberg

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

#include "L3G4200D.h"

/* ============================================================================
   I2Cdev Class Quick Primer:

   The I2Cdev class provides simple methods for reading and writing from I2C
   device registers without messing with the underlying TWI/I2C functions. You
   just specify the device address, register address, and source or destination
   data according to which action you are doing. Here is the list of relevant
   function prototypes from the I2Cdev class (more info in the .cpp/.h files):

    static int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);

    static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
    static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
    static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
    static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
    static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
    static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
    static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

   Note that ALL OF THESE METHODS ARE STATIC. No I2Cdev object is needed; just
   use the static class methods.
   
   Also note that the first two parameters of every one of these methods are
   the same: "devAddr" and "regAddr". For every method, you need to specify the
   target/slave address and the register address.
   
   If your device uses 8-bit registers, you will want to use the following:
       readBit, readBits, readByte, readBytes
       writeBit, writeBits, writeByte, writeBytes
       
   ...but if it uses 16-bit registers, you will want to use these instead:
       readBitW, readBitsW, readWord, readWords
       writeBitW, writeBitsW, writeWord, writeWords
       
   Here's a sample of how to use a few of the methods. Note that in each case, 
   the "buffer" variable is a uint8_t array or pointer, and the "value" variable
   (in three of the write examples) is a uint8_t single byte. The multi-byte
   write methods still require an array or pointer.

       READ 1 BIT FROM DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       bytesRead = I2Cdev::readBit(0x68, 0x02, 4, buffer);

       READ 3 BITS FROM DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       bytesRead = I2Cdev::readBits(0x68, 0x02, 4, 3, buffer);

       READ 1 BYTE FROM DEVICE 0x68, REGISTER 0x02
       bytesRead = I2Cdev::readByte(0x68, 0x02, buffer);

       READ 2 BYTES FROM DEVICE 0x68, REGISTER 0x02 (AND 0x03 FOR 2ND BYTE)
       bytesRead = I2Cdev::readBytes(0x68, 0x02, 2, buffer);

       WRITE 1 BIT TO DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       status = I2Cdev::writeBit(0x68, 0x02, 4, value);

       WRITE 3 BITS TO DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       status = I2Cdev::writeBits(0x68, 0x02, 4, 3, value);

       WRITE 1 BYTE TO DEVICE 0x68, REGISTER 0x02
       status = I2Cdev::writeByte(0x68, 0x02, value);

       WRITE 2 BYTES TO DEVICE 0x68, REGISTER 0x02 (AND 0x03 FOR 2ND BYTE)
       status = I2Cdev::writeBytes(0x68, 0x02, 2, buffer);
       
   The word-based methods are exactly the same, except they use 16-bit variables
   instead of 8-bit ones.

   ============================================================================ */

/** Default constructor, uses default I2C address.
 * @see L3G4200D_DEFAULT_ADDRESS
 */
L3G4200D::L3G4200D() {
    devAddr = L3G4200D_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see L3G4200D_DEFAULT_ADDRESS
 * @see L3G4200D_ADDRESS
 */
L3G4200D::L3G4200D(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 */
void L3G4200D::initialize() {
    // ----------------------------------------------------------------------------
    // STUB TODO:
    // Perform any important initialization here. Maybe nothing is required, but
    // the method should exist anyway.
    // ----------------------------------------------------------------------------
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool L3G4200D::testConnection() {
    if (I2Cdev::readByte(devAddr, L3G4200D_RA_WHO_AM_I, buffer) == 1) {
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// STUB TODO:
// Define methods to fully cover all available functionality provided by the
// device, according to the datasheet and/or register map. Unless there is very
// clear reason not to, try to follow the get/set naming convention for all
// values, for instance:
//   - uint8_t getThreshold()
//   - void setThreshold(uint8_t threshold)
//   - uint8_t getRate()
//   - void setRate(uint8_t rate)
//
// Some methods may be named differently if it makes sense. As long as all
// functionality is covered, that's the important part. The methods here are
// only examples and should not be kept for your real device.
// ----------------------------------------------------------------------------

// MEASURE1 register, read-only

uint8_t L3G4200D::getMeasurement1() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, L3G4200D_RA_MEASURE1, buffer);
    return buffer[0];
}

// MEASURE2 register, read-only

uint8_t L3G4200D::getMeasurement2() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, L3G4200D_RA_MEASURE2, buffer);
    return buffer[0];
}

// MEASURE3 register, read-only

uint8_t L3G4200D::getMeasurement3() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, L3G4200D_RA_MEASURE3, buffer);
    return buffer[0];
}

// CONFIG1 register, r/w

void L3G4200D::reset() {
    // write a single bit to the RESET position in the CONFIG1 register
    I2Cdev::writeBit(devAddr, L3G4200D_RA_CONFIG1, L3G4200D_CONFIG1_RESET_BIT, 1);
}
bool L3G4200D::getFIFOEnabled() {
    // read a single bit from the FIFO_EN position in the CONFIG1 regsiter
    I2Cdev::readBit(devAddr, L3G4200D_RA_CONFIG1, L3G4200D_CONFIG1_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void L3G4200D::setFIFOEnabled(bool enabled) {
    // write a single bit to the FIFO_EN position in the CONFIG1 regsiter
    I2Cdev::writeBit(devAddr, L3G4200D_RA_CONFIG1, L3G4200D_CONFIG1_FIFO_EN_BIT, enabled);
}

// CONFIG2 register, r/w

uint8_t L3G4200D::getInterruptMode() {
    // reading a single bit from a register
    I2Cdev::readBit(devAddr, L3G4200D_RA_CONFIG2, L3G4200D_CONFIG2_INTMODE_BIT, buffer);
    return buffer[0];
}
void L3G4200D::setInterruptMode(uint8_t mode) {
    // writing a single bit into a register
}
uint8_t L3G4200D::getRate() {
    // reading multiple single bit from a register
    I2Cdev::readBits(devAddr, L3G4200D_RA_CONFIG2, L3G4200D_CONFIG2_RATE_BIT, L3G4200D_CONFIG2_RATE_LENGTH, buffer);
    return buffer[0];
}
void L3G4200D::setRate(uint8_t rate) {
    // writing multiple bits into a register
    I2Cdev::writeBits(devAddr, L3G4200D_RA_CONFIG2, L3G4200D_CONFIG2_RATE_BIT, L3G4200D_CONFIG2_RATE_LENGTH, rate);
}

// DATA_* registers, r/w

uint16_t L3G4200D::getData() {
    // reading two H/L bytes and bit-shifting them into a 16-bit value
    I2Cdev::readBytes(devAddr, L3G4200D_RA_DATA_H, 2, buffer);
    return (buffer[0] << 8) + buffer[1];
}
void L3G4200D::setData(uint16_t value) {
    // splitting a 16-bit value into two H/L bytes and writing them
    buffer[0] = value >> 8;
    buffer[1] = value & 0xFF;
    I2Cdev::writeBytes(devAddr, L3G4200D_RA_DATA_H, 2, buffer);
}

// WHO_AM_I register, read-only

uint8_t L3G4200D::getDeviceID() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, L3G4200D_RA_WHO_AM_I, buffer);
    return buffer[0];
}
