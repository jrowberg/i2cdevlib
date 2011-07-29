// I2Cdev library collection - Main I2C device class header file
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

#ifndef _I2CDEV_H_
#define _I2CDEV_H_

// -----------------------------------------------------------------------------
// I2C interface implementation setting
// -----------------------------------------------------------------------------
#define I2CDEV_IMPLEMENTATION       I2CDEV_ARDUINO_WIRE

// -----------------------------------------------------------------------------
// I2C interface implementation options
// -----------------------------------------------------------------------------
#define I2CDEV_RAW                  1
#define I2CDEV_ARDUINO_WIRE         2

// -----------------------------------------------------------------------------
// Arduino-style "Serial.print" debug constant (uncomment to enable)
// -----------------------------------------------------------------------------
//#define I2CDEV_SERIAL_DEBUG

#ifdef LUFA_ARDUINO_WRAPPER
    #include "ArduinoWrapper.h"
#else
    #if (I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE)
        #include <Wire.h>
        #include "WProgram.h"
    #endif
#endif

class I2Cdev {
    public:
        I2Cdev();
        I2Cdev(uint8_t address);
        uint8_t readBit(uint8_t address, uint8_t bit);
        uint8_t readBits(uint8_t address, uint8_t bitStart, uint8_t length);
        uint8_t readByte(uint8_t address);
        void readBytes(uint8_t address, uint8_t length, uint8_t *data);
        void writeBit(uint8_t address, uint8_t bit, uint8_t value);
        void writeBits(uint8_t address, uint8_t bitStart, uint8_t length, uint8_t value);
        void writeByte(uint8_t address, uint8_t value);
        void writeBytes(uint8_t address, uint8_t length, uint8_t *data);

        uint8_t getDeviceAddress();
        void setDeviceAddress(uint8_t address);

    private:
        uint8_t deviceAddress;
};

#endif /* _I2CDEV_H_ */
