// I2Cdev library collection - Main I2C device class header file
// Abstracts bit and byte I2C R/W functions into a convenient class
// LPC17xx stub port by Tommy Savaria <uxtimezone@gmail.com>
// Based on Arduino's I2Cdev by Jeff Rowberg <jeff@rowberg.net>
//
// 2015-08-18 by Tommy Savaria <uxtimezone@gmail.com>
//
// Changelog:
//		2015-08-18 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2015 Jeff Rowberg
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
#include "LPC17xx.h"

//Set I2C bus to 100KHz
#define I2C_BUS_SPEED 100000

//CONSET & CONCLR Register bits
#define I2C_CONTROL_AA 0x04
#define I2C_CONTROL_SI 0x08
#define I2C_CONTROL_STOP 0x10
#define I2C_CONTROL_START 0x20
#define I2C_CONTROL_I2EN 0x40

#define I2CControl_Enable() LPC_I2C0->I2CONSET = I2C_CONTROL_I2EN
#define I2CControl_Start() LPC_I2C0->I2CONSET = I2C_CONTROL_START
#define I2CControl_Stop() LPC_I2C0->I2CONSET = I2C_CONTROL_STOP
#define I2CControl_SetAcknowledge() LPC_I2C0->I2CONSET = I2C_CONTROL_AA

#define I2CControl_ClearAll() LPC_I2C0->I2CONCLR = I2C_CONTROL_AA | I2C_CONTROL_SI | I2C_CONTROL_START
#define I2CControl_Disable() LPC_I2C0->I2CONCLR = I2C_CONTROL_I2EN
#define I2CControl_ClearStart() LPC_I2C0->I2CONCLR = I2C_CONTROL_START
#define I2CControl_ClearInterrupt() LPC_I2C0->I2CONCLR = I2C_CONTROL_SI
#define I2CControl_ClearAcknowledge() LPC_I2C0->I2CONCLR = I2C_CONTROL_AA

#define I2CControl_WaitInterrupt() while(!(LPC_I2C0->I2CONSET & I2C_CONTROL_SI))
#define I2CControl_WaitStop() while(LPC_I2C0->I2CONSET & I2C_CONTROL_STOP)


/** Default constructor.
 */
I2Cdev::I2Cdev() {
}

/** Initialize I2C bus
 *  I2CDev uses peripheral I2C0 on the LPC17xx
 */
void I2Cdev::initialize()
{
	static bool init = false;

	if(init)
		return;

	//Power on I2C0 peripheral
	LPC_SC->PCONP |= (1 << 7);

	//Set I2C0 Peripheral Clock to /4 mode
	LPC_SC->PCLKSEL0 &= ~(3 << 14);

	//Set I2C0 pins to I2C Mode
	LPC_PINCON->PINSEL1 &= ~(0xF << 22); // Clear bits
	LPC_PINCON->PINSEL1 |= (1 << 22); //SDA (P0.27)
	LPC_PINCON->PINSEL1 |= (1 << 24); //SCL (P0.28)

	//Set I2C Clock
	unsigned int clockDivider = SystemCoreClock / (4 * 2 * I2C_BUS_SPEED);
	LPC_I2C0->I2SCLL = clockDivider;
	LPC_I2C0->I2SCLH = clockDivider;

	I2CControl_ClearAll();
	I2CControl_Enable();

	init = true;
}

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

/** Read a single bit from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-15)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout) {
    uint16_t b;
    uint8_t count = readWord(devAddr, regAddr, &b, timeout);
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

/** Read multiple bits from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-15)
 * @param length Number of bits to read (not more than 16)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (1 = success, 0 = failure, -1 = timeout)
 */
int8_t I2Cdev::readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout) {
    // 1101011001101001 read byte
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    //    010           masked
    //           -> 010 shifted
    uint8_t count;
    uint16_t w;
    if ((count = readWord(devAddr, regAddr, &w, timeout)) != 0) {
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        w &= mask;
        w >>= (bitStart - length + 1);
        *data = w;
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

/** Read single word from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout) {
    return readWords(devAddr, regAddr, 1, data, timeout);
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
int8_t I2Cdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout) {

	//int8_t count = 0;
	//uint32_t t1 = millis();

	return I2C::read(devAddr, regAddr, length, data);
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of words read (-1 indicates failure)
 */
int8_t I2Cdev::readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout) {

	return I2C::read(devAddr, regAddr, length * 2, (uint8_t*)data);
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

/** write a single bit in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-15)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data) {
    uint16_t w;
    readWord(devAddr, regAddr, &w);
    w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
    return writeWord(devAddr, regAddr, w);
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

/** Write multiple bits in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-15)
 * @param length Number of bits to write (not more than 16)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data) {
    //              010 value to write
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    // 0001110000000000 mask word
    // 1010111110010110 original value (sample)
    // 1010001110010110 original & ~mask
    // 1010101110010110 masked | value
    uint16_t w;
    if (readWord(devAddr, regAddr, &w) != 0) {
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        w &= ~(mask); // zero all important bits in existing word
        w |= data; // combine data with existing word
        return writeWord(devAddr, regAddr, w);
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
    return writeBytes(devAddr, regAddr, 1, &data);
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
    return writeWords(devAddr, regAddr, 1, &data);
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {

	return I2C::write(devAddr, regAddr, length, data) != -1;
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) {

	return I2C::write(devAddr, regAddr, length*2, (uint8_t*)data) != -1;
}

/** Default timeout value for read operations.
 * Set this to 0 to disable timeout detection.
 */
uint16_t I2Cdev::readTimeout = I2CDEV_DEFAULT_READ_TIMEOUT;

//Read and Write functions
int8_t I2C::read(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data)
{
	int status;

	//Send Start bit
	status = I2C::start();

	if ((status != 0x10) && (status != 0x08))
		return -1; //I2C bus was busy (should not happen)

	//Write Device Address
	status = I2C::sendByte(devAddr);
	if(status != 0x40)
			return -1; //Slave with this address was not found

	//Write register address
	I2C::sendByte(regAddr);

	//Send new start bit
	status = I2C::start();
	if ((status != 0x10) && (status != 0x08))
			return -1; //I2C bus was busy (should not happen)

	//Write Device Address
	status = I2C::sendByte((devAddr << 1) + 1);
	if(status != 0x40)
		return -1; //Slave with this address was not found

	//Read all bytes
	for(int i = 0; i < length; i++)
		data[i] = I2C::readByte(i < (length - 1));

	//Send stop
	I2C::stop();

	return length;
}

int8_t I2C::write(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data)
{
	int status;

	//Send Start bit
	status = I2C::start();

	if ((status != 0x10) && (status != 0x08))
		return -1; //I2C bus was busy (should not happen)

	//Send slave address
	status = I2C::sendByte(devAddr << 1);
	if(status != 0x18)
		return -1; //Slave with this address was not found

	//Write register address
	I2C::sendByte(regAddr);

	//Write data
	for(int i = 0; i < length; i++)
		I2C::sendByte(data[i]);

	//Send stop
	I2C::stop();

	return length;
}

unsigned char I2C::readByte(bool ack)
{
	if(ack)
		I2CControl_SetAcknowledge();
	else
		I2CControl_ClearAcknowledge();

	I2CControl_ClearInterrupt();

	//Wait for interrupt signaling that byte was received
	I2CControl_WaitInterrupt();

	//Read byte
	return LPC_I2C0->I2DAT;
}

int I2C::sendByte(uint8_t byte)
{
	//Write data to be sent
	LPC_I2C0->I2DAT = byte;

	//Start transmitting byte by clearing interrupt
	I2CControl_ClearInterrupt();

	//Wait for interrupt signaling that byte was sent
	I2CControl_WaitInterrupt();

	//Return status
	return LPC_I2C0->I2STAT;

}

int I2C::start()
{
	int status;

	I2CControl_ClearAll();

	I2CControl_SetAcknowledge();

	I2CControl_Start();

	//Wait for interrupt signaling that start bit was sent
	I2CControl_WaitInterrupt();

	//Wait interrupt
	status = LPC_I2C0->I2STAT;

	//Clear start bit
	I2CControl_ClearStart();

	//Read status
	return status;
}

int I2C::stop()
{
	//Send Stop bit
	I2CControl_Stop();

	I2CControl_ClearInterrupt();

	//Wait for Stop flag to be cleared, indicating stop bit was sent
	I2CControl_WaitStop();

	//Read status
	return LPC_I2C0->I2STAT;
}
