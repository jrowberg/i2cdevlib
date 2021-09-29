// I2Cdev library collection - Main I2C device class header file
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
// 03/28/2017 by Kamnev Yuriy <kamnev.u1969@gmail.com>
//
// Changelog:
//     2017-03-28 - ported to STM32 using Keil MDK Pack

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2013 Jeff Rowberg
Copyright (c) 2017 Kamnev Yuriy

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
#include <string.h>

#include <I2C_STM32F10x.h>

extern ARM_DRIVER_I2C I2CDev_Driver;


#define _i2c_transmit(dev_addr, data, len, pending) \
	I2CDev_Driver.MasterTransmit(dev_addr, data, len, pending);\
	while(I2CDev_Driver.GetStatus().busy)


#define _i2c_receive(dev_addr, data, len, pending) \
	I2CDev_Driver.MasterReceive(dev_addr, data, len, pending); \
	while(I2CDev_Driver.GetStatus().busy)


#define i2c_transmit_ack(dev_addr, data, len) 	_i2c_transmit(dev_addr, data, len, true)
#define i2c_transmit_nack(dev_addr, data, len) 	_i2c_transmit(dev_addr, data, len, false)

#define i2c_receive_ack(dev_addr, data, len)	_i2c_receive(dev_addr, data, len, true)
#define i2c_receive_nack(dev_addr, data, len)	_i2c_receive(dev_addr, data, len, false)


/** Read several byte from an 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to read from
 * @param len 		How many bytes to read
 * @param data 		Buffer to save data into
 * @return Status of read operation (0 = success, <0 = error)
 */
int8_t I2Cdev_readBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *data) {
	int8_t err = 0;
	uint8_t reg_data[1] = {reg_addr};

	err = i2c_transmit_ack(dev_addr, reg_data, 1);

	if(err < 0) {
		return err;
	}

	err = i2c_receive_nack(dev_addr, data, len);

	return err;
}


/** Read a single byte from a 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register reg_addr to read from
 * @param data 		Buffer to save data into
 * @return Status of read operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_readByte(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data) {
	return I2Cdev_readBytes(dev_addr, reg_addr, 1, data);
}


/** Read a several 16-bit words from a 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register reg_addr to read from
 * @param len		Number of words to read
 * @param data 		Buffer to save data into
 * @return Status of read operation (true = success)
 */
int8_t 	I2Cdev_readWords(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint16_t *data) {
	int8_t err;
	uint16_t bytes_num = len*2;

	uint8_t reg_info[1] = {reg_addr};
	err = i2c_transmit_ack(dev_addr, reg_info, 1);

	if(err < 0) {
		return err;
	}

	uint8_t words_in_bytes[bytes_num];
	err = i2c_receive_nack(dev_addr, words_in_bytes, bytes_num);

	if(err < 0) {
		return err;
	}

	uint8_t words_cnt = 0;
	for(uint16_t i=0; i<bytes_num; i+=2) {
		data[words_cnt++] = (words_in_bytes[i] << 8) | words_in_bytes[i+1];
	}

	return 0;
}


/** Read a single word from a 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to read from
 * @param data 		Container for single word
 * @return Status of read operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_readWord(uint8_t dev_addr, uint8_t reg_addr, uint16_t *data) {
	return I2Cdev_readWords(dev_addr, reg_addr, 1, data);
}


/** Read a single bit from a 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to read from
 * @param bitn 		Bit position to read (0-15)
 * @param data 		Container for single bit value
 * @return Status of read operation (0 = success, <0 = error)
 */
int8_t I2Cdev_readBit(uint8_t dev_addr, uint8_t reg_addr, uint8_t bitn, uint8_t *data) {
	int8_t err;

	err = I2Cdev_readByte(dev_addr, reg_addr, data);
	*data = (*data >> bitn) & 0x01;

	return err;
}


/** Read several bits from a 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to read from
 * @param start_bit First bit position to read (0-7)
 * @param len		Number of bits to read (<= 8)
 * @param data 		Container for right-aligned value
 * @return Status of read operation (0 = success, <0 = error)
 */
int8_t I2Cdev_readBits(uint8_t dev_addr, uint8_t reg_addr, uint8_t start_bit, 
		uint8_t len, uint8_t *data) 
{
	int8_t err;

	uint8_t b;
	if ((err = I2Cdev_readByte(dev_addr, reg_addr, &b)) == 0) {
		uint8_t mask = ((1 << len) - 1) << (start_bit - len + 1);
		b &= mask;
		b >>= (start_bit - len + 1);
		*data = b;
	}

	return err;
}


/** Read a single bit from a 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to read from
 * @param bit_n 	Bit position to read (0-15)
 * @param data 		Container for single bit value
 * @return Status of read operation (true = success)
 */
int8_t 	I2Cdev_readBitW(uint8_t dev_addr, uint8_t reg_addr, uint8_t bit_n, uint16_t *data) {
	int8_t err;

	err = I2Cdev_readWord(dev_addr, reg_addr, data);
	*data = (*data >> bit_n) & 0x01;

	return err;
}


/** Read several bits from a 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to read from
 * @param start_bit First bit position to read (0-15)
 * @param len		Number of bits to read (<= 16)
 * @param data 		Container for right-aligned value
 * @return Status of read operation (0 = success, <0 = error)
 */
int8_t I2Cdev_readBitsW(uint8_t dev_addr, uint8_t reg_addr, uint8_t start_bit,
		uint8_t len, uint16_t *data)
{
    int8_t err;
    uint16_t w;

    if ((err = I2Cdev_readWord(dev_addr, reg_addr, &w)) == 0) {
        uint16_t mask = ((1 << len) - 1) << (start_bit - len + 1);
        w &= mask;
        w >>= (start_bit - len + 1);
        *data = w;
    }

    return err;
}


/** Write multiple bytes to an 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	First register address to write to
 * @param len 		Number of bytes to write
 * @param data 		Buffer to copy new data from
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_writeBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *data) {
	int8_t err;
	uint8_t ts_data[len+1];

	ts_data[0] = reg_addr;
	memcpy(ts_data+1, data, len);

	err = i2c_transmit_nack(dev_addr, ts_data, len+1);
	return err;
}


/** Write single byte to an 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register address to write to
 * @param data 		New byte value to write
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_writeByte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
	int8_t err;

	uint8_t ts_data[2] = {reg_addr, data};
	err = i2c_transmit_nack(dev_addr, ts_data, 2);

	return err;
}


/** Write single 16-bit word to an 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register address to write to
 * @param data 		New byte value to write
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_writeWord(uint8_t dev_addr, uint8_t reg_addr, uint16_t data) {
	int8_t err;
	uint8_t ts_data[3] = {reg_addr, (data >> 8) & 0xFF, data & 0xFF};

	err = i2c_transmit_nack(dev_addr, ts_data, 3);
	return err;
}


/** Write multiple words to a 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	First register address to write to
 * @param len 		Number of words to write
 * @param data 		Buffer to copy new data from
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_writeWords(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint16_t *data) {
	uint16_t bytes_num = len*2+1;
	uint8_t bytes[bytes_num];

	bytes[0] = reg_addr;

	uint16_t bytes_pos = 1;
	for(uint8_t i=0; i<len; i++) {
		bytes[bytes_pos] = (data[i] >> 8) & 0xFF;
		bytes[bytes_pos+1] = data[i] & 0xFF;

		bytes_pos += 2;
	}

	return i2c_transmit_nack(dev_addr, bytes, bytes_num);
}


/** write a single bit in an 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to write to
 * @param bit_n 	Bit position to write (0-7)
 * @param data 		New bit value to write
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_writeBit(uint8_t dev_addr, uint8_t reg_addr, uint8_t bit_n, uint8_t data) {
	uint8_t b;
	int8_t err;

	err = I2Cdev_readByte(dev_addr, reg_addr, &b);
	if(err < 0) {
		return err;
	}

	b = (data != 0) ? (b | (1<<bit_n)) : (b &= ~(1<<bit_n));

	return I2Cdev_writeByte(dev_addr, reg_addr, b);
}


/** write a single bit in a 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to write to
 * @param bit_n 	Bit position to write (0-15)
 * @param data 		New bit value to write
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t 	I2Cdev_writeBitW(uint8_t dev_addr, uint8_t reg_addr, uint8_t bit_n, uint16_t data) {
	uint16_t w;
	I2Cdev_readWord(dev_addr, reg_addr, &w);

	w = (data != 0) ? (w | (1<<bit_n)) : (w &= ~(1<<bit_n));

	return I2Cdev_writeWord(dev_addr, reg_addr, w);
}


/** Write multiple bits in an 8-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to write to
 * @param start_bit First bit position to write (0-7)
 * @param len 		Number of bits to write (not more than 8)
 * @param data 		Right-aligned value to write
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t I2Cdev_writeBits(uint8_t dev_addr, uint8_t reg_addr, uint8_t start_bit,
		uint8_t len, uint8_t data)
{
    uint8_t b;
    int8_t err;

    if ((err = I2Cdev_readByte(dev_addr, reg_addr, &b)) == 0) {
        uint8_t mask = ((1 << len) - 1) << (start_bit - len + 1);
        data <<= (start_bit - len + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte

        return I2Cdev_writeByte(dev_addr, reg_addr, b);
    }
    else {
        return err;
    }
}


/** Write multiple bits in a 16-bit device register.
 * @param dev_addr 	I2C slave device address
 * @param reg_addr 	Register regAddr to write to
 * @param start_bit First bit position to write (0-15)
 * @param len 		Number of bits to write (not more than 16)
 * @param data 		Right-aligned value to write
 * @return Status of operation (0 = success, <0 = error)
 */
int8_t I2Cdev_writeBitsW(uint8_t dev_addr, uint8_t reg_addr, uint8_t start_bit,
		uint8_t len, uint16_t data)
{
    uint16_t w;
    int8_t err;

    if ((err = I2Cdev_readWord(dev_addr, reg_addr, &w)) != 0) {
        uint16_t mask = ((1 << len) - 1) << (start_bit - len + 1);
        data <<= (start_bit - len + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        w &= ~(mask); // zero all important bits in existing word
        w |= data; // combine data with existing word
        return I2Cdev_writeWord(dev_addr, reg_addr, w);
    }
    else {
        return err;
    }
}

