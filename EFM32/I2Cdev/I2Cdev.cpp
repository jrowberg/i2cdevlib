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

#include "I2Cdev.h"

/** Default constructor.
 */
I2Cdev::I2Cdev() {
}

/** Initialize I2C0
 */
void I2Cdev::initialize() {
	CMU_ClockEnable(cmuClock_I2C0, true);

	GPIO_PinModeSet(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SDA_MODE, I2C_SDA_DOUT);
	GPIO_PinModeSet(I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_MODE, I2C_SCL_DOUT);

	I2C0->ROUTE |= I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN;

	const I2C_Init_TypeDef init = I2C_INIT_DEFAULT;
	I2C_Init(I2C0, &init);

	I2C_Enable(I2C0, true);
}

/** Enable or disable I2C
 * @param isEnabled true = enable, false = disable
 */
void I2Cdev::enable(bool isEnabled) {
  
  I2C_Enable(I2C0, isEnabled);
  CMU_ClockEnable(cmuClock_I2C0, isEnabled);
  
}

I2C_TransferReturn_TypeDef I2Cdev::transfer(I2C_TransferSeq_TypeDef *seq, uint16_t timeout) {

  I2C_TransferReturn_TypeDef ret;
  /* Do a polled transfer */
  ret = I2C_TransferInit(I2C0, seq);

  while (ret == i2cTransferInProgress && timeout--) {
    ret = I2C_Transfer(I2C0);
  }

  return(ret);
  
}

/** Default timeout value for read operations.
 */
uint16_t I2Cdev::readTimeout = I2CDEV_DEFAULT_READ_TIMEOUT;
/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout) {
    uint8_t b;
    uint8_t count = readByte(devAddr, regAddr, &b, timeout);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = readByte(devAddr, regAddr, &b, timeout)) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout) {
    return readBytes(devAddr, regAddr, 1, data, timeout);
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return I2C_TransferReturn_TypeDef http://downloads.energymicro.com/documentation/doxygen/group__I2C.html
 */
int8_t I2Cdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout) {

  I2C_TransferSeq_TypeDef seq;
  
  uint8_t regid[1];
 
  seq.addr = devAddr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  
  /* Select register to be read */
  regid[0] = regAddr;
  seq.buf[0].data = regid;
  seq.buf[0].len = 1;
 
  /* 1 bytes reg */
  seq.buf[1].data = data;
  seq.buf[1].len = length;
  
  if (transfer(&seq, timeout) == i2cTransferDone) {
	  return seq.buf[1].len;
  } else {
	  return false;
  }
  
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(devAddr, regAddr, b);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (readByte(devAddr, regAddr, &b) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return writeByte(devAddr, regAddr, b);
    } else {
        return false;
    }
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {

  I2C_TransferSeq_TypeDef seq;

  uint8_t writeData[3];

  seq.addr = devAddr << 1;
  seq.flags = I2C_FLAG_WRITE;

  /* Select register to be written */
  writeData[0] = regAddr;
  seq.buf[0].data = writeData;

  /* Only 1 byte reg */
  writeData[1] = data;
  seq.buf[0].len = 2;

  if (transfer(&seq) == i2cTransferDone) {
  	  return true;
    } else {
  	  return false;
    }

}
