// I2Cdev library collection - Main I2C device class header file
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
// 12/28/2017 by  Jihoon Lee <anfwkrpdnjs179@gmail.com>
//
// Changelog:
//     2017-12-28 - Initial commit. Tested on MSP430F5529
//                  Currently only support MSP430F55xx and MSP430X261x

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2013 Jeff Rowberg
Copyright (c) 2017 Jihoon Lee
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
#include <stdint.h>
#ifndef MSP430_I2CDEV_H_
#define MSP430_I2CDEV_H_

#define MSP430x261x     0                   // TBD: Add more variants as implementations are confirmed
#define MSP430F55xx     1
#define I2CDev_Driver   Driver_I2C2

// Device Variant should be declared here
#define DEVICE          MSP430F55xx         // CHANGE THIS LINE AS NECESSARY

#if DEVICE==MSP430F55xx
#include "msp430f55xx_i2c.h"
#elif DEVICE==MSP430x261x
#include "msp430x261x_i2c.h"
#endif

uint8_t  I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
uint8_t  I2Cdev_readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data);
uint8_t  I2Cdev_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
uint8_t  I2Cdev_readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data);
uint8_t  I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
uint8_t  I2Cdev_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data);
uint8_t  I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
uint8_t  I2Cdev_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

uint8_t  I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
uint8_t  I2Cdev_writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
uint8_t  I2Cdev_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
uint8_t  I2Cdev_writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
uint8_t  I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);//
uint8_t  I2Cdev_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);//
uint8_t  I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);//
uint8_t  I2Cdev_writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);//

#endif
