// I2Cdev library collection - MYDEVSTUB I2C device class
// Based on [Manufacturer Name] MYDEVSTUB datasheet, [datasheet date]
// [current release date] by [Author Name] <[Author Email]>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     [YYYY-mm-dd] - updated some broken thing
//     [YYYY-mm-dd] - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 [Author Name], Jeff Rowberg

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

#include "MYDEVSTUB.h"

// ============================================================================
// DEVICE LIBRARY CONVENTIONS:
//
// 1. The class name should be the same as the device model if at all possible.
//    No spaces or hyphens, and ideally no underlines unless they're absolutely
//    necessary for clarity. ALL CAPS for model numbers, or FirstInitial caps
//    for full names. For example:
//      - ADXL345
//      - MPU6050
//      - TCA6424A
//      - PanelPilot
//
// 2. All #defines should use a device-specific prefix that is the same as the
//    all-caps version of the class name ("MYDEVSTUB_" in this example).
//
// 3. All #defines should be ALL CAPS, no matter what. This makes them clearly
//    distinctive from variables, classes, and functions.
//
// 4. Class methods and member variables should be named using camelCaps.
//
// 5. Every device class should contain an "initialize()" method which takes
//    no parameters and resets any important settings back to a known state,
//    ideally the defaults outlined in the datasheet. Some devices have a
//    RESET command available, which may be suitable. Some devices may not
//    require any specific initialization, but an empty method should be
//    created for consistency anyway.
//
// 6. Every device class should contain a "testConnection()" method which
//    returns TRUE if the device appears to be connected, or FALSE otherwise.
//    If a known ID register or device code is available, check for that. Since
//    such an ID register is not always available, at least check to make sure
//    that an I2C read may be performed on a specific register and that data
//    does actually come back (the I2Cdev class returns a "bytes read" value
//    for all read operations).
//
// 7. All class methods should be documented with useful information in Doxygen
//    format. You can take the info right out of the datasheet if you want to,
//    since that's likely to be helpful. Writing your own info is fine as well.
//    The goal is to have enough clear documentation right in the code that
//    someone can determine how the device works by studying the code alone.
//
// ============================================================================

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
 * @see MYDEVSTUB_DEFAULT_ADDRESS
 */
MYDEVSTUB::MYDEVSTUB() {
    devAddr = MYDEVSTUB_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see MYDEVSTUB_DEFAULT_ADDRESS
 * @see MYDEVSTUB_ADDRESS
 */
MYDEVSTUB::MYDEVSTUB(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 */
void MYDEVSTUB::initialize() {
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
bool MYDEVSTUB::testConnection() {
    if (I2Cdev::readByte(devAddr, MYDEVSTUB_RA_WHO_AM_I, buffer) == 1) {
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

uint8_t MYDEVSTUB::getMeasurement1() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, MYDEVSTUB_RA_MEASURE1, buffer);
    return buffer[0];
}

// MEASURE2 register, read-only

uint8_t MYDEVSTUB::getMeasurement2() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, MYDEVSTUB_RA_MEASURE2, buffer);
    return buffer[0];
}

// MEASURE3 register, read-only

uint8_t MYDEVSTUB::getMeasurement3() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, MYDEVSTUB_RA_MEASURE3, buffer);
    return buffer[0];
}

// CONFIG1 register, r/w

void MYDEVSTUB::reset() {
    // write a single bit to the RESET position in the CONFIG1 register
    I2Cdev::writeBit(devAddr, MYDEVSTUB_RA_CONFIG1, MYDEVSTUB_CONFIG1_RESET_BIT, 1);
}
bool MYDEVSTUB::getFIFOEnabled() {
    // read a single bit from the FIFO_EN position in the CONFIG1 regsiter
    I2Cdev::readBit(devAddr, MYDEVSTUB_RA_CONFIG1, MYDEVSTUB_CONFIG1_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MYDEVSTUB::setFIFOEnabled(bool enabled) {
    // write a single bit to the FIFO_EN position in the CONFIG1 regsiter
    I2Cdev::writeBit(devAddr, MYDEVSTUB_RA_CONFIG1, MYDEVSTUB_CONFIG1_FIFO_EN_BIT, enabled);
}

// CONFIG2 register, r/w

uint8_t MYDEVSTUB::getInterruptMode() {
    // reading a single bit from a register
    I2Cdev::readBit(devAddr, MYDEVSTUB_RA_CONFIG2, MYDEVSTUB_CONFIG2_INTMODE_BIT, buffer);
    return buffer[0];
}
void MYDEVSTUB::setInterruptMode(uint8_t mode) {
    // writing a single bit into a register
}
uint8_t MYDEVSTUB::getRate() {
    // reading multiple single bit from a register
    I2Cdev::readBits(devAddr, MYDEVSTUB_RA_CONFIG2, MYDEVSTUB_CONFIG2_RATE_BIT, MYDEVSTUB_CONFIG2_RATE_LENGTH, buffer);
    return buffer[0];
}
void MYDEVSTUB::setRate(uint8_t rate) {
    // writing multiple bits into a register
    I2Cdev::writeBits(devAddr, MYDEVSTUB_RA_CONFIG2, MYDEVSTUB_CONFIG2_RATE_BIT, MYDEVSTUB_CONFIG2_RATE_LENGTH, rate);
}

// DATA_* registers, r/w

uint16_t MYDEVSTUB::getData() {
    // reading two H/L bytes and bit-shifting them into a 16-bit value
    I2Cdev::readBytes(devAddr, MYDEVSTUB_RA_DATA_H, 2, buffer);
    return (buffer[0] << 8) + buffer[1];
}
void MYDEVSTUB::setData(uint16_t value) {
    // splitting a 16-bit value into two H/L bytes and writing them
    buffer[0] = value >> 8;
    buffer[1] = value & 0xFF;
    I2Cdev::writeBytes(devAddr, MYDEVSTUB_RA_DATA_H, 2, buffer);
}

// WHO_AM_I register, read-only

uint8_t MYDEVSTUB::getDeviceID() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, MYDEVSTUB_RA_WHO_AM_I, buffer);
    return buffer[0];
}
