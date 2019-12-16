// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// EFM32 stub port by Nicolas Baldeck <nicolas@pioupiou.fr>
// Based on Arduino's I2Cdev by Jeff Rowberg <jeff@rowberg.net>
//
// Changelog:
//      2015-01-02 - Initial release


/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2015 Jeff Rowberg, Nicolas Baldeck

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

#include <driver/i2c.h>

#define I2C_SDA_PORT gpioPortA
#define I2C_SDA_PIN 0
#define I2C_SDA_MODE gpioModeWiredAnd
#define I2C_SDA_DOUT 1

#define I2C_SCL_PORT gpioPortA
#define I2C_SCL_PIN 1
#define I2C_SCL_MODE gpioModeWiredAnd
#define I2C_SCL_DOUT 1

#define I2CDEV_DEFAULT_READ_TIMEOUT 1000

class I2Cdev {
    public:
        I2Cdev();

        static void initialize();
        static void enable(bool isEnabled);

        static int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
        //TODO static int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
        static int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
        //TODO static int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
        static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
        static int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
        static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
        //TODO static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);

        static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
        //TODO static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
        static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        //TODO static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
        static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
        static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        //TODO static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

        static uint16_t readTimeout;

    //private:
        static void SelectRegister(uint8_t dev, uint8_t reg);
        //static I2C_TransferReturn_TypeDef transfer(I2C_TransferSeq_TypeDef *seq, uint16_t timeout=I2Cdev::readTimeout);
};

#endif /* _I2CDEV_H_ */
