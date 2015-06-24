// I2Cdev library collection - Main I2C device class header file
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
// 11/28/2014 by Marton Sebok <sebokmarton@gmail.com>
// 24/06/2015 by Grégoire Surrel <gregoire.surrel@epfl.ch>
//
// Changelog:
//     2014-11-28 - ported to PIC18 peripheral library from Arduino code

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2013 Jeff Rowberg
Copyright (c) 2014 Marton Sebok
Copyright (c) 2015 Grégoire Surrel

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

#include <jendefs.h>

int8 I2Cdev_readBit(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint8 *data);
int8 I2Cdev_readBitW(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint16 *data);
int8 I2Cdev_readBits(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint8 *data);
int8 I2Cdev_readBitsW(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint16 *data);
int8 I2Cdev_readByte(uint8 devAddr, uint8 regAddr, uint8 *data);
int8 I2Cdev_readWord(uint8 devAddr, uint8 regAddr, uint16 *data);
int8 I2Cdev_readBytes(uint8 devAddr, uint8 regAddr, uint8 length, uint8 *data);
int8 I2Cdev_readWords(uint8 devAddr, uint8 regAddr, uint8 length, uint16 *data);

uint8 I2Cdev_writeBit(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint8 data);
uint8 I2Cdev_writeBitW(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint16 data);
uint8 I2Cdev_writeBits(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint8 data);
uint8 I2Cdev_writeBitsW(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint16 data);
uint8 I2Cdev_writeByte(uint8 devAddr, uint8 regAddr, uint8 data);
uint8 I2Cdev_writeWord(uint8 devAddr, uint8 regAddr, uint16 data);
uint8 I2Cdev_writeBytes(uint8 devAddr, uint8 regAddr, uint8 length, uint8 *data);
uint8 I2Cdev_writeWords(uint8 devAddr, uint8 regAddr, uint8 length, uint16 *data);

#endif /* _I2CDEV_H_ */
