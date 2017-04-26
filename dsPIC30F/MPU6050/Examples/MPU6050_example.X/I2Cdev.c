// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// 6/9/2012 by Jeff Rowberg <jeff@rowberg.net>
// 11/28/2014 by Marton Sebok <sebokmarton@gmail.com>
//
// Changelog:
//     2014-11-28 - ported to PIC18 peripheral library from Arduino code

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2013 Jeff Rowberg
Copyright (c) 2014 Marton Sebok
Copyright (c) 2017 Daichou
 *
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


/*uint16_t config2;
uint16_t config1 = (I2C_ON & I2C_IDLE_CON & I2C_CLK_HLD &
             I2C_IPMI_DIS & I2C_7BIT_ADD &
             I2C_SLW_DIS & I2C_SM_DIS &
             I2C_GCALL_DIS & I2C_STR_DIS &
             I2C_NACK & I2C_ACK_DIS & I2C_RCV_DIS &
             I2C_STOP_DIS & I2C_RESTART_DIS &
             I2C_START_DIS);
*/
/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @return Number of bytes read (-1 indicates failure)
 */
int8_t I2Cdev_readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
    //int8_t count = 0;
    IFS0bits.MI2CIF = 0;
    IEC0bits.MI2CIE = 0;
    //IdleI2C();
    /*master Start*/
    StartI2C();
    /* Clear interrupt flag */

    /* Wait till Start sequence is completed */
    while(I2CCONbits.SEN);

    /*master send AD+W*/
    /* Write Slave Address (Write)*/
    MasterWriteI2C(devAddr << 1 | 0x00);

    /* Wait until address is transmitted */
    while(I2CSTATbits.TBF);  // 8 clock cycles

    /*Slave send Ack*/
    while(!IFS0bits.MI2CIF); // Wait for 9th clock cycle
    IFS0bits.MI2CIF = 0;     // Clear interrupt flag
    //while(I2CSTATbits.ACKSTAT);
    /*Master send RA*/
    /* Write Register Address */
    MasterWriteI2C(regAddr);

    /* Wait until address is transmitted */
    while(I2CSTATbits.TBF);  // 8 clock cycles

    /*Slave send ACK*/
    while(!IFS0bits.MI2CIF); // Wait for 9th clock cycle
    IFS0bits.MI2CIF = 0;     // Clear interrupt flag
    while(I2CSTATbits.ACKSTAT);

    /*Master Pause*/
    StopI2C();
    /* Wait till stop sequence is completed */
    while(I2CCONbits.PEN);

    /*Master Start*/
    StartI2C();
    /* Wait till Start sequence is completed */
    while(I2CCONbits.SEN);

    /*Master send AD+R*/
    /* Write Slave Address (Read)*/
    MasterWriteI2C(devAddr << 1 | 0x01);
    /* Wait until address is transmitted */
    while(I2CSTATbits.TBF);  // 8 clock cycles

    /*Slave send Ack*/
    while(!IFS0bits.MI2CIF); // Wait for 9th clock cycle
    IFS0bits.MI2CIF = 0;     // Clear interrupt flag
    while(I2CSTATbits.ACKSTAT);

    /*Slave send DATA*/
    //uint16_t flag = MastergetsI2C(length,data,I2C_DATA_WAIT);

    /*Slave send NACK*/
    //MastergetsI2C(length,data,100);
    //NotAckI2C();

    data[0] = MasterReadI2C();
    unsigned int i;
    for (i = 1 ; i < length ; i++ ){
        AckI2C();
        while(I2CCONbits.ACKEN == 1);
        data[i] = MasterReadI2C();
    }
    NotAckI2C();
    while(I2CCONbits.ACKEN == 1);
    /*Master Pause*/
    StopI2C();
    /* Wait till stop sequence is completed */
    while(I2CCONbits.PEN);
    //CloseI2C();
    //IdleI2C();
    return length;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
    return I2Cdev_readBytes(devAddr, regAddr, 1, data);
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @return Number of words read (-1 indicates failure)
 */
int8_t I2Cdev_readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data) {
    unsigned char Onebyte[100];
    I2Cdev_readBytes(devAddr,regAddr,length*2,Onebyte);
    unsigned int i;
    for (int i = 0 ; i < length ; i++ ){
        data[i] = Onebyte[i*2] << 8 | Onebyte[i*2+1];
    }
    return length;
}

/** Read single word from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data) {
    return I2Cdev_readWords(devAddr, regAddr, 1, data);
}

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
    uint8_t b;
    uint8_t count = I2Cdev_readByte(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read a single bit from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-15)
 * @param data Container for single bit value
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data) {
    uint16_t b;
    uint8_t count = I2Cdev_readWord(devAddr, regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = I2Cdev_readByte(devAddr, regAddr, &b)) != 0) {
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
 * @return Status of read operation (1 = success, 0 = failure, -1 = timeout)
 */
int8_t I2Cdev_readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data) {
    // 1101011001101001 read byte
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    //    010           masked
    //           -> 010 shifted
    uint8_t count;
    uint16_t w;
    if ((count = I2Cdev_readWord(devAddr, regAddr, &w)) != 0) {
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        w &= mask;
        w >>= (bitStart - length + 1);
        *data = w;
    }
    return count;
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    //if (data[length-1] != '\0' && length != 1)return false;
	//OpenI2C(config1,config2);
    IFS0bits.MI2CIF = 0;
    IEC0bits.MI2CIE = 0;
    //IdleI2C();
    /*Master Start*/
    StartI2C();
    /* Wait util Start sequence is completed */
    while(I2CCONbits.SEN);
    /* Clear interrupt flag */
    IFS0bits.MI2CIF = 0;

    /*Master send AD+W*/
    /* Write Slave address (Write)*/
    MasterWriteI2C(devAddr << 1 | 0x00);
    /* Wait till address is transmitted */
    while(I2CSTATbits.TBF);  // 8 clock cycles

    /*Slave send ACK*/
    while(!IFS0bits.MI2CIF); // Wait for 9th clock cycle
    IFS0bits.MI2CIF = 0;     // Clear interrupt flag
    while(I2CSTATbits.ACKSTAT);

    /*Master send RA*/
    /* Write Slave address (Write)*/
    MasterWriteI2C(regAddr);
    /* Wait till address is transmitted */
    while(I2CSTATbits.TBF);  // 8 clock cycles

    /*Slave send ACK*/
    while(!IFS0bits.MI2CIF); // Wait for 9th clock cycle
    IFS0bits.MI2CIF = 0;     // Clear interrupt flag
    while(I2CSTATbits.ACKSTAT);

    /*Master send data*/
    /* Transmit string of data */
    //MasterputsI2C(data);
    uint8_t i;
    for (i = 0 ; i < length ; i++){
        MasterWriteI2C(data[i]);
        /* Wait till address is transmitted */
        while(I2CSTATbits.TBF);  // 8 clock cycles

        /*Slave send ACK*/
        while(!IFS0bits.MI2CIF); // Wait for 9th clock cycle
        IFS0bits.MI2CIF = 0;     // Clear interrupt flag
    }
    //while(!IFS0bits.MI2CIF); // Wait for 9th clock cycle
    //IFS0bits.MI2CIF = 0;     // Clear interrupt flag
    StopI2C();
    /* Wait till stop sequence is completed */
    while(I2CCONbits.PEN);
    //CloseI2C();
    //IdleI2C();
    return true;
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
    return I2Cdev_writeBytes(devAddr, regAddr, 1, &data);
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) {
    unsigned char OneByte[100];
    unsigned int i;
    for (i = 0 ; i < length ; i++){
        OneByte[i*2] = data[i]>>8;
        OneByte[i*2+1] = data[i] & 0XFF;
    }
    I2Cdev_writeBytes(devAddr,regAddr,length*2,OneByte);
    return true;
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
    return I2Cdev_writeWords(devAddr, regAddr, 1, &data);
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
    uint8_t b;
    I2Cdev_readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return I2Cdev_writeByte(devAddr, regAddr, b);
}

/** write a single bit in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-15)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data) {
    uint16_t w;
    I2Cdev_readWord(devAddr, regAddr, &w);
    w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
    return I2Cdev_writeWord(devAddr, regAddr, w);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (I2Cdev_readByte(devAddr, regAddr, &b) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
		return I2Cdev_writeByte(devAddr, regAddr, b);
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
bool I2Cdev_writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data) {
    //              010 value to write
    // fedcba9876543210 bit numbers
    //    xxx           args: bitStart=12, length=3
    // 0001110000000000 mask word
    // 1010111110010110 original value (sample)
    // 1010001110010110 original & ~mask
    // 1010101110010110 masked | value
    uint16_t w;
    if (I2Cdev_readWord(devAddr, regAddr, &w) != 0) {
        uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        w &= ~(mask); // zero all important bits in existing word
        w |= data; // combine data with existing word
        return I2Cdev_writeWord(devAddr, regAddr, w);
    } else {
        return false;
    }
}
