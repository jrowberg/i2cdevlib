// I2Cdev library collection - L3G4200D I2C device class
// Based on STMicroelectronics L3G4200D datasheet rev. 3, 12/2010
// 7/31/2013 by Jonathan Arnett <j3rn@j3rn.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2013-07-31 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jonathan Arnett, Jeff Rowberg

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

#include "L3G4200D.h"

/** Default constructor, uses default I2C address.
 * @see L3G4200D_DEFAULT_ADDRESS
 */
L3G4200D::L3G4200D() {
    devAddr = L3G4200D_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see L3G4200D_DEFAULT_ADDRESS
 * @see L3G4200D_ADDRESS
 */
L3G4200D::L3G4200D(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * All values are defaults except for the power on bit in CTRL_REG_1
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_RA_CTRL_REG2
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_RA_CTRL_REG5
 */
void L3G4200D::initialize() {
	I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG1, 0b00001111);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG2, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG3, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG4, 0b00000000);
    I2Cdev::writeByte(devAddr, L3G4200D_RA_CTRL_REG5, 0b00000000);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool L3G4200D::testConnection() {
    return getDeviceID() == 0b11010011;
}

// WHO_AM_I register, read-only

/** Get the Device ID.
 * The WHO_AM_I register holds the device's id
 * @return Device ID (should be 0b11010011, 109, 0x69)
 * @see L3G4200D_RA_WHO_AM_I
 */
uint8_t L3G4200D::getDeviceID() {
    I2Cdev::readByte(devAddr, L3G4200D_RA_WHO_AM_I, buffer);
    return buffer[0];
}

// CTRL_REG1 register, r/w

/** Set the output data rate
 * @param rate The new data output rate (can be 100, 200, 400, or 800)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 */
void L3G4200D::setOutputDataRate(uint16_t rate) {
	uint8_t writeVal;
	
	if (rate == 100) {
		writeVal = L3G4200D_RATE_100;
	} else if (rate == 200) {
		writeVal = L3G4200D_RATE_200;
	} else if (rate == 400) {
		writeVal = L3G4200D_RATE_400;
	} else {
		writeVal = L3G4200D_RATE_800;
	}
	
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ODR_BIT,
		L3G4200D_ODR_LENGTH, writeVal); 
}

/** Get the current output data rate
 * @return Current data output rate
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 */
uint16_t L3G4200D::getOutputDataRate() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ODR_BIT, 
		L3G4200D_ODR_LENGTH, buffer);
	uint8_t rate = buffer[0];

	if (rate == L3G4200D_RATE_100) {
		return 100;
	} else if (rate == L3G4200D_RATE_200) {
		return 200;
	} else if (rate == L3G4200D_RATE_400) {
		return 400;
	}
	return 800;
}

/** Set the bandwidth cut-off mode
 * @param mode The new bandwidth cut-off mode
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_BW_BIT
 * @see L3G4200D_BW_LENGTH
 * @see L3G4200D_BW_LOW
 * @see L3G4200D_BW_MED_LOW
 * @see L3G4200D_BW_MED_HIGH
 * @see L3G4200D_BW_HIGH
 */
void L3G4200D::setBandwidthCutOffMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_BIT, 
		L3G4200D_BW_LENGTH, mode);
}

/** Get the current bandwidth cut-off mode
 * @return Current bandwidth cut off mode
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_BW_BIT
 * @see L3G4200D_BW_LENGTH
 * @see L3G4200D_BW_LOW
 * @see L3G4200D_BW_MED_LOW
 * @see L3G4200D_BW_MED_HIGH
 * @see L3G4200D_BW_HIGH
 */
uint8_t L3G4200D::getBandwidthCutOffMode() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_BIT, 
		L3G4200D_BW_LENGTH, buffer);
	return buffer[0];
}

/** Gets the current bandwidth cutoff based on ODR and BW
 * @return Float value of the bandwidth cut off
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 * @see L3G4200D_BW_BIT
 * @see L3G4200D_BW_LENGTH
 * @see L3G4200D_BW_LOW
 * @see L3G4200D_BW_MED_LOW
 * @see L3G4200D_BW_MED_HIGH
 * @see L3G4200D_BW_HIGH
 */
float L3G4200D::getBandwidthCutOff() {
	uint16_t dataRate = getOutputDataRate();
	uint8_t bandwidthMode = getBandwidthCutOffMode();

	if (dataRate == 100) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 12.5;
		} else {
			return 25.0;
		}
	} else if (dataRate == 200) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 12.5;
		} else if (bandwidthMode == L3G4200D_BW_MED_LOW) {
			return 25.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_HIGH) {
			return 50.0;
		} else {
			return 70.0;
		}
	} else if (dataRate == 400) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 20.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_LOW) {
			return 25.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_HIGH) {
			return 50.0;
		} else {
			return 110.0;
		}
	} else {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 30.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_LOW) {
			return 35.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_HIGH) {
			return 50.0;
		} else {
			return 110.0;
		}
	}
}

/** Set power on or off
 * @param enabled The new power setting (true for on, false for off)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_PD_BIT
 */
void L3G4200D::setPowerOn(bool on) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_PD_BIT, on);
}

/** Get the current power state
 * @return Powered on state (true for on, false for off)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_PD_BIT
 */
bool L3G4200D::getPowerOn() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_PD_BIT, buffer);
	return buffer[0];
}

/** Enables or disables the ability to get Z data
 * @param enabled The new enabled state of the Z axis
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ZEN_BIT
 */
void L3G4200D::setZEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ZEN_BIT, enabled);
}

/** Get whether Z axis data is enabled
 * @return True if the Z axis is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ZEN_BIT
 */
bool L3G4200D::getZEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ZEN_BIT, buffer);
	return buffer[0];
}

/** Enables or disables the ability to get Y data
 * @param enabled The new enabled state of the Y axis
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_YEN_BIT
 */
void L3G4200D::setYEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_YEN_BIT, enabled);
}

/** Get whether Y axis data is enabled
 * @return True if the Y axis is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_YEN_BIT
 */
bool L3G4200D::getYEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_YEN_BIT, buffer);
	return buffer[0];
}

/** Enables or disables the ability to get X data
 * @param enabled The new enabled state of the X axis
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_XEN_BIT
 */
void L3G4200D::setXEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_XEN_BIT, enabled);
}

/** Get whether X axis data is enabled
 * @return True if the X axis is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_XEN_BIT
 */
bool L3G4200D::getXEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_XEN_BIT, buffer);
	return buffer[0];
}

// CTRL_REG2 register, r/w

/** Set the high pass mode
 * @param mode The new high pass mode
 * @see L3G4200D_RA_CTRL_REG2
 * @see L3G4200D_HPM_BIT
 * @see L3G4200D_HPM_LENGTH
 * @see L3G4200D_HPM_HRF
 * @see L3G4200D_HPM_REFERENCE
 * @see L3G4200D_HPM_NORMAL
 * @see L3G4200D_HPM_AUTORESET
 */
void L3G4200D::setHighPassMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG2, L3G4200D_HPM_BIT, 
		L3G4200D_HPM_LENGTH, mode);
}

/** Get the high pass mode
 * @return High pass mode
 * @see L3G4200D_RA_CTRL_REG2
 * @see L3G4200D_HPM_BIT
 * @see L3G4200D_HPM_LENGTH
 * @see L3G4200D_HPM_HRF
 * @see L3G4200D_HPM_REFERENCE
 * @see L3G4200D_HPM_NORMAL
 * @see L3G4200D_HPM_AUTORESET
 */
uint8_t L3G4200D::getHighPassMode() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG2, L3G4200D_HPM_BIT, 
		L3G4200D_HPM_LENGTH, buffer);
	return buffer[0];
}

/** Set the high pass filter cut off frequency level (1 - 10)
 * @param level The new level for the hpcf, using one of the defined levels
 * @see L3G4200D_RA_CTRL_REG2
 * @see L3G4200D_HPCF_BIT
 * @see L3G4200D_HPCF_LENGTH
 * @see L3G4200D_HPCF1
 * @see L3G4200D_HPCF2
 * @see L3G4200D_HPCF3
 * @see L3G4200D_HPCF4
 * @see L3G4200D_HPCF5
 * @see L3G4200D_HPCF6
 * @see L3G4200D_HPCF7
 * @see L3G4200D_HPCF8
 * @see L3G4200D_HPCF9
 * @see L3G4200D_HPCF10
 */
void L3G4200D::setHighPassFilterCutOffFrequencyLevel(uint8_t level) {
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG2, L3G4200D_HPCF_BIT, 
		L3G4200D_HPCF_LENGTH, level);
}

/** Get the high pass filter cut off frequency level (1 - 10)
 * @return High pass filter cut off frequency level
 * @see L3G4200D_RA_CTRL_REG2
 * @see L3G4200D_HPCF_BIT
 * @see L3G4200D_HPCF_LENGTH
 * @see L3G4200D_HPCF1
 * @see L3G4200D_HPCF2
 * @see L3G4200D_HPCF3
 * @see L3G4200D_HPCF4
 * @see L3G4200D_HPCF5
 * @see L3G4200D_HPCF6
 * @see L3G4200D_HPCF7
 * @see L3G4200D_HPCF8
 * @see L3G4200D_HPCF9
 * @see L3G4200D_HPCF10
 */
uint8_t L3G4200D::getHighPassFilterCutOffFrequencyLevel() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG2, L3G4200D_HPCF_BIT, 
		L3G4200D_HPCF_LENGTH, buffer);
	return buffer[0];
}

// CTRL_REG3 register, r/w

/** Set the INT1 interrupt enabled state
 * @param enabled New enabled state for the INT1 interrupt
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_INT1_BIT
 */
void L3G4200D::setINT1InterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_INT1_BIT, 
		enabled);
}

/** Get the INT1 interrupt enabled state
 * @return True if the INT1 interrupt is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_INT1_BIT
 */
bool L3G4200D::getINT1InterruptEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_INT1_BIT, 
		buffer);
	return buffer[0];
}

/** Set the INT1 boot status enabled state
 * @param enabled New enabled state for the INT1 boot status
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_BOOT_BIT
 */
void L3G4200D::setINT1BootStatusEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_BOOT_BIT, 
		enabled);
}

/** Get the INT1 boot status enabled state
 * @return INT1 boot status status
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_BOOT_BIT
 */
bool L3G4200D::getINT1BootStatusEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_BOOT_BIT, 
		buffer);
	return buffer[0];
}

/** Interrupts the active INT1 configuration
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_H_LACTIVE_BIT
 */
void L3G4200D::interruptActiveINT1Config() {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_H_LACTIVE_BIT, 1);
}

/** Set output mode to push-pull or open-drain
 * @param mode New output mode
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_PP_OD_BIT
 * @see L3G4200D_PUSH_PULL
 * @see L3G4200D_OPEN_DRAIN
 */
void L3G4200D::setOutputMode(bool mode) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_PP_OD_BIT, 
		mode);
}

/** Get whether mode is push-pull or open drain
 * @return Output mode (TRUE for push-pull, FALSE for open-drain)
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_PP_OD_BIT
 * @see L3G4200D_PUSH_PULL
 * @see L3G4200D_OPEN_DRAIN
 */
bool L3G4200D::getOutputMode() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_PP_OD_BIT, 
		buffer);
	return buffer[0];
}

/** Set data ready interrupt enabled state on INT2 pin
 * @param enabled New INT2 data ready interrupt enabled state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_DRDY_BIT
 */
void L3G4200D::setINT2DataReadyEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_DRDY_BIT, 
		enabled);
}

/** Get whether the data ready interrupt is enabled on the INT2 pin
 * @return True if the INT2 data ready interrupt is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_DRDY_BIT
 */
bool L3G4200D::getINT2DataReadyEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_DRDY_BIT, 
		buffer);
	return buffer[0];
}

/** Set whether the INT2 FIFO watermark interrupt is enabled
 * The sensor contains a 32-slot FIFO buffer for storing data so that it may be 
 * read later. If enabled, the sensor will generate an interrupt on the 
 * INT2/DRDY pin when the watermark has been reached. The watermark can be 
 * configured through the setFIFOWatermark function.
 * @param enabled New enabled state of the INT2 FIFO watermark
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_WTM_BIT
 */
void L3G4200D::setINT2FIFOWatermarkInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_WTM_BIT, 
		enabled);
}

/** Get the INT2 FIFO watermark interrupt enabled state
 * @return true if the FIFO watermark is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_WTM_BIT
 */ 
bool L3G4200D::getINT2FIFOWatermarkInterruptEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_WTM_BIT, 
		buffer);
	return buffer[0];
}

/** Set whether an interrupt is triggered on INT2 when the FIFO is overrun
 * @param enabled New FIFO overrun interrupt enabled state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_ORUN_BIT
 */
void L3G4200D::setINT2FIFOOverrunInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_ORUN_BIT, 
		enabled);
}

/** Get whether an interrupt is triggered on INT2 when the FIFO is overrun
 * @return True if the INT2 FIFO overrun interrupt is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_ORUN_BIT
 */
bool L3G4200D::getINT2FIFOOverrunInterruptEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_ORUN_BIT,
		buffer);
	return buffer[0];
}

/** Set whether an interrupt is triggered on INT2 when the FIFO buffer is empty
 * @param enabled New INT2 FIFO empty interrupt state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_EMPTY_BIT
 */
void L3G4200D::setINT2FIFOEmptyInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_EMPTY_BIT, 
		enabled);
}

/** Get whether the INT2 FIFO empty interrupt is enabled
 * @returns Trur if the INT2 FIFO empty interrupt is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_EMPTY_BIT
 */
bool L3G4200D::getINT2FIFOEmptyInterruptEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_EMPTY_BIT, 
		buffer);
	return buffer[0];
}

// CTRL_REG4 register, r/w

/** Set the Block Data Update (BDU) enabled state
 * @param enabled New BDU enabled state
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_BDU_BIT
 */
void L3G4200D::setBlockDataUpdateEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_BDU_BIT, enabled);
}

/** Get the BDU enabled state
 * @return True if Block Data Update is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_BDU_BIT
 */
bool L3G4200D::getBlockDataUpdateEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_BDU_BIT, buffer);
	return buffer[0];
}

/** Set the data endian modes
 * In Big Endian mode, the Most Significat Byte (MSB) is on the lower address, 
 * and the Least Significant Byte (LSB) is on the higher address. Little Endian 
 * mode reverses this order. Little Endian is the default mode.
 * @param endianness New endian mode
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_BLE_BIT
 * @see L3G4200D_BIG_ENDIAN
 * @see L3G4200D_LITTLE_ENDIAN
 */
void L3G4200D::setEndianMode(bool endianness) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_BLE_BIT, 
		endianness);
}

/** Get the data endian mode
 * @return Current endian mode
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_BLE_BIT
 * @see L3G4200D_BIG_ENDIAN
 * @see L3G4200D_LITTLE_ENDIAN
 */
bool L3G4200D::getEndianMode() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_BLE_BIT,
		buffer);
	return buffer[0];
}

/** Set the full scale of the data output (in dps)
 * @param scale The new scale of the data output (250, 500, 2000)
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_FS_BIT
 * @see L3G4200D_FS_LENGTH
 * @see L3G4200D_FS_250
 * @see L3G4200D_FS_500
 * @see L3G4200D_FS_2000
 */
void L3G4200D::setFullScale(uint16_t scale) {
	uint8_t writeBits;
	
	if (scale == 250) {
		writeBits = L3G4200D_FS_250;
	} else if (scale == 500) {
		writeBits = L3G4200D_FS_500;
	} else {
		writeBits = L3G4200D_FS_2000;
	}

	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_FS_BIT, 
		L3G4200D_FS_LENGTH, writeBits);
}

/** Get the current full scale of the output data (in dps)
 * @return Current scale of the output data
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_FS_BIT
 * @see L3G4200D_FS_LENGTH
 * @see L3G4200D_FS_250
 * @see L3G4200D_FS_500
 * @see L3G4200D_FS_2000
 */
uint16_t L3G4200D::getFullScale() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG4, 
		L3G4200D_FS_BIT, L3G4200D_FS_LENGTH, buffer);
	uint8_t readBits = buffer[0];
	
	if (readBits == L3G4200D_FS_250) {
		return 250;
	} else if (readBits == L3G4200D_FS_500) {
		return 500;
	} else {
		return 2000;
	}
}

/** Set the self test mode
 * @param mode New self test mode (Normal, 0, 1)
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_ST_BIT
 * @see L3G4200D_ST_LENGTH
 * @see L3G4200D_SELF_TEST_NORMAL
 * @see L3G4200D_SELF_TEST_0
 * @see L3G4200D_SELF_TEST_1
 */
void L3G4200D::setSelfTestMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_ST_BIT, 
		L3G4200D_ST_LENGTH, mode);
}

/** Get the current self test mode
 * @return Current self test mode
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_ST_BIT
 * @see L3G4200D_ST_LENGTH
 * @see L3G4200D_SELF_TEST_NORMAL
 * @see L3G4200D_SELF_TEST_0
 * @see L3G4200D_SELF_TEST_1
 */
uint8_t L3G4200D::getSelfTestMode() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_ST_BIT, 
		L3G4200D_ST_LENGTH, buffer);
	return buffer[0];
}

/** Set the SPI mode
 * @param mode New SPI mode
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_SIM_BIT
 * @see L3G4200D_SPI_4_WIRE
 * @see L3G4200D_SPI_3_WIRE
 */
void L3G4200D::setSPIMode(bool mode) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_SIM_BIT, mode);
}

/** Get the SPI mode
 * @return Current SPI mode
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_SIM_BIT
 * @see L3G4200D_SPI_4_WIRE
 * @see L3G4200D_SPI_3_WIRE
 */
bool L3G4200D::getSPIMode() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_SIM_BIT, 
		buffer);
 	return buffer[0];
}

// CTRL_REG5 register, r/w

/** Reboots the FIFO memory content
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_BOOT_BIT
 */
void L3G4200D::rebootMemoryContent() {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_BOOT_BIT, true);
}

/** Set whether the FIFO buffer is enabled
 * @param enabled New enabled state of the FIFO buffer
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_FIFO_EN_BIT
 */
void L3G4200D::setFIFOEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_FIFO_EN_BIT, 
		enabled);
}

/** Get whether the FIFO buffer is enabled
 * @return True if the FIFO buffer is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_FIFO_EN_BIT
 */
bool L3G4200D::getFIFOEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_FIFO_EN_BIT, 
		buffer);
	return buffer[0];
}

/** Set the high pass filter enabled state
 * @param enabled New high pass filter enabled state
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_HPEN_BIT
 */
void L3G4200D::setHighPassFilterEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_HPEN_BIT, 
		enabled);
}

/** Get whether the high pass filter is enabled
 * @return True if the high pass filter is enabled, false otherwise
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_HPEN_BIT
 */
bool L3G4200D::getHighPassFilterEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_HPEN_BIT,
		buffer);
	return buffer[0];
}

/** Sets the filter mode to one of the four provided.
 * This function also uses the setHighPassFilterEnabled function in order to set
 * the mode. That function does not haved to be called in addition to this one. 
 * In addition to setting the filter for the data in the FIFO buffer 
 * (controlled by the bits written to OUT_SEL), this function also sets the
 * filter used for interrupt generation (the bits written to INT1_SEL) to be the
 * same as the filter used for the FIFO buffer.
 * @param filter New method to be used when filtering data
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_INT1_SEL_BIT
 * @see L3G4200D_INT1_SEL_LENGTH
 * @see L3G4200D_OUT_SEL_BIT
 * @see L3G4200D_OUT_SEL_LENGTH
 * @see L3G4200D_NON_HIGH_PASS
 * @see L3G4200D_HIGH_PASS
 * @see L3G4200D_LOW_PASS
 * @see L3G4200D_LOW_HIGH_PASS
 */
void L3G4200D::setDataFilter(uint8_t filter) {
	if (filter == L3G4200D_HIGH_PASS || filter == L3G4200D_LOW_HIGH_PASS) {
		setHighPassFilterEnabled(true);
	} else {
		setHighPassFilterEnabled(false);
	}
	
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_OUT_SEL_BIT, 
		L3G4200D_OUT_SEL_LENGTH, filter);
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_INT1_SEL_BIT, 
		L3G4200D_INT1_SEL_LENGTH, filter);
}

/** Gets the data filter currently in use
 * @return Defined value that represents the filter in use
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_OUT_SEL_BIT
 * @see L3G4200D_OUT_SEL_LENGTH
 * @see L3G4200D_NON_HIGH_PASS
 * @see L3G4200D_HIGH_PASS
 * @see L3G4200D_LOW_PASS
 * @see L3G4200D_LOW_HIGH_PASS
 */
uint8_t L3G4200D::getDataFilter() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_OUT_SEL_BIT, 
		L3G4200D_OUT_SEL_LENGTH, buffer);
	uint8_t outBits = buffer[0];

	if (outBits == L3G4200D_NON_HIGH_PASS || outBits == L3G4200D_HIGH_PASS) {
		return outBits;
	}

	if (getHighPassFilterEnabled()) {
		return L3G4200D_LOW_HIGH_PASS;
	} else {
		return L3G4200D_LOW_PASS;
	}
}

// REFERENCE/DATACAPTURE register, r/w

/** Set the reference value for interrupt generation
 * @param reference New reference value for interrupt generation
 * @see L3G4200D_RA_REFERENCE
 */
void L3G4200D::setInterruptReference(uint8_t reference) {
	I2Cdev::writeByte(devAddr, L3G4200D_RA_REFERENCE, reference);
}

/** Get the 8-bit reference value for interrupt generation
 * @return 8-bit reference value for interrupt generation
 * @see L3G4200D_RA_REFERENCE
 */
uint8_t L3G4200D::getInterruptReference() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_REFERENCE, buffer);
	return buffer[0];
}

// OUT_TEMP register, read-only

/** Gets the current temperature reading from the sensor
 * @return Current temperature
 * @see L3G4200D_RA_OUT_TEMP
 */
uint8_t L3G4200D::getTemperature() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_OUT_TEMP, buffer);
	return buffer[0];
}

// STATUS register, read-only

/** Get whether new data overwrote the last set of data before it was read
 * @return True if the last set of data was overwritten before being read, false
 * otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZYXOR_BIT
 */
bool L3G4200D::getXYZOverrun() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZYXOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether new Z data overwrote the last set of data before it was read
 * @return True if the last set of Z data was overwritten before being read,
 * false otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZOR_BIT
 */
bool L3G4200D::getZOverrun() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether new Y data overwrote the last set of data before it was read
 * @return True if the last set of Y data was overwritten before being read, 
 * false otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_YOR_BIT
 */
bool L3G4200D::getYOverrun() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_YOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether new X data overwrote the last set of data before it was read
 * @return True if the last set of X data was overwritten before being read, 
 * false otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_XOR_BIT
 */
bool L3G4200D::getXOverrun() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_XOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new data avaialable
 * @return True if there is new data available, false otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZYXDA_BIT
 */
bool L3G4200D::getXYZDataAvailable() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZYXDA_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new Z data avaialable
 * @return True if there is new Z data available, false otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZDA_BIT
 */
bool L3G4200D::getZDataAvailable() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZDA_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new Y data avaialable
 * @return True if there is new Y data available, false otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_YDA_BIT
 */
bool L3G4200D::getYDataAvailable() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_YDA_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new X data avaialable
 * @return True if there is new X data available, false otherwise
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_XDA_BIT
 */
bool L3G4200D::getXDataAvailable() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_XDA_BIT, 
		buffer);
	return buffer[0];
}

// OUT_* registers, read-only

/** Get the angular velocity for all 3 axes
 * Due to the fact that this device supports two difference Endian modes, both 
 * must be accounted for when reading data. In Little Endian mode, the first 
 * byte (lowest address) is the least significant and in Big Endian mode the 
 * first byte is the most significant.
 * @param x 16-bit integer container for the X-axis angular velocity
 * @param y 16-bit integer container for the Y-axis angular velocity
 * @param z 16-bit integer container for the Z-axis angular velocity
 */
void L3G4200D::getAngularVelocity(int16_t* x, int16_t* y, int16_t* z) {
	*x = getAngularVelocityX();
	*y = getAngularVelocityY();
	*z = getAngularVelocityZ();
}

/** Get the angular velocity about the X-axis
 * @return Angular velocity about the X-axis
 * @see L3G4200D_RA_OUT_X_L
 * @see L3G4200D_RA_OUT_X_H
 */
int16_t L3G4200D::getAngularVelocityX() {
	uint8_t bufferL[6];
	uint8_t bufferH[6];
	I2Cdev::readBytes(devAddr, L3G4200D_RA_OUT_X_L, 1, bufferL);
	I2Cdev::readBytes(devAddr, L3G4200D_RA_OUT_X_H, 1, bufferH);
	if (getEndianMode() == L3G4200D_BIG_ENDIAN) {
		return (((int16_t) bufferL[0]) << 8) | bufferH[0];
	} else {
		return (((int16_t) bufferH[0]) << 8) | bufferL[0];
	}
}
	
/** Get the angular velocity about the Y-axis
 * @return Angular velocity about the Y-axis
 * @see L3G4200D_RA_OUT_Y_L
 * @see L3G4200D_RA_OUT_Y_H
 */
int16_t L3G4200D::getAngularVelocityY() {
	uint8_t bufferL[6];
	uint8_t bufferH[6];
	I2Cdev::readBytes(devAddr, L3G4200D_RA_OUT_Y_L, 1, bufferL);
	I2Cdev::readBytes(devAddr, L3G4200D_RA_OUT_Y_H, 1, bufferH);
	if (getEndianMode() == L3G4200D_BIG_ENDIAN) {
		return (((int16_t) bufferL[0]) << 8) | bufferH[0];
	} else {
		return (((int16_t) bufferH[0]) << 8) | bufferL[0];
	}
}

/** Get the angular velocity about the Z-axis
 * @return Angular velocity about the Z-axis
 * @see L3G4200D_RA_OUT_Z_L
 * @see L3G4200D_RA_OUT_Z_H
 */
int16_t L3G4200D::getAngularVelocityZ() {
	uint8_t bufferL[6];
	uint8_t bufferH[6];
	I2Cdev::readBytes(devAddr, L3G4200D_RA_OUT_Z_L, 1, bufferL);
	I2Cdev::readBytes(devAddr, L3G4200D_RA_OUT_Z_H, 1, bufferH);
	if (getEndianMode() == L3G4200D_BIG_ENDIAN) {
		return (((int16_t) bufferL[0]) << 8) | bufferH[0];
	} else {
		return (((int16_t) bufferH[0]) << 8) | bufferL[0];
	}
}

// FIFO_CTRL register, r/w

/** Set the FIFO mode to one of the defined modes
 * @param mode New FIFO mode
 * @see L3G4200D_RA_FIFO_CTRL
 * @see L3G4200D_FIFO_MODE_BIT
 * @see L3G4200D_FIFO_MODE_LENGTH
 * @see L3G4200D_FM_BYPASS
 * @see L3G4200D_FM_FIFO
 * @see L3G4200D_FM_STREAM
 * @see L3G4200D_FM_STREAM_FIFO
 * @see L3G4200D_FM_BYPASS_STREAM
 */
void L3G4200D::setFIFOMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3G4200D_RA_FIFO_CTRL, L3G4200D_FIFO_MODE_BIT, 
		L3G4200D_FIFO_MODE_LENGTH, mode);
}

/** Get the FIFO mode to one of the defined modes
 * @return Current FIFO mode
 * @see L3G4200D_RA_FIFO_CTRL
 * @see L3G4200D_FIFO_MODE_BIT
 * @see L3G4200D_FIFO_MODE_LENGTH
 * @see L3G4200D_FM_BYPASS
 * @see L3G4200D_FM_FIFO
 * @see L3G4200D_FM_STREAM
 * @see L3G4200D_FM_STREAM_FIFO
 * @see L3G4200D_FM_BYPASS_STREAM
 */
uint8_t L3G4200D::getFIFOMode() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_FIFO_CTRL, 
		L3G4200D_FIFO_MODE_BIT, L3G4200D_FIFO_MODE_LENGTH, buffer);
	return buffer[0];
}

/** Set the FIFO watermark threshold
 * @param wtm New FIFO watermark threshold
 * @see L3G4200D_RA_FIFO_CTRL
 * @see L3G4200D_FIFO_WTM_BIT
 * @see L3G4200D_FIFO_WTM_LENGTH
 */
void L3G4200D::setFIFOThreshold(uint8_t wtm) {
    I2Cdev::writeBits(devAddr, L3G4200D_RA_FIFO_CTRL, L3G4200D_FIFO_WTM_BIT, 
        L3G4200D_FIFO_WTM_LENGTH, wtm);
}

/** Get the FIFO watermark threshold
 * @return FIFO watermark threshold
 * @see L3G4200D_RA_FIFO_CTRL
 * @see L3G4200D_FIFO_WTM_BIT
 * @see L3G4200D_FIFO_WTM_LENGTH
 */
uint8_t L3G4200D::getFIFOThreshold() {
    I2Cdev::readBits(devAddr, L3G4200D_RA_FIFO_CTRL, L3G4200D_FIFO_WTM_BIT,
        L3G4200D_FIFO_WTM_LENGTH, buffer);
    return buffer[0];
}

// FIFO_SRC register, read-only

/** Get whether the number of data sets in the FIFO buffer is less than the 
 * watermark
 * @return True if the number of data sets in the FIFO buffer is more than or 
 * equal to the watermark, false otherwise.
 * @see L3G4200D_RA_FIFO_SRC
 * @see L3G4200D_FIFO_STATUS_BIT
 */
bool L3G4200D::getFIFOAtWatermark() {
   	I2Cdev::readBit(devAddr, L3G4200D_RA_FIFO_SRC, L3G4200D_FIFO_STATUS_BIT, 
        buffer);
   	return buffer[0];
}

/** Get whether the FIFO buffer is full
 * @return True if the FIFO buffer is full, false otherwise
 * @see L3G4200D_RA_FIFO_SRC
 * @see L3G4200D_FIFO_OVRN_BIT
 */
bool L3G4200D::getFIFOOverrun() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_FIFO_SRC, 
        L3G4200D_FIFO_OVRN_BIT, buffer);
    return buffer[0];
}

/** Get whether the FIFO buffer is empty
 * @return True if the FIFO buffer is empty, false otherwise
 * @see L3G4200D_RA_FIFO_SRC
 * @see L3G4200D_FIFO_EMPTY_BIT
 */
bool L3G4200D::getFIFOEmpty() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_FIFO_SRC,
        L3G4200D_FIFO_EMPTY_BIT, buffer);
    return buffer[0];
}

/** Get the number of filled FIFO buffer slots
 * @return Number of filled slots in the FIFO buffer
 * @see L3G4200D_RA_FIFO_SRC
 * @see L3G4200D_FIFO_FSS_BIT
 * @see L3G4200D_FIFO_FSS_LENGTH
 */ 
uint8_t L3G4200D::getFIFOStoredDataLevel() {
    I2Cdev::readBits(devAddr, L3G4200D_RA_FIFO_SRC, 
        L3G4200D_FIFO_FSS_BIT, L3G4200D_FIFO_FSS_LENGTH, buffer);
    return buffer[0];
}

// INT1_CFG register, r/w

/** Set the combination mode for interrupt events
 * @param combination New combination mode for interrupt events. 
 * L3G4200D_INT1_OR for OR and L3G4200D_INT1_AND for AND
 * @see L3G4200D_RA_INT1_CFG
 * @see L3G4200D_INT1_AND_OR_BIT
 * @see L3G4200D_INT1_OR
 * @see L3G4200D_INT1_AND
 */
void L3G4200D::setInterruptCombination(bool combination) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_INT1_AND_OR_BIT,
        combination);
}

/** Get the combination mode for interrupt events
 * @return Combination mode for interrupt events. L3G4200D_INT1_OR for OR and 
 * L3G4200D_INT1_AND for AND
 * @see L3G4200D_RA_INT1_CFG
 * @see L3G4200D_INT1_AND_OR_BIT
 * @see L3G4200D_INT1_OR
 * @see L3G4200D_INT1_AND
 */
bool L3G4200D::getInterruptCombination() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_INT1_AND_OR_BIT,
        buffer);
    return buffer[0];
}

/** Set whether an interrupt request is latched
 * This bit is cleared when the INT1_SRC register is read
 * @param latched New status of the latched request
 * @see L3G4200D_RA_INT1_CFG
 * @see L3G4200D_INT1_LIR_BIT
 */
void L3G4200D::setInterruptRequestLatched(bool latched) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_INT1_LIR_BIT, latched);
}

/** Get whether an interrupt request is latched
 * @return True if an interrupt request is latched, false otherwise
 * @see L3G4200D_RA_INT1_CFG
 * @see L3G4200D_INT1_LIR_BIT
 */
bool L3G4200D::getInterruptRequestLatched() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_INT1_LIR_BIT, 
        buffer); 
    return buffer[0];
};

/** Set whether the interrupt for Z high is enabled
 * @param enabled New enabled state for Z high interrupt.
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_ZHIE_BIT
 */
void L3G4200D::setZHighInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_ZHIE_BIT, enabled);
}

/** Get whether the interrupt for Z high is enabled
 * @return True if the interrupt for Z high is enabled, false otherwise 
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_ZHIE_BIT
 */
bool L3G4200D::getZHighInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_ZHIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for Z low is enabled
 * @param enabled New enabled state for Z low interrupt.
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_ZLIE_BIT
 */
void L3G4200D::setZLowInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_ZLIE_BIT, enabled);
}

/** Get whether the interrupt for Z low is enabled
 * @return True if the interrupt for Z low is enabled, false otherwise
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_ZLIE_BIT
 */
bool L3G4200D::getZLowInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_ZLIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for Y high is enabled
 * @param enabled New enabled state for Y high interrupt.
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_YHIE_BIT
 */
void L3G4200D::setYHighInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_YHIE_BIT, enabled);
}

/** Get whether the interrupt for Y high is enabled
 * @return True if the interrupt for Y high is enabled, false otherwise
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_YHIE_BIT
 */
bool L3G4200D::getYHighInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_YHIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for Y low is enabled
 * @param enabled New enabled state for Y low interrupt.
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_YLIE_BIT
 */
void L3G4200D::setYLowInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_YLIE_BIT, enabled);
}

/** Get whether the interrupt for Y low is enabled
 * @return True if the interrupt for Y low is enabled, false otherwise 
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_YLIE_BIT
 */
bool L3G4200D::getYLowInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_YLIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for X high is enabled
 * @param enabled New enabled state for X high interrupt.
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_XHIE_BIT
 */
void L3G4200D::setXHighInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_XHIE_BIT, enabled);
}

/** Get whether the interrupt for X high is enabled
 * @return True if the interrupt for X high is enabled, false otherwise
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_XHIE_BIT
 */
bool L3G4200D::getXHighInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_XHIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for X low is enabled
 * @param enabled New enabled state for X low interrupt.
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_XLIE_BIT
 */
void L3G4200D::setXLowInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_XLIE_BIT, enabled);
}

/** Get whether the interrupt for X low is enabled
 * @return True if the interrupt for X low is enabled, false otherwise
 * @see L3G4200D_INT1_CFG
 * @see L3G4200D_XLIE_BIT
 */
bool L3G4200D::getXLowInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_CFG, L3G4200D_XLIE_BIT, 
        buffer);
    return buffer[0];
}

// INT1_SRC register, read-only

/** Get whether an interrupt has been generated
 * @return True if one or more interrupts has been generated, false otherwise
 * @see L3G4200D_RA_INT1_SRC
 * @see L3G4200D_INT1_IA_BIT
 */
bool L3G4200D::getInterruptActive() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_SRC, L3G4200D_INT1_IA_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Z high event has occurred
 * @return True if a Z high event has occurred, false otherwise
 * @see L3G4200D_RA_INT1_SRC
 * @see L3G4200D_INT1_ZH_BIT
 */
bool L3G4200D::getZHigh() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_SRC, L3G4200D_INT1_ZH_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Z low event has occurred
 * @return True if a Z low event has occurred, false otherwise
 * @see L3G4200D_RA_INT1_SRC
 * @see L3G4200D_INT1_ZL_BIT
 */
bool L3G4200D::getZLow() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_SRC, L3G4200D_INT1_ZL_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Y high event has occurred
 * @return True if a Y high event has occurred, false otherwise
 * @see L3G4200D_RA_INT1_SRC
 * @see L3G4200D_INT1_YH_BIT
 */
bool L3G4200D::getYHigh() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_SRC, L3G4200D_INT1_YH_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Y low event has occurred
 * @return True if a Y low event has occurred, false otherwise
 * @see L3G4200D_RA_INT1_SRC
 * @see L3G4200D_INT1_YL_BIT
 */
bool L3G4200D::getYLow() {
   	I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_SRC, L3G4200D_INT1_YL_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a X high event has occurred
 * @return True if a X high event has occurred, false otherwise
 * @see L3G4200D_RA_INT1_SRC
 * @see L3G4200D_INT1_XH_BIT
 */
bool L3G4200D::getXHigh() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_SRC, L3G4200D_INT1_XH_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a X low event has occurred
 * @return True if a X low event has occurred, false otherwise
 * @see L3G4200D_RA_INT1_SRC
 * @see L3G4200D_INT1_XL_BIT
 */
bool L3G4200D::getXLow() {
    I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_SRC, L3G4200D_INT1_XL_BIT,
        buffer);
    return buffer[0];
}

// INT1_THS_* registers, r/w

/** Set the threshold for a high interrupt on the X axis
 * @param threshold New threshold for a high interrupt on the X axis
 * @see L3G4200D_INT1_THS_XH
 */
void L3G4200D::setXHighThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_THS_XH, threshold);
}

/** Retrieve the threshold for a high interrupt on the X axis
 * @return X high interrupt threshold
 * @see L3G4200D_INT1_THS_XH
 */
uint8_t L3G4200D::getXHighThreshold() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_THS_XH, buffer);
	return buffer[0];
}

/** Set the threshold for a low interrupt on the X axis
 * @param threshold New threshold for a low interrupt on the X axis
 * @see L3G4200D_INT1_THS_XL
 */
void L3G4200D::setXLowThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_THS_XL, threshold);
}

/** Retrieve the threshold for a low interrupt on the X axis
 * @return X low interrupt threshold
 * @see L3G4200D_INT1_THS_XL
 */
uint8_t L3G4200D::getXLowThreshold() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_THS_XL, buffer);
	return buffer[0];
}

/** Set the threshold for a high interrupt on the Y axis
 * @param threshold New threshold for a high interrupt on the Y axis
 * @see L3G4200D_INT1_THS_YH
 */
void L3G4200D::setYHighThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_THS_YH, threshold);
}

/** Retrieve the threshold for a high interrupt on the Y axis
 * @return Y high interrupt threshold
 * @see L3G4200D_INT1_THS_YH
 */
uint8_t L3G4200D::getYHighThreshold() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_THS_YH, buffer);
	return buffer[0];
}

/** Set the threshold for a low interrupt on the Y axis
 * @param threshold New threshold for a low interrupt on the Y axis
 * @see L3G4200D_INT1_THS_YL
 */
void L3G4200D::setYLowThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_THS_YL, threshold);
}

/** Retrieve the threshold for a low interrupt on the Y axis
 * @return Y low interrupt threshold
 * @see L3G4200D_INT1_THS_YL
 */
uint8_t L3G4200D::getYLowThreshold() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_THS_YL, buffer);
	return buffer[0];
}

/** Set the threshold for a high interrupt on the Z axis
 * @param threshold New threshold for a high interrupt on the Z axis
 * @see L3G4200D_INT1_THS_ZH
 */
void L3G4200D::setZHighThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_THS_ZH, threshold);
}

/** Retrieve the threshold for a high interrupt on the Z axis
 * @return Z high interrupt threshold
 * @see L3G4200D_INT1_THS_ZH
 */
uint8_t L3G4200D::getZHighThreshold() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_THS_ZH, buffer);
	return buffer[0];
}

/** Set the threshold for a low interrupt on the Z axis
 * @param threshold New threshold for a low interrupt on the Z axis
 * @see L3G4200D_RA_INT1_THS_ZL
 */
void L3G4200D::setZLowThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3G4200D_RA_INT1_THS_ZL, threshold);
}

/** Retrieve the threshold for a low interrupt on the Z axis
 * @return Z low interrupt threshold
 * @see L3G4200D_INT1_THS_ZL
 */
uint8_t L3G4200D::getZLowThreshold() {
	I2Cdev::readByte(devAddr, L3G4200D_RA_INT1_THS_ZL, buffer);
	return buffer[0];
}

// INT1_DURATION register, r/w

/* Set the minimum duration for an interrupt event to be recognized
 * This depends on the chosen output data rate
 * @param duration New duration necessary for an interrupt event to be 
 * recognized
 * @see L3G4200D_RA_INT1_DURATION
 * @see L3G4200D_INT1_DUR_BIT
 * @see L3G4200D_INT1_DUR_LENGTH
 */
void L3G4200D::setDuration(uint8_t duration) {
	I2Cdev::writeBits(devAddr, L3G4200D_RA_INT1_DURATION, L3G4200D_INT1_DUR_BIT,
		L3G4200D_INT1_DUR_LENGTH, duration);
}

/** Get the minimum duration for an interrupt event to be recognized
 * @return Duration necessary for an interrupt event to be recognized
 * @see L3G4200D_RA_INT1_DURATION
 * @see L3G4200D_INT1_DUR_BIT
 * @see L3G4200D_INT1_DUR_LENGTH
 */
uint8_t L3G4200D::getDuration() {
	I2Cdev::readBits(devAddr, L3G4200D_RA_INT1_DURATION, 
		L3G4200D_INT1_DUR_BIT, L3G4200D_INT1_DUR_LENGTH, buffer);
	return buffer[0];
}

/** Set whether the interrupt wait feature is enabled
 * If false, the interrupt falls immediately if signal crosses the selected 
 * threshold. Otherwise, if signal crosses the selected threshold, the interrupt
 * falls only after the duration has counted number of samples at the selected 
 * data rate, written into the duration counter register.
 * @param enabled New enabled state of the interrupt wait
 * @see L3G4200D_RA_INT1_DURATION
 * @see L3G4200D_INT1_WAIT_BIT
 */
void L3G4200D::setWaitEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_INT1_DURATION, L3G4200D_INT1_WAIT_BIT,
		enabled);
}

/** Get whether the interrupt wait feature is enabled
 * @return True if the wait feature is enabled, false otherwise
 * @see L3G4200D_RA_INT1_DURATION
 * @see L3G4200D_INT1_WAIT_BIT
 */
bool L3G4200D::getWaitEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_RA_INT1_DURATION, 
		L3G4200D_INT1_WAIT_BIT, buffer);
	return buffer[0];
}
