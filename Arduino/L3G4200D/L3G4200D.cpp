// I2Cdev library collection - L3G4200D I2C device class header file
// Based on STMicroelectronics L3G4200D datasheet rev. 3, 12/2010
// TODO Add release date here
// [current release date] by Jonathan "j3rn" Arnett <j3rn@j3rn.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// TODO Add initial release date here as well
// Changelog:
//     [YYYY-mm-dd] - initial release

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
 */
void L3G4200D::initialize() {
    setPowerOn(true);
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
 * @return the device ID (should be 0b11010011, 109, 0x69)
 * @see L3G4200D_RA_WHO_AM_I
 */
uint8_t L3G4200D::getDeviceID() {
    // read a single byte and return it
    I2Cdev::readByte(devAddr, L3G4200D_RA_WHO_AM_I, buffer);
    return buffer[0];
}

// CTRL_REG1 register, r/w

/** Set the data output rate
 * @param rate The new data output rate (can be 100, 200, 400, or 800)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 */
void L3G4200D::setDataOutputRate(uINT16_t rate) {
	uint8_t writeVal;
	
	if (rate == 100) {
		writeVal = L3G4200D_RATE_100;
	} else if (rate == 200) {
		writeVal = L3G4200D_RATE_200;
	} else if (rate == 400) {
		writeVal = L3G4200D_RATE_400;
	} else {
		// Default/fallback is 800
		writeVal = L3G4200D_RATE_800;
	}
	
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ODR_BIT,
		L3G4200D_ODR_LENGTH, writeVal); 
}

/** Get the current data output rate
 * @return the current data output rate
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ODR_BIT
 * @see L3G4200D_ODR_LENGTH
 * @see L3G4200D_RATE_100
 * @see L3G4200D_RATE_200
 * @see L3G4200D_RATE_400
 * @see L3G4200D_RATE_800
 */
uINT16_t L3G4200D::getDataOutputRate() {
	// Get rate from device
	uint8_t rate = I2Cdec::readBits(devAddr, L3G4200D_RA_CTRL_REG1,
		L3G4200D_ODR_BIT, L3G4200D_ODR_LENGTH, buffer);
	
	// Return the matching rate
	if (rate == L3G4200D_RATE_100) {
		return 100;
	} else if (rate == L3G4200D_RATE_200) {
		return 200;
	} else if (rate == L3G4200D_RATE_400) {
		return 400;
	}

	// We can assume that if it is not one of the previous, it is 800
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
	// Assume cutOff is a valid mode
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_BIT, 
		L3G4200D_BW_LENGTH, mode);
}

/** Get the current bandwidth cut-off mode
 * @return the current bandwidth cut off mode
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_BW_BIT
 * @see L3G4200D_BW_LENGTH
 * @see L3G4200D_BW_LOW
 * @see L3G4200D_BW_MED_LOW
 * @see L3G4200D_BW_MED_HIGH
 * @see L3G4200D_BW_HIGH
 */
uint8_t L3G4200D::getBandwidthCutOffMode() {
	return I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_BW_BIT, 
		L3G4200D_BW_LENGTH, buffer);
}

/** Gets the current bandwidth cutoff based on ODR and BW
 * @return the float value of the bandwidth cut off
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
	// Get necessary info from device
	int8_t dataRate = I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, 
		L3G4200D_ODR_BIT, L3G4200D_ODR_LENGTH, buffer);
	int8_t bandwidthMode = I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG1, 
		L3G4200D_BW_BIT, L3G4200D_BW_LENGTH, buffer);

	// Compute cut-off based on table in datasheet
	if (dataRate == L3G4200D_RATE_100) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 12.5;
		} else {
			return 25.0;
		}
	} else if (dataRate == L3G4200D_RATE_200) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 12.5;
		} else if (bandwidthMode == L3G4200D_BW_MED_LOW) {
			return 25.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_HIGH) {
			return 50.0;
		} else {
			// L3G4200D_BW_HIGH assumed
			return 70.0;
		}
	} else if (dataRate == L3G4200D_RATE_400) {
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 20.0;
		} else if (bandwidthMode == L3G4200D_MED_LOW) {
			return 25.0;
		} else if (bandwidthMode == L3G4200D_MED_HIGH) {
			return 50.0;
		} else {
			// L3G4200D_BW_HIGH assumed
			return 110.0;
		}
	} else {
		// L3G4200D_RATE_800 assumed
		if (bandwidthMode == L3G4200D_BW_LOW) {
			return 30.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_LOW) {
			return 35.0;
		} else if (bandwidthMode == L3G4200D_BW_MED_HIGH) {
			return 50.0;
		} else {
			// L3G4200D_BW_HIGH assumed
			return 110.0;
		}
	}
}

/** Set power on or off
 * @param enabled The new power setting (TRUE for ON, FALSE for OFF)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_PD_BIT
 */
void L3G4200D::setPowerOn(bool on) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_PD_BIT, on);
}

/** Get the current power state
 * @return The powered on state (TRUE for ON, FALSE for OFF)
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_PD_BIT
 */
bool L3G4200D::getPowerOn() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_PD_BIT, buffer);
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
 * @return the z axis enabled state
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_ZEN_BIT
 */
bool L3G4200D::getZEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_ZEN_BIT, 
		buffer);
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
 * @return the y axis enabled state
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_YEN_BIT
 */
bool L3G4200D::getYEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_YEN_BIT, 
		buffer);
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
 * @return the x axis enabled state
 * @see L3G4200D_RA_CTRL_REG1
 * @see L3G4200D_XEN_BIT
 */
bool L3G4200D::getXEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG1, L3G4200D_XEN_BIT, 
		buffer);
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
	I2Cdev::writeBits(devAddr, L3G4200D_CTRL_REG2, L3G4200D_HPM_BIT, 
		L3G4200D_HPM_LENGTH, mode);
}

/** Get the high pass mode
 * @return the high pass mode
 * @see L3G4200D_RA_CTRL_REG2
 * @see L3G4200D_HPM_BIT
 * @see L3G4200D_HPM_LENGTH
 * @see L3G4200D_HPM_HRF
 * @see L3G4200D_HPM_REFERENCE
 * @see L3G4200D_HPM_NORMAL
 * @see L3G4200D_HPM_AUTORESET
 */
uint8_t L3G4200D::getHighPassMode() {
	return I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG2, L3G4200D_HPM_BIT, 
		L3G4200D_HPM_LENGTH, buffer);
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
 * @return the high pass filter cut off frequency level
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
	return I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG2, L3G4200D_HPCF_BIT, 
		L3G4200D_HPCF_LENGTH, buffer);
}

// CTRL_REG3 register, r/w

/** Set the INT1 interrupt enabled state
 * @param enabled The new enabled stat for the INT1 interrupt
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_INT1_BIT
 */
void L3G4200D::setINT1InterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_INT1_BIT, 
		enabled);
}

/** Get the INT1 interrupt enabled state
 * @return the INT1 interrupt enabled status
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_INT1_BIT
 */
bool L3G4200D::getINT1InterruptEnabled(bool enabled) {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_INT1_BIT, 
		buffer);
}

/** Set the INT1 boot status enabled state
 * @param enabled The new enabled state for the INT1 boot status
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_BOOT_BIT
 */
void L3G4200D::setINT1BootStatusEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_BOOT_BIT, 
		enabled);
}

/** Get the INT1 boot status enabled state
 * @return the INT1 boot status status
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I1_BOOT_BIT
 */
bool L3G4200D::getINT1BootStatusEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I1_BOOT_BIT, 
		buffer);
}

/** Interrupts the active INT1 configuration
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_H_LACTIVE_BIT
 */
void L3G4200D::interruptACtiveINT1Config() {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_H_LACTIVE_BIT, 1);
}

/** Set output mode to push-pull or open-drain
 * @param mode Set output mode (TRUE for push-pull, FALSE for open-drain)
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_PP_OD_BIT
 * @see L3G4200D_PUSH_PULL
 * @see L3G4200D_OPEN_DRAIN
 */
void L3G4200D::setOutputMode(bool mode) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_PP_OD_BIT, 
		pushPull);
}

/** Get whether mode is push-pull or open drain
 * @return the output mode (TRUE for push-pull, FALSE for open-drain)
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_PP_OD_BIT
 * @see L3G4200D_PUSH_PULL
 * @see L3G4200D_OPEN_DRAIN
 */
bool L3G4200D::getOutputMode() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_PP_OD_BIT, 
		buffer);
}

/** Set data ready interrupt enabled state on INT2 pin
 * @param enabled The new INT2 data ready interrupt enabled state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_DRDY_BIT
 */
void L3G4200D::setINT2DataReadyEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_DRDY_BIT, 
		enabled);
}

/** Get whether the data ready interrupt is enabled on the INT2 pin
 * @return the INT2 data ready interrupt enabled state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_DRDY_BIT
 */
bool L3G4200D::getINT2DataReadyEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_DRDY_BIT, 
		buffer);
}

/** Set whether the INT2 FIFO watermark interrupt is enabled
 * The sensor contains a 32-slot FIFO buffer for storing data so that it may be 
 * read later. If enabled, the sensor will generate an interrupt on the 
 * INT2/DRDY pin when the watermark has been reached. The watermark can be 
 * configured through the setFIFOWatermark function.
 * @param enabled The new enabled state of the INT2 FIFO watermark
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_WTM_BIT
 */
void L3G4200D::setINT2FIFOWatermarkInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_WTM_BIT, 
		enabled);
}

/** Get the INT2 FIFO watermark interrupt enabled state
 * @return whether the FIFO watermark is enabled
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_WTM_BIT
 */ 
void L3G4200D::getINT2FIFOWatermarkInterruptEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_WTM_BIT, 
		buffer);
}

/** Set whether an interrupt is triggered on INT2 when the FIFO is overrun
 * @param enabled The new FIFO overrun interrupt enabled state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_ORUN_BIT
 */
void L3G4200D::setINT2FIFOOverrunInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_ORUN_BIT, 
		enabled);
}

/** Get whether an interrupt is triggered on INT2 when the FIFO is overrun
 * @return the enabled state of the INT2 FIFO overrun interrupt
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_ORUN_BIT
 */
bool L3G4200D::getINT2FIFOOverrunInterruptEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_ORUN_BIT,
		buffer);
}

/** Set whether an interrupt is triggered on INT2 when the FIFO buffer is empty
 * @param enabled The new INT2 FIFO empty interrupt state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_EMPTY_BIT
 */
void L3G4200D::setINT2FIFOEmptyInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG3, L3G4200D_I2_EMPTY_BIT, 
		enabled);
}

/** Get whether the INT2 FIFO empty interrupt is enabled
 * @returns the INT2 FIFO empty interrupt enabled state
 * @see L3G4200D_RA_CTRL_REG3
 * @see L3G4200D_I2_EMPTY_BIT
 */
bool L3G4200D::getINT2FIFOEmptyInterruptEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG3, 
		L3G4200D_I2_EMPTY_BIT, buffer);
}

// CTRL_REG4 register, r/w

/** Set the Block Data Update (BDU) enabled state
 * @param enabled The new BDU enabled state
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_BDU_BIT
 */
void L3G4200D::setBlockDataUpdateEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_BDU_BIT, enabled);
}

/** Get the BDU enabled state
 * @return the BDU enabled state
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_BDU_BIT
 */
bool L3G4200D::getBlockDataUpdateEnabled() {
	return L3G4200D::readBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_BDU_BIT, 
		buffer);
}

/** Set the data endian mode
 * @param endianness The new endianness mode (TRUE for Big Endian, FALSE for 
 * Little Endian)
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
 * @return the current endian mode (TRUE for Big Endian, FALSE for Little 
 * Endian) 
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_BLE_BIT
 * @see L3G4200D_BIG_ENDIAN
 * @see L3G4200D_LITTLE_ENDIAN
 */
void L3G4200D::getEndianMode() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_BLE_BIT,
		buffer);
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
		// If none of the above, assumed is 2000
		writeBits = L3G4200D_FS_2000;
	}

	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_FS_BIT, 
		L3G4200D_FS_LENGTH, writeBits);
}

/** Get the current full scale of the output data (in dps)
 * @return the current scale of the output data
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_FS_BIT
 * @see L3G4200D_FS_LENGTH
 * @see L3G4200D_FS_250
 * @see L3G4200D_FS_500
 * @see L3G4200D_FS_2000
 */
uint16_t L3G4200D::getFullScale() {
	uint8_t readBits = I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG4, 
		L3G4200D_FS_BIT, L3G4200D_FS_LENGTH, buffer);
	
	if (readBits == L3G4200D_FS_250) {
		return 250;
	} else if (readBits == L3G4200D_FS_500) {
		return 500;
	} else {
		// Assumed if none of the above, is 2000
		return 2000;
	}
}

/** Set the self test mode
 * @param mode The new self test mode (Normal, 0, 1)
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
 * @return the current self test mode
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_ST_BIT
 * @see L3G4200D_ST_LENGTH
 * @see L3G4200D_SELF_TEST_NORMAL
 * @see L3G4200D_SELF_TEST_0
 * @see L3G4200D_SELF_TEST_1
 */
uint8_t L3G4200D::getSelfTestMode() {
	return I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_ST_BIT, 
		L3G4200D_ST_LENGTH, buffer);
}

/** Set the SPI mode
 * @param mode The new SPI mode (TRUE for 4 wire, FALSE for 3 wire)
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_SIM_BIT
 * @see L3G4200D_SPI_4_WIRE
 * @see L3G4200D_SPI_3_WIRE
 */
void L3G4200D::setSPIMode(bool mode) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_SIM_BIT, mode);
}

/** Get the SPI mode
 * @return the current SPI mode (TRUE for 4 wire, FALSE for 3 wire)
 * @see L3G4200D_RA_CTRL_REG4
 * @see L3G4200D_SIM_BIT
 * @see L3G4200D_SPI_4_WIRE
 * @see L3G4200D_SPI_3_WIRE
 */
bool L3G4200D::getSPIMode() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG4, L3G4200D_SIM_BIT, 
		buffer);
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
 * @param enabled The new enabled state of the FIFO buffer
 * @see L3G4200D_CTRL_REG5
 * @see L3G4200D_FIFO_EN_BIT
 */
void L3G4200D::setFIFOEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_CTRL_REG5, L3G4200D_FIFO_EN_BIT, 
		enabled);
}

/** Get whether the FIFO buffer is enabled
 * @return the current enabled state of the FIFO buffer
 * @see L3G4200D_CTRL_REG5
 * @see L3G4200D_FIFO_EN_BIT
 */
bool L3G4200D::getFIFOEnabled() {
	I2Cdev::readBit(devAddr, L3G4200D_CTRL_REG5, L3G4200D_FIFO_EN_BIT, 
		buffer);
}

/** Set the high pass filter enabled state
 * @param enabled The new high pass filter enabled state
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_HPEN_BIT
 */
void L3G4200D::setHighPassFilterEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_HPEN_BIT, 
		enabled);
}

/** Get whether the high pass filter is enabled
 * @return the enabled state of the high pass filter
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_HPEN_BIT
 */
bool L3G4200D::getHighPassFilterEnabled() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_HPEN_BIT,
		buffer);
}

/** Sets the filter mode to one of the four provided.
 * This function also uses the setHighPassFilterEnabled function in order to set
 * the mode. That function does not haved to be called in addition to this one.
 * @param filter The new method to be used when filtering data
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_INT1_SEL_BIT
 * @see L3G4200D_INT1_SEL_LENGTH
 * @see L3G4200D_OUT_SEL_BIT
 * @see L3G4200D_OUT_SEL_LENGTH
 */
void L3G4200D::setDataFilter(uint8_t filter) {
	// Enable filter for HIGH and LOW_HIGH modes, disable for NON_HIGH and LOW
	if (filter == L3G4200D_HIGH_PASS || filter == L3G4200D_LOW_HIGH_PASS) {
		setHighPassFilterEnabled(true);
	} else {
		setHighPassFilterEnabled(false);
	}
	
	// Set data filter and set INT1 to use same filter when generating 
	// interrupts
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_OUT_SEL_BIT, 
		L3G4200D_OUT_SEL_LENGTH, filter);
	I2Cdev::writeBits(devAddr, L3G4200D_RA_CTRL_REG5, L3G4200D_INT1_SEL_BIT, 
		L3G4200D_INT1_SEL_LENGTH, filter);
}

/** Gets the data filter currently in use
 * @return the defined value that represents the filter in use
 * @see L3G4200D_RA_CTRL_REG5
 * @see L3G4200D_OUT_SEL_BIT
 * @see L3G4200D_OUT_SEL_LENGTH
 */
uint8_t L3G4200D::getDataFilter() {
	uint8_t outBits = I2Cdev::readBits(devAddr, L3G4200D_RA_CTRL_REG5, 
		L3G4200D_OUT_SEL_BIT, L3G4200D_OUT_SEL_LENGTH, buffer);
	
	// If the first bit is 0, the second is significant. Otherwise, more 
	// information ais required to determine the filter
	if (outBits == L3G4200D_NON_HIGH_PASS || outBits == L3G4200D_HIGH_PASS) {
		return outBits;
	}

	if (getHighPassFilterEnabled()) {
		return L3G4200D_HIGH_LOW_PASS;
	} else {
		return L3G4200D_LOW_PASS;
	}
}

// REFERENCE/DATACAPTURE register, r/w

/** Set the reference value for interrupt generation
 * @param reference The new reference value for interrupt generation
 * @see L3G4200D_RA_REFERENCE
 */
void L3G4200D::setInterruptReference(uint8_t reference) {
	I2Cdev::writeByte(devAddr, L3G4200D_RA_REFERENCE, reference);
}

/** Get the 8-bit reference value for interrupt generation
 * @return The 8-bit reference value for interrupt generation
 * @see L3G4200D_RA_REFERENCE
 */
uint8_t L3G4200D::getInterruptReference() {
	return I2Cdev::readByte(devAddr, L3G4200D_RA_REFERENCE, buffer);
}

// OUT_TEMP register, read-only

/** Gets the current temperature reading from the sensor
 * @return The current temperature
 * @see L3G4200D_RA_OUT_TEMP
 */
uint8_t L3G4200D::getTemperature() {
	return I2Cdev::readByte(devAddr, L3G4200D_OUT_TEMP, buffer);
}

// STATUS register, read-only

/** Get whether new data overwrote the last set of data before it was read
 * @return whether the last set of data was overwritten before being read
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZYXOR_BIT
 */
bool L3G4200D::getXYZOverrun() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZYXOR_BIT, 
		buffer);
}

/** Get whether new Z data overwrote the last set of data before it was read
 * @return whether the last set of Z data was overwritten before being read
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZOR_BIT
 */
bool L3G4200D::getZOverrun() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZOR_BIT, 
		buffer);
}

/** Get whether new Y data overwrote the last set of data before it was read
 * @return whether the last set of Y data was overwritten before being read
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_YOR_BIT
 */
bool L3G4200D::getYOverrun() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_YOR_BIT, 
		buffer);
}

/** Get whether new X data overwrote the last set of data before it was read
 * @return whether the last set of X data was overwritten before being read
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_XOR_BIT
 */
bool L3G4200D::getXOverrun() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_XOR_BIT, 
		buffer);
}

/** Get whether there is new data avaialable
 * @return whether there is new data available
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZYXDA_BIT
 */
bool L3G4200D::getXYZDataAvailable() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZYXDA_BIT, 
		buffer);
}

/** Get whether there is new Z data avaialable
 * @return whether there is new Z data available
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_ZDA_BIT
 */
bool L3G4200D::getZDataAvailable() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_ZDA_BIT, 
		buffer);
}

/** Get whether there is new Y data avaialable
 * @return whether there is new Y data available
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_YDA_BIT
 */
bool L3G4200D::getYDataAvailable() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_YDA_BIT, 
		buffer);
}

/** Get whether there is new X data avaialable
 * @return whether there is new X data available
 * @see L3G4200D_RA_STATUS
 * @see L3G4200D_XDA_BIT
 */
bool L3G4200D::getXDataAvailable() {
	return I2Cdev::readBit(devAddr, L3G4200D_RA_STATUS, L3G4200D_XDA_BIT, 
		buffer);
}

// OUT_* registers, read-only

/** Get the angular velocity for all 3 axes
 * @param x the 16-bit integer container for the X-axis angular velocity
 * @param y the 16-bit integer container for the Y-axis angular velocity
 * @param z the 16-bit integer container for the Z-axis angular velocity
 */
void L3G4200D::getAngularVelocity(int16_t* x, int16_t* y, int16_t* z) {
	I2Cdev::readBytes(devAddr, L3G4200D_OUT_X_L, 6, buffer);
	
	// Endianness decides which byte is the most significant
	if (getEndianMode() == L3G4200D_LITTLE_ENDIAN) {
		*x = (((int16_t)buffer[1]) << 8) | buffer[0];
		*y = (((int16_t)buffer[3]) << 8) | buffer[2];
		*z = (((int16_t)buffer[5]) << 8) | buffer[4];
	} else {
		*x = (((int16_t)buffer[0]) << 8) | buffer[1];
		*y = (((int16_t)buffer[2]) << 8) | buffer[3];
		*z = (((int16_t)buffer[4]) << 8) | buffer[5];
	}
}

/** Get the angular velocity about the X-axis
 * @return the angular velocity about the X-axis
 * @see L3G4200D_RA_OUT_X_L
 * @see L3G4200D_RA_OUT_X_H
 */
int16_t L3G4200D::getAngularVelocityX() {
	I2Cdev::readBytes(devAddr, L3G4200D_OUT_X_L, 2, buffer);
	
	// Endianness decides which byte is the most significant
	if (getEndianMode() == L3G4200D_LITTLE_ENDIAN) {
		return (((int16_t)buffer[1]) << 8) | buffer[0];
	} else {
		return (((int16_t)buffer[0]) << 8) | buffer[1];
	}
}
	
/** Get the angular velocity about the Y-axis
 * @return the angular velocity about the Y-axis
 * @see L3G4200D_RA_OUT_Y_L
 * @see L3G4200D_RA_OUT_Y_H
 */
int16_t L3G4200D::getAngularVelocityY() {
	I2Cdev::readBytes(devAddr, L3G4200D_OUT_Y_L, 2, buffer);
	
	// Endianness decides which byte is the most significant
	if (getEndianMode() == L3G4200D_LITTLE_ENDIAN) {
		return (((int16_t)buffer[1]) << 8) | buffer[0];
	} else {
		return (((int16_t)buffer[0]) << 8) | buffer[1];
	}
}

/** Get the angular velocity about the Z-axis
 * @return the angular velocity about the Z-axis
 * @see L3G4200D_RA_OUT_Z_L
 * @see L3G4200D_RA_OUT_Z_H
 */
int16_t L3G4200D::getAngularVelocityZ() {
	I2Cdev::readBytes(devAddr, L3G4200D_OUT_Z_L, 2, buffer);
	
	// Endianness decides which byte is the most significant
	if (getEndianMode() == L3G4200D_LITTLE_ENDIAN) {
		return (((int16_t)buffer[1]) << 8) | buffer[0];
	} else {
		return (((int16_t)buffer[0]) << 8) | buffer[1];
	}
}

// FIFO_CTRL register, r/w

/** Set the FIFO mode to one of the defined modes
 * @param mode The new FIFO mode
 * @see L3G4200D_RA_FIFO_CTRL
 * @see L3G4200D_MODE_BIT
 * @see L3G4200D_MODE_LENGTH
 * @see L3G4200D_FM_BYPASS
 * @see L3G4200D_FM_FIFO
 * @see L3G4200D_FM_STREAM
 * @see L3G4200D_FM_STREAM_FIFO
 * @see L3G4200D_FM_BYPASS_STREAM
 */
void L3G4200D::setFIFOMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3G4200D_RA_FIFO_CTRL, L3G4200D_MODE_BIT, 
		L3G4200D_MODE_LENGTH, mode);
}

/** Get the FIFO mode to one of the defined modes
 * @return the current FIFO mode
 * @see L3G4200D_RA_FIFO_CTRL
 * @see L3G4200D_MODE_BIT
 * @see L3G4200D_MODE_LENGTH
 * @see L3G4200D_FM_BYPASS
 * @see L3G4200D_FM_FIFO
 * @see L3G4200D_FM_STREAM
 * @see L3G4200D_FM_STREAM_FIFO
 * @see L3G4200D_FM_BYPASS_STREAM
 */
uint8_t L3G4200D::getFIFOMode() {
	return I2Cdev::readBits(devAddr, L3G4200D_RA_FIFO_CTRL, L3G4200D_MODE_BIT,
		L3G4200D_MODE_LENGTH, buffer);
}

























