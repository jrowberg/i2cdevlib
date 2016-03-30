// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
// 11/28/2014 by Marton Sebok <sebokmarton@gmail.com>
// 24/06/2015 by Grégoire Surrel <gregoire.surrel@epfl.ch>
//
// Changelog:
//     2014-11-28 - ported to PIC18 peripheral library from Arduino code
//     2015-06-24 - ported to Jennic peripheral library from PIC18 code

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

#include "I2Cdev.h"
#include <jendefs.h>
#include <AppHardwareApi.h>
#include "os.h"
#include "os_gen.h"
#include "dbg.h"
#include "dbg_uart.h"

//#define I2CDEV_SERIAL_DEBUG

#define COMMAND_WRITE FALSE
#define COMMAND_READ TRUE

#define SEND_ACK E_AHI_SI_SEND_ACK
#define SEND_NACK E_AHI_SI_SEND_NACK
#define SEND_CONTINUE E_AHI_SI_NO_STOP_BIT
#define SEND_STOP E_AHI_SI_STOP_BIT

// Helper variables
bool_t I2Cdev_enabled = FALSE;//Tracks whether the I2C interface is enabled or not

bool_t debug = FALSE;
uint8 i2cdev_debug_depth = 0;

// Helper functions

/** Active wait .
 * @return Status of wait (timeout or succeed)
 */
bool_t IdleI2C() {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "IdleI2C\r\n");
#endif

	volatile uint32 i;
	bool_t check_status;
	for (i = 0; (check_status = bAHI_SiMasterPollTransferInProgress()) && (i < 1000); i++) {
		// NOP
	}


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif


	return check_status;
}

/** Start I2C
 */
void StartI2C() {
	if (I2Cdev_enabled)
		return;


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "StartI2C\r\n");
#endif

	// Pullups if needed
	//	vAHI_DioSetPullup(0, (1 << 14));
	//	vAHI_DioSetDirection((1 << 14), 0);
	//	vAHI_DioSetPullup(0, (1 << 15));
	//	vAHI_DioSetDirection((1 << 15), 0);
	vAHI_SiMasterConfigure(FALSE, FALSE, 7); //activate I2C with 7 = 400Khz, 31 = 100kHz clock,
	IdleI2C();
	I2Cdev_enabled = TRUE;


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

}

/** Stops I2C.
 */
void StopI2C() {
	if (!I2Cdev_enabled)
		return;


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "StopI2C\r\n");
#endif


	vAHI_SiMasterDisable();
	// Disable pullups
	//	vAHI_DioSetPullup(0, (1 << 14));
	//	vAHI_DioSetDirection((1 << 14), 0);
	//	vAHI_DioSetPullup(0, (1 << 15));
	//	vAHI_DioSetDirection((1 << 15), 0);
	I2Cdev_enabled = FALSE;


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

}

/** Write device address on the bus.
 * @param devAddr I2C slave device address
 * @param readCommand if last bit of addr (8th) is 1 or 0 (read or write)
 * @return Status of operation (0: OK, -1 error in Write, -5 error in Read, -2 no ACK for Write, -6 no ACK for Read)
 */
int8 WriteAddressI2C(uint8 devAddr, bool_t readCommand) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "WriteAddressI2C\r\n");
#endif

	bool_t check_status;
	if (!I2Cdev_enabled)
		StartI2C();

	//IdleI2C();

	vAHI_SiMasterWriteSlaveAddr(devAddr, readCommand);
	bAHI_SiMasterSetCmdReg(E_AHI_SI_START_BIT, E_AHI_SI_NO_STOP_BIT,
			E_AHI_SI_NO_SLAVE_READ, E_AHI_SI_SLAVE_WRITE, E_AHI_SI_SEND_ACK,
			E_AHI_SI_NO_IRQ_ACK);

	check_status = IdleI2C();

	if (check_status) {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -1 - 4*(readCommand == COMMAND_READ); // -1 if WRITE command, -5 if READ command
	}

	//check acknowledge receive
	if ((check_status = bAHI_SiMasterCheckRxNack()))
	{

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -2 - 4*(readCommand == COMMAND_READ); // -2 if WRITE command, -6 if READ command
	}


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return 0;
}

/** Write device register to read/write.
 * @param devAddr I2C slave device address
 * @return Status of operation (0: OK, -3 error in transmission, -5 no ACK received)
 */
int8 WriteRegisterI2C(uint8 regAddr) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "WriteRegisterI2C\r\n");
#endif

	bool_t check_status;
	if (!I2Cdev_enabled)
		StartI2C();

	vAHI_SiMasterWriteData8(regAddr);
	bAHI_SiMasterSetCmdReg(E_AHI_SI_NO_START_BIT, E_AHI_SI_NO_STOP_BIT,
			E_AHI_SI_NO_SLAVE_READ, E_AHI_SI_SLAVE_WRITE, E_AHI_SI_SEND_ACK,
			E_AHI_SI_NO_IRQ_ACK);

	check_status = IdleI2C();

	if (check_status) {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -3;
	}

	//check acknowledge receive
	if ((check_status = bAHI_SiMasterCheckRxNack())) {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -4;
	}


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return 0;
}

/** Read I2C bus.
 * @param sendAck if ACK is required
 * @param sendStop if STOP required
 * @return Result of reading, or negative error code (-7 error in first reading, -9 error in final reading, -8 no ACK for first reading, -10 no ACK for final reading)
 */
uint8 ReadI2C(bool_t sendAck, bool_t sendStop) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "ReadI2C\r\n");
#endif

	bool_t check_status;
	if (!I2Cdev_enabled)  {
		i2cdev_debug_depth--;
		return -15;
	}

	bAHI_SiMasterSetCmdReg(E_AHI_SI_NO_START_BIT, sendStop,
			E_AHI_SI_SLAVE_READ, E_AHI_SI_NO_SLAVE_WRITE, sendAck,
			E_AHI_SI_NO_IRQ_ACK);

	check_status = IdleI2C();

	if (check_status) {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -7 - 2*(sendStop == SEND_STOP); // Return -7, or -9 if final reading
	}

	// Check acknowledge
	if ((check_status = bAHI_SiMasterCheckRxNack())) {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -8 - 2*(sendStop == SEND_STOP); // Return -8, or -10 if final reading
	}

	uint8 buf = u8AHI_SiMasterReadData8();

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return buf;
}


/** Write I2C bus.
 * @param data data to send
 * @param sendStop if STOP required
 * @return Result of reading, or negative error code (-7 error in first writing, -9 error in final writing, -8 no ACK for first writing, -10 no ACK for final writing)
 */
uint8 WriteI2C(uint8 data, bool_t sendStop) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "WriteI2C\r\n");
#endif

	bool_t check_status;
	if (!I2Cdev_enabled) {
		i2cdev_debug_depth--;
		return -15;
	}

	vAHI_SiMasterWriteData8(data);
	bAHI_SiMasterSetCmdReg(E_AHI_SI_NO_START_BIT, sendStop,
			E_AHI_SI_NO_SLAVE_READ, E_AHI_SI_SLAVE_WRITE, E_AHI_SI_SEND_ACK,
			E_AHI_SI_NO_IRQ_ACK);

	check_status = IdleI2C();

	if (check_status) {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -11 - 2*(sendStop == SEND_STOP); // Return -7, or -9 if final reading
	}

	// Check acknowledge
	if ((check_status = bAHI_SiMasterCheckRxNack())) {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return -12 - 2*(sendStop == SEND_STOP); // Return -8, or -10 if final reading
	}


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return 0;
}



/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @return Number of bytes read (-1 indicates failure)
 */
int8 I2Cdev_readBytes(uint8 devAddr, uint8 regAddr, uint8 length, uint8 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readBytes\r\n");
#endif

	uint8 count = 0;

	// Start I2C
	StartI2C();

	// Device write address
	WriteAddressI2C(devAddr, COMMAND_WRITE);

	// Register address
	WriteRegisterI2C(regAddr);

	// Device read address
	WriteAddressI2C(devAddr, COMMAND_READ);

	for (count = 0; count < length; count++) {
		if (count == length - 1) {
			// NACK
			data[count] = ReadI2C(SEND_NACK, SEND_STOP);
		} else {
			// ACK
			data[count] = ReadI2C(SEND_ACK, SEND_CONTINUE);
		}
	}

	// Stop I2C
	StopI2C();


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return count;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @return Status of read operation (TRUE = success)
 */
int8 I2Cdev_readByte(uint8 devAddr, uint8 regAddr, uint8 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readByte\r\n");
#endif

	uint8 buf = I2Cdev_readBytes(devAddr, regAddr, 1, data);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return buf;
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @return Number of words read (-1 indicates failure)
 */
int8 I2Cdev_readWords(uint8 devAddr, uint8 regAddr, uint8 length, uint16 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readWords\r\n");
#endif

	uint8 count = 0;

	// Start I2C
	StartI2C();

	// Device write address
	WriteAddressI2C(devAddr, COMMAND_WRITE);

	// Register address
	WriteRegisterI2C(regAddr);

	// Device read address
	WriteAddressI2C(devAddr, COMMAND_READ);

	for (count = 0; count < length; count++) {
		// The PIC 18 doesn't send an ACK between bytes of words. Is it a bug?
		// I think so (https://github.com/jrowberg/i2cdevlib/issues/172), therefore
		// I stick to my implementation
		data[count] = ReadI2C(SEND_ACK, SEND_CONTINUE) << 8;

		if (count == length - 1) {
			// NACK for final LSB, to end transmission
			data[count] |= ReadI2C(SEND_NACK, SEND_STOP);
		} else {
			// ACK otherwise, to continue reading
			data[count] |= ReadI2C(SEND_ACK, SEND_CONTINUE);
			count++;
		}
	}

	// Stop I2C
	StopI2C();


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return count;
}

/** Read single word from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @return Status of read operation (TRUE = success)
 */
int8 I2Cdev_readWord(uint8 devAddr, uint8 regAddr, uint16 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readWord\r\n");
#endif

	uint8 buf = I2Cdev_readWords(devAddr, regAddr, 1, data);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return buf;
}

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @return Status of read operation (TRUE = success)
 */
int8 I2Cdev_readBit(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint8 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readBit\r\n");
#endif

	uint8 b;
	uint8 count = I2Cdev_readByte(devAddr, regAddr, &b);
	*data = b & (1 << bitNum);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return count;
}

/** Read a single bit from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-15)
 * @param data Container for single bit value
 * @return Status of read operation (TRUE = success)
 */
int8 I2Cdev_readBitW(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint16 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readBitW\r\n");
#endif

	uint16 b;
	uint8 count = I2Cdev_readWord(devAddr, regAddr, &b);
	*data = b & (1 << bitNum);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @return Status of read operation (TRUE = success)
 */
int8 I2Cdev_readBits(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint8 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readBits\r\n");
#endif

	// 01101001 read byte
	// 76543210 bit numbers
	//    xxx   args: bitStart=4, length=3
	//    010   masked
	//   -> 010 shifted
	uint8 count, b;
	if ((count = I2Cdev_readByte(devAddr, regAddr, &b)) != 0) {
		uint8 mask = ((1 << length) - 1) << (bitStart - length + 1);
		b &= mask;
		b >>= (bitStart - length + 1);
		*data = b;
	}

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return count;
}

/** Read multiple bits from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-15)
 * @param length Number of bits to read (not more than 16)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @return Status of read operation (1 = success, 0 = failure, -1 = timeout)
 */
int8 I2Cdev_readBitsW(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint16 *data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "readBitsW\r\n");
#endif

	// 1101011001101001 read byte
	// fedcba9876543210 bit numbers
	//    xxx           args: bitStart=12, length=3
	//    010           masked
	//           -> 010 shifted
	uint8 count;
	uint16 w;
	if ((count = I2Cdev_readWord(devAddr, regAddr, &w)) != 0) {
		uint16 mask = ((1 << length) - 1) << (bitStart - length + 1);
		w &= mask;
		w >>= (bitStart - length + 1);
		*data = w;
	}

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return count;
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeBytes(uint8 devAddr, uint8 regAddr, uint8 length, uint8* data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeBytes\r\n");
#endif

	uint8 count;

	// Start I2C
	StartI2C();

	// Device write address
	WriteAddressI2C(devAddr, COMMAND_WRITE);

	// Register address
	WriteRegisterI2C(regAddr);

	for (count = 0; count < length; count++) {
		if (count == length - 1) {
			// Stop
			WriteI2C(data[count], SEND_STOP);
		} else {
			// continue
			WriteI2C(data[count], SEND_CONTINUE);
		}
	}

	// Stop I2C
	StopI2C();


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return TRUE;
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeByte(uint8 devAddr, uint8 regAddr, uint8 data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeByte\r\n");
#endif

	uint8 buf = I2Cdev_writeBytes(devAddr, regAddr, 1, &data);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return buf;
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeWords(uint8 devAddr, uint8 regAddr, uint8 length, uint16* data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeWords\r\n");
#endif

	uint8 count;

	// Start I2C
	StartI2C();

	// Device write address
	WriteAddressI2C(devAddr, COMMAND_WRITE);

	// Register address
	WriteRegisterI2C(regAddr);

	for (count = 0; count < length; count++) {
		// Send MSB
		IdleI2C();
		WriteI2C(data[count] >> 8, SEND_CONTINUE);

		// Send LSB
		if (count == length - 1) {
			// Stop
			WriteI2C(data[count] & 0xFF, SEND_STOP);
		} else {
			// continue
			WriteI2C(data[count] & 0xFF, SEND_CONTINUE);
		}
	}

	// Stop I2C
	StopI2C();


#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return TRUE;
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeWord(uint8 devAddr, uint8 regAddr, uint16 data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeWord\r\n");
#endif

	uint8 buf = I2Cdev_writeWords(devAddr, regAddr, 1, &data);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return buf;
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeBit(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint8 data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeBit\r\n");
#endif

	uint8 b;
	I2Cdev_readByte(devAddr, regAddr, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	uint8 buf = I2Cdev_writeByte(devAddr, regAddr, b);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return buf;
}

/** write a single bit in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-15)
 * @param value New bit value to write
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeBitW(uint8 devAddr, uint8 regAddr, uint8 bitNum, uint16 data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeBitW\r\n");
#endif

	uint16 w;
	I2Cdev_readWord(devAddr, regAddr, &w);
	w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
	uint8 buf = I2Cdev_writeWord(devAddr, regAddr, w);

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth--;
#endif

	return buf;
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeBits(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint8 data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeBits\r\n");
#endif

	//      010 value to write
	// 76543210 bit numbers
	//    xxx   args: bitStart=4, length=3
	// 00011100 mask byte
	// 10101111 original value (sample)
	// 10100011 original & ~mask
	// 10101011 masked | value
	uint8 b;
	if (I2Cdev_readByte(devAddr, regAddr, &b) != 0) {
		uint8 mask = ((1 << length) - 1) << (bitStart - length + 1);
		data <<= (bitStart - length + 1); // shift data into correct position
		data &= mask; // zero all non-important bits in data
		b &= ~(mask); // zero all important bits in existing byte
		b |= data; // combine data with existing byte
		uint8 buf = I2Cdev_writeByte(devAddr, regAddr, b);

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return buf;
	} else {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return FALSE;
	}
}

/** Write multiple bits in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-15)
 * @param length Number of bits to write (not more than 16)
 * @param data Right-aligned value to write
 * @return Status of operation (TRUE = success)
 */
uint8 I2Cdev_writeBitsW(uint8 devAddr, uint8 regAddr, uint8 bitStart, uint8 length, uint16 data) {

#ifdef I2CDEV_SERIAL_DEBUG
	i2cdev_debug_depth++; int z; for(z=0;z<i2cdev_debug_depth;z++) DBG_vPrintf(TRACE_APP, "\t"); DBG_vPrintf(TRACE_APP, "writeBytesW\r\n");
#endif

	//              010 value to write
	// fedcba9876543210 bit numbers
	//    xxx           args: bitStart=12, length=3
	// 0001110000000000 mask word
	// 1010111110010110 original value (sample)
	// 1010001110010110 original & ~mask
	// 1010101110010110 masked | value
	uint16 w;
	if (I2Cdev_readWord(devAddr, regAddr, &w) != 0) {
		uint16 mask = ((1 << length) - 1) << (bitStart - length + 1);
		data <<= (bitStart - length + 1); // shift data into correct position
		data &= mask; // zero all non-important bits in data
		w &= ~(mask); // zero all important bits in existing word
		w |= data; // combine data with existing word
		uint8 buf = I2Cdev_writeWord(devAddr, regAddr, w);

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return buf;
	} else {

#ifdef I2CDEV_SERIAL_DEBUG
		i2cdev_debug_depth--;
#endif

		return FALSE;
	}
}
