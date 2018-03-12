// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// Based on Arduino's I2Cdev by Jeff Rowberg <jeff@rowberg.net>
// BeagleBone Black (potencially other linux boards) port by Mateus Amarante <mateus.amarujo@gmail.com>
//
// Changelog:
//      2018-03-02 - Initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
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

#include <stdint.h>

#define DEFAULT_BBB_I2C_BUS 2

class I2Cdev
{
public:
  I2Cdev();
  I2Cdev(uint8_t busAddr);

  static int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
  static int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data);
  static int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
  static int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data);
  static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
  static int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data);
  static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
  static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

  static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
  static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
  static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
  static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
  static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
  static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
  static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
  static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

private:
  char path_[13]; // up to "/dev/i2c-255"
};

#endif /* _I2CDEV_H_ */
