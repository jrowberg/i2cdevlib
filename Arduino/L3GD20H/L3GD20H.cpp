// I2Cdev library collection - L3GD20H I2C device class
// Based on STMicroelectronics L3GD20H datasheet rev. 2, 3/2013
// 3/05/2015 by Nate Costello <natecostello at gmail dot com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2015-03-05 - initial release

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

#include "L3GD20H.h"

/** Default constructor, uses default I2C address.
 * @see L3GD20H_DEFAULT_ADDRESS
 */
L3GD20H::L3GD20H() {
    devAddr = L3GD20H_DEFAULT_ADDRESS;
    endianMode = 0;
}

/** Specific address constructor.
 * @param address I2C address
 * @see L3GD20H_DEFAULT_ADDRESS
 * @see L3GD20H_ADDRESS
 */
L3GD20H::L3GD20H(uint8_t address) {
    devAddr = address;
    endianMode = 0;
}

/** Power on and prepare for general usage.
 * All values are defaults except for the power on bit in CTRL_1
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_RA_CTRL2
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_RA_CTRL5
 */
void L3GD20H::initialize() {
	I2Cdev::writeByte(devAddr, L3GD20H_RA_CTRL1, 0b00001111);
    I2Cdev::writeByte(devAddr, L3GD20H_RA_CTRL2, 0b00000000);
    I2Cdev::writeByte(devAddr, L3GD20H_RA_CTRL3, 0b00000000);
    I2Cdev::writeByte(devAddr, L3GD20H_RA_CTRL4, 0b00000000);
    I2Cdev::writeByte(devAddr, L3GD20H_RA_CTRL5, 0b00000000);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool L3GD20H::testConnection() {
    return getDeviceID() == 0b11010111;
}

// WHO_AM_I register, read-only

/** Get the Device ID.
 * The WHO_AM_I register holds the device's id
 * @return Device ID (should be 0b11010011, 109, 0x69)
 * @see L3GD20H_RA_WHO_AM_I
 */
uint8_t L3GD20H::getDeviceID() {
    I2Cdev::readByte(devAddr, L3GD20H_RA_WHO_AM_I, buffer);
    return buffer[0];
}

// CTRL1 register, r/w

/** Set the output data rate.  Makes use of the setLowODREnabled function.
 * @param rate The new data output rate (can be 12, 25, 50, 100, 200, 400, or 800)
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_ODR_BIT
 * @see L3GD20H_ODR_LENGTH
 * @see L3GD20H_RATE_100_12
 * @see L3GD20H_RATE_200_25
 * @see L3GD20H_RATE_400_50
 * @see L3GD20H_RATE_800_50
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_LOW_ODR_BIT
 */
void L3GD20H::setOutputDataRate(uint16_t rate) {
	uint8_t writeVal;
	bool lowODRwriteVal;

	if (rate ==12) {
		writeVal = L3GD20H_RATE_100_12;
		setLowODREnabled(true);
	} else if (rate == 25) {
		writeVal = L3GD20H_RATE_200_25;
		setLowODREnabled(true);
	} else if (rate == 50) {
		writeVal = L3GD20H_RATE_400_50;
		setLowODREnabled(true);
	} else if (rate == 100) {
		writeVal = L3GD20H_RATE_100_12;
		setLowODREnabled(false);
	} else if (rate == 200) {
		writeVal = L3GD20H_RATE_200_25;
		setLowODREnabled(false);
	} else if (rate == 400) {
		writeVal = L3GD20H_RATE_400_50;
		setLowODREnabled(false);
	} else {
		writeVal = L3GD20H_RATE_800_50;
		setLowODREnabled(false);
	}
	
	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL1, L3GD20H_ODR_BIT,
		L3GD20H_ODR_LENGTH, writeVal); 
}

/** Get the current output data rate
 * @return Current data output rate
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_ODR_BIT
 * @see L3GD20H_ODR_LENGTH
 * @see L3GD20H_RATE_100_12
 * @see L3GD20H_RATE_200_25
 * @see L3GD20H_RATE_400_50
 * @see L3GD20H_RATE_800_50
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_LOW_ODR_BIT
 */
uint16_t L3GD20H::getOutputDataRate() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_CTRL1, L3GD20H_ODR_BIT, 
		L3GD20H_ODR_LENGTH, buffer);
	uint8_t rate = buffer[0];

	if (rate == L3GD20H_RATE_100_12) {
		if (getLowODREnabled() == true) {
			return 12;
		} else {
			return 100;	
		}
	} else if (rate == L3GD20H_RATE_200_25) {
		if (getLowODREnabled() == true) {
			return 25;
		} else {
			return 200;
		}
	} else if (rate == L3GD20H_RATE_400_50) {
		if (getLowODREnabled() == true) {
			return 50;
		} else {
			return 400;
		}
	} else if (rate == L3GD20H_RATE_800_50) {
		if (getLowODREnabled() == true) {
			return 50;
		} else {
			return 800;
		}
	}
	return 800;
}

/** Set the bandwidth cut-off mode
 * @param mode The new bandwidth cut-off mode
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_BW_BIT
 * @see L3GD20H_BW_LENGTH
 * @see L3GD20H_BW_LOW
 * @see L3GD20H_BW_MED_LOW
 * @see L3GD20H_BW_MED_HIGH
 * @see L3GD20H_BW_HIGH
 */
void L3GD20H::setBandwidthCutOffMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL1, L3GD20H_BW_BIT, 
		L3GD20H_BW_LENGTH, mode);
}

/** Get the current bandwidth cut-off mode
 * @return Current bandwidth cut off mode
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_BW_BIT
 * @see L3GD20H_BW_LENGTH
 * @see L3GD20H_BW_LOW
 * @see L3GD20H_BW_MED_LOW
 * @see L3GD20H_BW_MED_HIGH
 * @see L3GD20H_BW_HIGH
 */
uint8_t L3GD20H::getBandwidthCutOffMode() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_CTRL1, L3GD20H_BW_BIT, 
		L3GD20H_BW_LENGTH, buffer);
	return buffer[0];
}

// /** Gets the current bandwidth cutoff based on ODR and BW
//  * @return Float value of the bandwidth cut off
//  * @see L3GD20H_RA_CTRL1
//  * @see L3GD20H_ODR_BIT
//  * @see L3GD20H_ODR_LENGTH
//  * @see L3GD20H_RATE_100_12
//  * @see L3GD20H_RATE_200_25
//  * @see L3GD20H_RATE_400_50
//  * @see L3GD20H_RATE_800_50
//  * @see L3GD20H_BW_BIT
//  * @see L3GD20H_BW_LENGTH
//  * @see L3GD20H_BW_LOW
//  * @see L3GD20H_BW_MED_LOW
//  * @see L3GD20H_BW_MED_HIGH
//  * @see L3GD20H_BW_HIGH
//  */
// float L3GD20H::getBandwidthCutOff() {
// 	uint16_t dataRate = getOutputDataRate();
// 	uint8_t bandwidthMode = getBandwidthCutOffMode();

// 	if (dataRate == 50) {
// 		return 16.6;
// 	} else if (dataRate == 100) {
// 		if (bandwidthMode == L3GD20H_BW_LOW) {
// 			return 12.5;
// 		} else {
// 			return 25.0;
// 		}
// 	} else if (dataRate == 200) {
// 		if (bandwidthMode == L3GD20H_BW_LOW) {
// 			return 12.5;
// 		} else if (bandwidthMode == L3GD20H_BW_MED_LOW) {
// 			return 0.0;
// 		} else if (bandwidthMode == L3GD20H_BW_MED_HIGH) {
// 			return 0.0;
// 		} else {
// 			return 70.0;
// 		}
// 	} else if (dataRate == 400) {
// 		if (bandwidthMode == L3GD20H_BW_LOW) {
// 			return 20.0;
// 		} else if (bandwidthMode == L3GD20H_BW_MED_LOW) {
// 			return 25.0;
// 		} else if (bandwidthMode == L3GD20H_BW_MED_HIGH) {
// 			return 50.0;
// 		} else {
// 			return 110.0;
// 		}
// 	} else if (dataRate == 800) {
// 		if (bandwidthMode == L3GD20H_BW_LOW) {
// 			return 30.0;
// 		} else if (bandwidthMode == L3GD20H_BW_MED_LOW) {
// 			return 35.0;
// 		} else if (bandwidthMode == L3GD20H_BW_MED_HIGH) {
// 			return 0.0;
// 		} else {
// 			return 110.0;
// 		}
// 	} else {
// 		return 0.0
// 	}
// }

/** Set power on or off
 * @param enabled The new power setting (true for on, false for off)
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_PD_BIT
 */
void L3GD20H::setPowerOn(bool on) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_PD_BIT, on);
}

/** Get the current power state
 * @return Powered on state (true for on, false for off)
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_PD_BIT
 */
bool L3GD20H::getPowerOn() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_PD_BIT, buffer);
	return buffer[0];
}

/** Enables or disables the ability to get Z data
 * @param enabled The new enabled state of the Z axis
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_ZEN_BIT
 */
void L3GD20H::setZEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_ZEN_BIT, enabled);
}

/** Get whether Z axis data is enabled
 * @return True if the Z axis is enabled, false otherwise
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_ZEN_BIT
 */
bool L3GD20H::getZEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_ZEN_BIT, buffer);
	return buffer[0];
}

/** Enables or disables the ability to get Y data
 * @param enabled The new enabled state of the Y axis
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_YEN_BIT
 */
void L3GD20H::setYEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_YEN_BIT, enabled);
}

/** Get whether Y axis data is enabled
 * @return True if the Y axis is enabled, false otherwise
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_YEN_BIT
 */
bool L3GD20H::getYEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_YEN_BIT, buffer);
	return buffer[0];
}

/** Enables or disables the ability to get X data
 * @param enabled The new enabled state of the X axis
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_XEN_BIT
 */
void L3GD20H::setXEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_XEN_BIT, enabled);
}

/** Get whether X axis data is enabled
 * @return True if the X axis is enabled, false otherwise
 * @see L3GD20H_RA_CTRL1
 * @see L3GD20H_XEN_BIT
 */
bool L3GD20H::getXEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL1, L3GD20H_XEN_BIT, buffer);
	return buffer[0];
}

// CTRL2 register, r/w

/** Set the high pass mode
 * @param mode The new high pass mode
 * @see L3GD20H_RA_CTRL2
 * @see L3GD20H_HPM_BIT
 * @see L3GD20H_HPM_LENGTH
 * @see L3GD20H_HPM_HRF
 * @see L3GD20H_HPM_REFERENCE
 * @see L3GD20H_HPM_NORMAL
 * @see L3GD20H_HPM_AUTORESET
 */
void L3GD20H::setHighPassMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL2, L3GD20H_HPM_BIT, 
		L3GD20H_HPM_LENGTH, mode);
}

/** Get the high pass mode
 * @return High pass mode
 * @see L3GD20H_RA_CTRL2
 * @see L3GD20H_HPM_BIT
 * @see L3GD20H_HPM_LENGTH
 * @see L3GD20H_HPM_HRF
 * @see L3GD20H_HPM_REFERENCE
 * @see L3GD20H_HPM_NORMAL
 * @see L3GD20H_HPM_AUTORESET
 */
uint8_t L3GD20H::getHighPassMode() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_CTRL2, L3GD20H_HPM_BIT, 
		L3GD20H_HPM_LENGTH, buffer);
	return buffer[0];
}

/** Set the high pass filter cut off frequency level (1 - 10)
 * @param level The new level for the hpcf, using one of the defined levels
 * @see L3GD20H_RA_CTRL2
 * @see L3GD20H_HPCF_BIT
 * @see L3GD20H_HPCF_LENGTH
 * @see L3GD20H_HPCF1
 * @see L3GD20H_HPCF2
 * @see L3GD20H_HPCF3
 * @see L3GD20H_HPCF4
 * @see L3GD20H_HPCF5
 * @see L3GD20H_HPCF6
 * @see L3GD20H_HPCF7
 * @see L3GD20H_HPCF8
 * @see L3GD20H_HPCF9
 * @see L3GD20H_HPCF10
 */
void L3GD20H::setHighPassFilterCutOffFrequencyLevel(uint8_t level) {
	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL2, L3GD20H_HPCF_BIT, 
		L3GD20H_HPCF_LENGTH, level);
}

/** Get the high pass filter cut off frequency level (1 - 10)
 * @return High pass filter cut off frequency level
 * @see L3GD20H_RA_CTRL2
 * @see L3GD20H_HPCF_BIT
 * @see L3GD20H_HPCF_LENGTH
 * @see L3GD20H_HPCF1
 * @see L3GD20H_HPCF2
 * @see L3GD20H_HPCF3
 * @see L3GD20H_HPCF4
 * @see L3GD20H_HPCF5
 * @see L3GD20H_HPCF6
 * @see L3GD20H_HPCF7
 * @see L3GD20H_HPCF8
 * @see L3GD20H_HPCF9
 * @see L3GD20H_HPCF10
 */
uint8_t L3GD20H::getHighPassFilterCutOffFrequencyLevel() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_CTRL2, L3GD20H_HPCF_BIT, 
		L3GD20H_HPCF_LENGTH, buffer);
	return buffer[0];
}

// CTRL3 register, r/w

/** Set the INT1 interrupt enabled state
 * @param enabled New enabled state for the INT1 interrupt
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT1_IG_BIT
 */
void L3GD20H::setINT1InterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT1_IG_BIT, 
		enabled);
}

/** Get the INT1 interrupt enabled state
 * @return True if the INT1 interrupt is enabled, false otherwise
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT1_IG_BIT
 */
bool L3GD20H::getINT1InterruptEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT1_IG_BIT, 
		buffer);
	return buffer[0];
}

/** Set the INT1 boot status enabled state
 * @param enabled New enabled state for the INT1 boot status
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT1_BOOT_BIT
 */
void L3GD20H::setINT1BootStatusEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT1_BOOT_BIT, 
		enabled);
}

/** Get the INT1 boot status enabled state
 * @return INT1 boot status status
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT1_BOOT_BIT
 */
bool L3GD20H::getINT1BootStatusEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT1_BOOT_BIT, 
		buffer);
	return buffer[0];
}

/** Interrupts the active INT1 configuration
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_H_LACTIVE_BIT
 */
void L3GD20H::interruptActiveINT1Config() {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_H_LACTIVE_BIT, 1);
}

/** Set output mode to push-pull or open-drain
 * @param mode New output mode
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_PP_OD_BIT
 * @see L3GD20H_PUSH_PULL
 * @see L3GD20H_OPEN_DRAIN
 */
void L3GD20H::setOutputMode(bool mode) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_PP_OD_BIT, 
		mode);
}

/** Get whether mode is push-pull or open drain
 * @return Output mode (TRUE for open-drain, FALSE for push-pull)
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_PP_OD_BIT
 * @see L3GD20H_PUSH_PULL
 * @see L3GD20H_OPEN_DRAIN
 */
bool L3GD20H::getOutputMode() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_PP_OD_BIT, 
		buffer);
	return buffer[0];
}

/** Set data ready interrupt enabled state on INT2 pin
 * @param enabled New INT2 data ready interrupt enabled state
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT2_DRDY_BIT
 */
void L3GD20H::setINT2DataReadyEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_DRDY_BIT, 
		enabled);
}

/** Get whether the data ready interrupt is enabled on the INT2 pin
 * @return True if the INT2 data ready interrupt is enabled, false otherwise
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT2_DRDY_BIT
 */
bool L3GD20H::getINT2DataReadyEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_DRDY_BIT, 
		buffer);
	return buffer[0];
}

/** Set whether the INT2 FIFO threshold (watermark) interrupt is enabled
 * The sensor contains a 32-slot FIFO buffer for storing data so that it may be 
 * read later. If enabled, the sensor will generate an interrupt on the 
 * INT2/DRDY pin when the threshold has been reached. The threshold can be 
 * configured through the setFIFOWatermark function.
 * @param enabled New enabled state of the INT2 FIFO threshold (watermark)
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_I2_WTM_BIT
 */
void L3GD20H::setINT2FIFOWatermarkInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_FTH_BIT, 
		enabled);
}

/** Get the INT2 FIFO threshold (watermark) interrupt enabled state
 * @return true if the FIFO watermark is enabled, false otherwise
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT2_FTH_BIT
 */ 
bool L3GD20H::getINT2FIFOWatermarkInterruptEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_FTH_BIT, 
		buffer);
	return buffer[0];
}

/** Set whether an interrupt is triggered on INT2 when the FIFO is overrun
 * @param enabled New FIFO overrun interrupt enabled state
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT2_ORUN_BIT
 */
void L3GD20H::setINT2FIFOOverrunInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_ORUN_BIT, 
		enabled);
}

/** Get whether an interrupt is triggered on INT2 when the FIFO is overrun
 * @return True if the INT2 FIFO overrun interrupt is enabled, false otherwise
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT2_ORUN_BIT
 */
bool L3GD20H::getINT2FIFOOverrunInterruptEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_ORUN_BIT,
		buffer);
	return buffer[0];
}

/** Set whether an interrupt is triggered on INT2 when the FIFO buffer is empty
 * @param enabled New INT2 FIFO empty interrupt state
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT2_EMPTY_BIT
 */
void L3GD20H::setINT2FIFOEmptyInterruptEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_EMPTY_BIT, 
		enabled);
}

/** Get whether the INT2 FIFO empty interrupt is enabled
 * @returns True if the INT2 FIFO empty interrupt is enabled, false otherwise
 * @see L3GD20H_RA_CTRL3
 * @see L3GD20H_INT2_EMPTY_BIT
 */
bool L3GD20H::getINT2FIFOEmptyInterruptEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL3, L3GD20H_INT2_EMPTY_BIT, 
		buffer);
	return buffer[0];
}

// CTRL4 register, r/w

/** Set the Block Data Update (BDU) enabled state
 * @param enabled New BDU enabled state
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_BDU_BIT
 */
void L3GD20H::setBlockDataUpdateEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL4, L3GD20H_BDU_BIT, enabled);
}

/** Get the BDU enabled state
 * @return True if Block Data Update is enabled, false otherwise
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_BDU_BIT
 */
bool L3GD20H::getBlockDataUpdateEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL4, L3GD20H_BDU_BIT, buffer);
	return buffer[0];
}

/** Set the data endian modes
 * In Big Endian mode, the Most Significat Byte (MSB) is on the lower address, 
 * and the Least Significant Byte (LSB) is on the higher address. Little Endian 
 * mode reverses this order. Little Endian is the default mode.
 * @param endianness New endian mode
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_BLE_BIT
 * @see L3GD20H_BIG_ENDIAN
 * @see L3GD20H_LITTLE_ENDIAN
 */
void L3GD20H::setEndianMode(bool endianness) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL4, L3GD20H_BLE_BIT, 
		endianness);
	endianMode = getEndianMode();
}

/** Get the data endian mode
 * @return Current endian mode
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_BLE_BIT
 * @see L3GD20H_BIG_ENDIAN
 * @see L3GD20H_LITTLE_ENDIAN
 */
bool L3GD20H::getEndianMode() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL4, L3GD20H_BLE_BIT,
		buffer);
	return buffer[0];
}

/** Set the full scale of the data output (in dps)
 * @param scale The new scale of the data output (250 [actual 245], 500, 2000)
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_FS_BIT
 * @see L3GD20H_FS_LENGTH
 * @see L3GD20H_FS_250
 * @see L3GD20H_FS_500
 * @see L3GD20H_FS_2000
 */
void L3GD20H::setFullScale(uint16_t scale) {
	uint8_t writeBits;
	
	if (scale == 250) {
		writeBits = L3GD20H_FS_250;
	} else if (scale == 500) {
		writeBits = L3GD20H_FS_500;
	} else {
		writeBits = L3GD20H_FS_2000;
	}

	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL4, L3GD20H_FS_BIT, 
		L3GD20H_FS_LENGTH, writeBits);
}

/** Get the current full scale of the output data (in dps)
 * @return Current scale of the output data
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_FS_BIT
 * @see L3GD20H_FS_LENGTH
 * @see L3GD20H_FS_250
 * @see L3GD20H_FS_500
 * @see L3GD20H_FS_2000
 */
uint16_t L3GD20H::getFullScale() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_CTRL4, 
		L3GD20H_FS_BIT, L3GD20H_FS_LENGTH, buffer);
	uint8_t readBits = buffer[0];
	
	if (readBits == L3GD20H_FS_250) {
		return 250;
	} else if (readBits == L3GD20H_FS_500) {
		return 500;
	} else {
		return 2000;
	}
}

//TODO
//Implement 
//L3GD20H::setLevelSensitiveLatchedEnabled() and
//L3GD20H::getLevelSensitiveLatchedEnabled()
//

/** Set the self test mode
 * @param mode New self test mode (Normal, 0, 1)
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_ST_BIT
 * @see L3GD20H_ST_LENGTH
 * @see L3GD20H_SELF_TEST_NORMAL
 * @see L3GD20H_SELF_TEST_0
 * @see L3GD20H_SELF_TEST_1
 */
void L3GD20H::setSelfTestMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL4, L3GD20H_ST_BIT, 
		L3GD20H_ST_LENGTH, mode);
}

/** Get the current self test mode
 * @return Current self test mode
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_ST_BIT
 * @see L3GD20H_ST_LENGTH
 * @see L3GD20H_SELF_TEST_NORMAL
 * @see L3GD20H_SELF_TEST_0
 * @see L3GD20H_SELF_TEST_1
 */
uint8_t L3GD20H::getSelfTestMode() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_CTRL4, L3GD20H_ST_BIT, 
		L3GD20H_ST_LENGTH, buffer);
	return buffer[0];
}

/** Set the SPI mode
 * @param mode New SPI mode
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_SIM_BIT
 * @see L3GD20H_SPI_4_WIRE
 * @see L3GD20H_SPI_3_WIRE
 */
void L3GD20H::setSPIMode(bool mode) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL4, L3GD20H_SIM_BIT, mode);
}

/** Get the SPI mode
 * @return Current SPI mode
 * @see L3GD20H_RA_CTRL4
 * @see L3GD20H_SIM_BIT
 * @see L3GD20H_SPI_4_WIRE
 * @see L3GD20H_SPI_3_WIRE
 */
bool L3GD20H::getSPIMode() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL4, L3GD20H_SIM_BIT, 
		buffer);
 	return buffer[0];
}

// CTRL5 register, r/w

/** Reboots the FIFO memory content
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_BOOT_BIT
 */
void L3GD20H::rebootMemoryContent() {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL5, L3GD20H_BOOT_BIT, true);
}

/** Set whether the FIFO buffer is enabled
 * @param enabled New enabled state of the FIFO buffer
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_FIFO_EN_BIT
 */
void L3GD20H::setFIFOEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL5, L3GD20H_FIFO_EN_BIT, 
		enabled);
}

/** Get whether the FIFO buffer is enabled
 * @return True if the FIFO buffer is enabled, false otherwise
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_FIFO_EN_BIT
 */
bool L3GD20H::getFIFOEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL5, L3GD20H_FIFO_EN_BIT, 
		buffer);
	return buffer[0];
}

/** Set whether the sensing chain FIFO stops writing new values once
 * the FIFO Threshold (watermark) is reached
 * @param enabled New state of the StopOnFTH bit
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_FIFO_STOPONFTH_BIT
 */
void L3GD20H::setStopOnFIFOThresholdEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL5, L3GD20H_STOPONFTH_BIT, 
		enabled);
}

/** Get whether the sensing chain FIFO stopping writing new values once
 * the FIFO Threshold (watermark) is enabled
 * @return True if the state of the StopOnFTH bit is high (enabled)
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_FIFO_STOPONFTH_BIT
 */
bool L3GD20H::getStopOnFIFOThresholdEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL5, L3GD20H_STOPONFTH_BIT, 
		buffer);
	return buffer[0];
}


/** Set the high pass filter enabled state
 * @param enabled New high pass filter enabled state
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_HPEN_BIT
 */
void L3GD20H::setHighPassFilterEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_CTRL5, L3GD20H_HPEN_BIT, 
		enabled);
}

/** Get whether the high pass filter is enabled
 * @return True if the high pass filter is enabled, false otherwise
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_HPEN_BIT
 */
bool L3GD20H::getHighPassFilterEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_CTRL5, L3GD20H_HPEN_BIT,
		buffer);
	return buffer[0];
}

/** Sets the filter mode to one of the four provided.
 * This function also uses the setHighPassFilterEnabled function in order to set
 * the mode. That function does not have to be called in addition to this one. 
 * In addition to setting the filter for the data in the FIFO buffer 
 * (controlled by the bits written to OUT_SEL), this function also sets the
 * filter used for interrupt generation (the bits written to IG_SEL) to be the
 * same as the filter used for the FIFO buffer.  The filter used for interrupt
 * generation can be set separately with the setInterruptFilter function.
 * @param filter New method to be used when filtering data
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_IG_SEL_BIT
 * @see L3GD20H_IG_SEL_LENGTH
 * @see L3GD20H_OUT_SEL_BIT
 * @see L3GD20H_OUT_SEL_LENGTH
 * @see L3GD20H_NON_HIGH_PASS
 * @see L3GD20H_HIGH_PASS
 * @see L3GD20H_LOW_PASS
 * @see L3GD20H_LOW_HIGH_PASS
 */
void L3GD20H::setDataFilter(uint8_t filter) {
	if (filter == L3GD20H_HIGH_PASS || filter == L3GD20H_LOW_HIGH_PASS) {
		setHighPassFilterEnabled(true);
	} else {
		setHighPassFilterEnabled(false);
	}
	
	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL5, L3GD20H_OUT_SEL_BIT, 
		L3GD20H_OUT_SEL_LENGTH, filter);
	I2Cdev::writeBits(devAddr, L3GD20H_RA_CTRL5, L3GD20H_IG_SEL_BIT, 
		L3GD20H_IG_SEL_LENGTH, filter);
}

/** Gets the data filter currently in use
 * @return Defined value that represents the filter in use
 * @see L3GD20H_RA_CTRL5
 * @see L3GD20H_OUT_SEL_BIT
 * @see L3GD20H_OUT_SEL_LENGTH
 * @see L3GD20H_NON_HIGH_PASS
 * @see L3GD20H_HIGH_PASS
 * @see L3GD20H_LOW_PASS
 * @see L3GD20H_LOW_HIGH_PASS
 */
uint8_t L3GD20H::getDataFilter() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_CTRL5, L3GD20H_OUT_SEL_BIT, 
		L3GD20H_OUT_SEL_LENGTH, buffer);
	uint8_t outBits = buffer[0];

	if (outBits == L3GD20H_NON_HIGH_PASS || outBits == L3GD20H_HIGH_PASS) {
		return outBits;
	}

	if (getHighPassFilterEnabled()) {
		return L3GD20H_LOW_HIGH_PASS;
	} else {
		return L3GD20H_LOW_PASS;
	}
}

//ToDo:
//setInterruptFilter
//getInterruptFilter

// REFERENCE register, r/w

/** Set the reference value for the high pass filter
 * @param reference New 8-bit digital high pass filter reference value
 * @see L3GD20H_RA_REFERENCE
 */
void L3GD20H::setHighPassFilterReference(uint8_t reference) {
	I2Cdev::writeByte(devAddr, L3GD20H_RA_REFERENCE, reference);
}

/** Get the 8-bit reference value for the high pass filter
 * @return 8-bit reference value for the high pass filter
 * @see L3GD20H_RA_REFERENCE
 */
uint8_t L3GD20H::getHighPassFilterReference() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_REFERENCE, buffer);
	return buffer[0];
}

// void L3GD20H::setInterruptReference(uint8_t reference) {
// 	I2Cdev::writeByte(devAddr, L3GD20H_RA_REFERENCE, reference);
// }

// uint8_t L3GD20H::getInterruptReference() {
// 	I2Cdev::readByte(devAddr, L3GD20H_RA_REFERENCE, buffer);
// 	return buffer[0];
// }

// OUT_TEMP register, read-only

/** Gets the current temperature reading from the sensor
 * @return Current temperature
 * @see L3GD20H_RA_OUT_TEMP
 */
uint8_t L3GD20H::getTemperature() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_OUT_TEMP, buffer);
	return buffer[0];
}

// STATUS register, read-only

/** Get whether new data overwrote the last set of data before it was read
 * @return True if the last set of data was overwritten before being read, false
 * otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_ZYXOR_BIT
 */
bool L3GD20H::getXYZOverrun() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_ZYXOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether new Z data overwrote the last set of data before it was read
 * @return True if the last set of Z data was overwritten before being read,
 * false otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_ZOR_BIT
 */
bool L3GD20H::getZOverrun() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_ZOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether new Y data overwrote the last set of data before it was read
 * @return True if the last set of Y data was overwritten before being read, 
 * false otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_YOR_BIT
 */
bool L3GD20H::getYOverrun() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_YOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether new X data overwrote the last set of data before it was read
 * @return True if the last set of X data was overwritten before being read, 
 * false otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_XOR_BIT
 */
bool L3GD20H::getXOverrun() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_XOR_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new data avaialable
 * @return True if there is new data available, false otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_ZYXDA_BIT
 */
bool L3GD20H::getXYZDataAvailable() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_ZYXDA_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new Z data avaialable
 * @return True if there is new Z data available, false otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_ZDA_BIT
 */
bool L3GD20H::getZDataAvailable() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_ZDA_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new Y data avaialable
 * @return True if there is new Y data available, false otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_YDA_BIT
 */
bool L3GD20H::getYDataAvailable() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_YDA_BIT, 
		buffer);
	return buffer[0];
}

/** Get whether there is new X data avaialable
 * @return True if there is new X data available, false otherwise
 * @see L3GD20H_RA_STATUS
 * @see L3GD20H_XDA_BIT
 */
bool L3GD20H::getXDataAvailable() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_STATUS, L3GD20H_XDA_BIT, 
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
void L3GD20H::getAngularVelocity(int16_t* x, int16_t* y, int16_t* z) {
	I2Cdev::readBytes(devAddr, L3GD20H_RA_OUT_X_L | 0x80, 6, buffer);
	if (endianMode == L3GD20H_LITTLE_ENDIAN) {
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
 * @return Angular velocity about the X-axis
 * @see L3GD20H_RA_OUT_X_L
 * @see L3GD20H_RA_OUT_X_H
 */
int16_t L3GD20H::getAngularVelocityX() {
	I2Cdev::readBytes(devAddr, L3GD20H_RA_OUT_X_L | 0x80, 2, buffer);
	if (endianMode == L3GD20H_LITTLE_ENDIAN) {
		return (((int16_t)buffer[1]) << 8) | buffer[0];
	} else {
		return (((int16_t)buffer[0]) << 8) | buffer[1];
	}
}
	
/** Get the angular velocity about the Y-axis
 * @return Angular velocity about the Y-axis
 * @see L3GD20H_RA_OUT_Y_L
 * @see L3GD20H_RA_OUT_Y_H
 */
int16_t L3GD20H::getAngularVelocityY() {
	I2Cdev::readBytes(devAddr, L3GD20H_RA_OUT_Y_L | 0x80, 2, buffer);
	if (endianMode == L3GD20H_LITTLE_ENDIAN) {
		return (((int16_t)buffer[1]) << 8) | buffer[0];
	} else {
		return (((int16_t)buffer[0]) << 8) | buffer[1];
	}
}

/** Get the angular velocity about the Z-axis
 * @return Angular velocity about the Z-axis
 * @see L3GD20H_RA_OUT_Z_L
 * @see L3GD20H_RA_OUT_Z_H
 */
int16_t L3GD20H::getAngularVelocityZ() {
	I2Cdev::readBytes(devAddr, L3GD20H_RA_OUT_Z_L | 0x80, 2, buffer);
	if (endianMode == L3GD20H_LITTLE_ENDIAN) {
		return (((int16_t)buffer[1]) << 8) | buffer[0];
	} else {
		return (((int16_t)buffer[0]) << 8) | buffer[1];
	}
}

// FIFO_CTRL register, r/w

/** Set the FIFO mode to one of the defined modes
 * @param mode New FIFO mode
 * @see L3GD20H_RA_FIFO_CTRL
 * @see L3GD20H_FIFO_MODE_BIT
 * @see L3GD20H_FIFO_MODE_LENGTH
 * @see L3GD20H_FM_BYPASS
 * @see L3GD20H_FM_FIFO
 * @see L3GD20H_FM_STREAM
 * @see L3GD20H_FM_STREAM_FIFO
 * @see L3GD20H_FM_BYPASS_STREAM
 * @see L3GD20H_FM_DYNAMIC_STREAM
 * @see L3GD20H_FM_BYPASS_FIFO
 */
void L3GD20H::setFIFOMode(uint8_t mode) {
	I2Cdev::writeBits(devAddr, L3GD20H_RA_FIFO_CTRL, L3GD20H_FIFO_MODE_BIT, 
		L3GD20H_FIFO_MODE_LENGTH, mode);
}

/** Get the FIFO mode to one of the defined modes
 * @return Current FIFO mode
 * @see L3GD20H_RA_FIFO_CTRL
 * @see L3GD20H_FIFO_MODE_BIT
 * @see L3GD20H_FIFO_MODE_LENGTH
 * @see L3GD20H_FM_BYPASS
 * @see L3GD20H_FM_FIFO
 * @see L3GD20H_FM_STREAM
 * @see L3GD20H_FM_STREAM_FIFO
 * @see L3GD20H_FM_BYPASS_STREAM
 * @see L3GD20H_FM_DYNAMIC_STREAM
 * @see L3GD20H_FM_BYPASS_FIFO
 */
uint8_t L3GD20H::getFIFOMode() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_FIFO_CTRL, 
		L3GD20H_FIFO_MODE_BIT, L3GD20H_FIFO_MODE_LENGTH, buffer);
	return buffer[0];
}

/** Set the 5-bit FIFO (watermark) threshold
 * @param fth New 5-bit FIFO (watermark) threshold
 * @see L3GD20H_RA_FIFO_CTRL
 * @see L3GD20H_FIFO_TH_BIT
 * @see L3GD20H_FIFO_TH_LENGTH
 */
void L3GD20H::setFIFOThreshold(uint8_t fth) {
    I2Cdev::writeBits(devAddr, L3GD20H_RA_FIFO_CTRL, L3GD20H_FIFO_TH_BIT, 
        L3GD20H_FIFO_TH_LENGTH, fth);
}

/** Get the FIFO watermark threshold
 * @return FIFO watermark threshold
 * @see L3GD20H_RA_FIFO_CTRL
 * @see L3GD20H_FIFO_TH_BIT
 * @see L3GD20H_FIFO_TH_LENGTH
 */
uint8_t L3GD20H::getFIFOThreshold() {
    I2Cdev::readBits(devAddr, L3GD20H_RA_FIFO_CTRL, L3GD20H_FIFO_TH_BIT,
        L3GD20H_FIFO_TH_LENGTH, buffer);
    return buffer[0];
}

// FIFO_SRC register, read-only

/** Get whether the number of data sets in the FIFO buffer is less than the 
 * watermark
 * @return True if the number of data sets in the FIFO buffer is more than or 
 * equal to the watermark, false otherwise.
 * @see L3GD20H_RA_FIFO_SRC
 * @see L3GD20H_FIFO_TH_STATUS_BIT
 */
bool L3GD20H::getFIFOAtWatermark() {
   	I2Cdev::readBit(devAddr, L3GD20H_RA_FIFO_SRC, L3GD20H_FIFO_TH_STATUS_BIT, 
        buffer);
   	return buffer[0];
}

/** Get whether the FIFO buffer is full
 * @return True if the FIFO buffer is full, false otherwise
 * @see L3GD20H_RA_FIFO_SRC
 * @see L3GD20H_FIFO_OVRN_BIT
 */
bool L3GD20H::getFIFOOverrun() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_FIFO_SRC, 
        L3GD20H_OVRN_BIT, buffer);
    return buffer[0];
}

/** Get whether the FIFO buffer is empty
 * @return True if the FIFO buffer is empty, false otherwise
 * @see L3GD20H_RA_FIFO_SRC
 * @see L3GD20H_FIFO_EMPTY_BIT
 */
bool L3GD20H::getFIFOEmpty() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_FIFO_SRC,
        L3GD20H_EMPTY_BIT, buffer);
    return buffer[0];
}

/** Get the number of filled FIFO buffer slots
 * @return Number of filled slots in the FIFO buffer
 * @see L3GD20H_RA_FIFO_SRC
 * @see L3GD20H_FIFO_FSS_BIT
 * @see L3GD20H_FIFO_FSS_LENGTH
 */ 
uint8_t L3GD20H::getFIFOStoredDataLevel() {
    I2Cdev::readBits(devAddr, L3GD20H_RA_FIFO_SRC, 
        L3GD20H_FIFO_FSS_BIT, L3GD20H_FIFO_FSS_LENGTH, buffer);
    return buffer[0];
}

// IG_CFG register, r/w

/** Set the combination mode for interrupt events
 * @param combination New combination mode for interrupt events. 
 * L3GD20H_AND_OR_OR for OR and L3GD20H_AND_OR_AND for AND
 * @see L3GD20H_RA_IG_CFG
 * @see L3GD20H_AND_OR_BIT
 * @see L3GD20H_AND_OR_OR
 * @see L3GD20H_AND_OR_AND
 */
void L3GD20H::setInterruptCombination(bool combination) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_AND_OR_BIT,
        combination);
}

/** Get the combination mode for interrupt events
 * @return Combination mode for interrupt events. L3GD20H_AND_OR_OR for OR and 
 * L3GD20H_AND_OR_AND for AND
 * @see L3GD20H_RA_IG_CFG
 * @see L3GD20H_AND_OR_BIT
 * @see L3GD20H_AND_OR_OR
 * @see L3GD20H_AND_OR_AND
 */
bool L3GD20H::getInterruptCombination() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_AND_OR_BIT,
        buffer);
    return buffer[0];
}

/** Set whether an interrupt request is latched
 * This bit is cleared when the IG_SRC register is read
 * @param latched New status of the latched request
 * @see L3GD20H_RA_IG_CFG
 * @see L3GD20H_LIR_BIT
 */
void L3GD20H::setInterruptRequestLatched(bool latched) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_LIR_BIT, latched);
}

/** Get whether an interrupt request is latched
 * @return True if an interrupt request is latched, false otherwise
 * @see L3GD20H_RA_IG_CFG
 * @see L3GD20H_LIR_BIT
 */
bool L3GD20H::getInterruptRequestLatched() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_LIR_BIT, 
        buffer); 
    return buffer[0];
};

/** Set whether the interrupt for Z high is enabled
 * @param enabled New enabled state for Z high interrupt.
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_ZHIE_BIT
 */
void L3GD20H::setZHighInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_ZHIE_BIT, enabled);
}

/** Get whether the interrupt for Z high is enabled
 * @return True if the interrupt for Z high is enabled, false otherwise 
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_ZHIE_BIT
 */
bool L3GD20H::getZHighInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_ZHIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for Z low is enabled
 * @param enabled New enabled state for Z low interrupt.
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_ZLIE_BIT
 */
void L3GD20H::setZLowInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_ZLIE_BIT, enabled);
}

/** Get whether the interrupt for Z low is enabled
 * @return True if the interrupt for Z low is enabled, false otherwise
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_ZLIE_BIT
 */
bool L3GD20H::getZLowInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_ZLIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for Y high is enabled
 * @param enabled New enabled state for Y high interrupt.
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_YHIE_BIT
 */
void L3GD20H::setYHighInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_YHIE_BIT, enabled);
}

/** Get whether the interrupt for Y high is enabled
 * @return True if the interrupt for Y high is enabled, false otherwise
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_YHIE_BIT
 */
bool L3GD20H::getYHighInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_YHIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for Y low is enabled
 * @param enabled New enabled state for Y low interrupt.
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_YLIE_BIT
 */
void L3GD20H::setYLowInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_YLIE_BIT, enabled);
}

/** Get whether the interrupt for Y low is enabled
 * @return True if the interrupt for Y low is enabled, false otherwise 
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_YLIE_BIT
 */
bool L3GD20H::getYLowInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_YLIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for X high is enabled
 * @param enabled New enabled state for X high interrupt.
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_XHIE_BIT
 */
void L3GD20H::setXHighInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_XHIE_BIT, enabled);
}

/** Get whether the interrupt for X high is enabled
 * @return True if the interrupt for X high is enabled, false otherwise
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_XHIE_BIT
 */
bool L3GD20H::getXHighInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_XHIE_BIT, 
        buffer);
    return buffer[0];
}

/** Set whether the interrupt for X low is enabled
 * @param enabled New enabled state for X low interrupt.
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_XLIE_BIT
 */
void L3GD20H::setXLowInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_XLIE_BIT, enabled);
}

/** Get whether the interrupt for X low is enabled
 * @return True if the interrupt for X low is enabled, false otherwise
 * @see L3GD20H_IG_CFG
 * @see L3GD20H_XLIE_BIT
 */
bool L3GD20H::getXLowInterruptEnabled() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_CFG, L3GD20H_XLIE_BIT, 
        buffer);
    return buffer[0];
}

// IG_SRC register, read-only

/** Get whether an interrupt has been generated
 * @return True if one or more interrupts has been generated, false otherwise
 * @see L3GD20H_RA_IG_SRC
 * @see L3GD20H_IA_BIT
 */
bool L3GD20H::getInterruptActive() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_SRC, L3GD20H_IA_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Z high event has occurred
 * @return True if a Z high event has occurred, false otherwise
 * @see L3GD20H_RA_IG_SRC
 * @see L3GD20H_ZH_BIT
 */
bool L3GD20H::getZHigh() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_SRC, L3GD20H_ZH_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Z low event has occurred
 * @return True if a Z low event has occurred, false otherwise
 * @see L3GD20H_RA_IG_SRC
 * @see L3GD20H_ZL_BIT
 */
bool L3GD20H::getZLow() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_SRC, L3GD20H_ZL_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Y high event has occurred
 * @return True if a Y high event has occurred, false otherwise
 * @see L3GD20H_RA_IG_SRC
 * @see L3GD20H_YH_BIT
 */
bool L3GD20H::getYHigh() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_SRC, L3GD20H_YH_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a Y low event has occurred
 * @return True if a Y low event has occurred, false otherwise
 * @see L3GD20H_RA_IG_SRC
 * @see L3GD20H_YL_BIT
 */
bool L3GD20H::getYLow() {
   	I2Cdev::readBit(devAddr, L3GD20H_RA_IG_SRC, L3GD20H_YL_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a X high event has occurred
 * @return True if a X high event has occurred, false otherwise
 * @see L3GD20H_RA_IG_SRC
 * @see L3GD20H_XH_BIT
 */
bool L3GD20H::getXHigh() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_SRC, L3GD20H_XH_BIT,
        buffer);
    return buffer[0];
}

/** Get whether a X low event has occurred
 * @return True if a X low event has occurred, false otherwise
 * @see L3GD20H_RA_IG_SRC
 * @see L3GD20H_XL_BIT
 */
bool L3GD20H::getXLow() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_SRC, L3GD20H_XL_BIT,
        buffer);
    return buffer[0];
}

// IG_THS_* registers, r/w

/** Set the interrupt generation counter mode selection.
 * @param enabled New enabled state for X low interrupt.
 * @see L3GD20H_IG_THS_XH
 * @see L3GD20H_DCRM_BIT
 * @see L3GD20H_DCRM_RESET
 * @see L3GD20H_DCRM_DEC
 */
void L3GD20H::setDecrementMode(bool mode) {
    I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_THS_XH, L3GD20H_DCRM_BIT, mode);
}

/** Get the interrupt generation counter mode selection.
 * @return Mode Interrupt generation counter mode
 * @see L3GD20H_IG_THS_XH
 * @see L3GD20H_DCRM_BIT
 * @see L3GD20H_DCRM_RESET
 * @see L3GD20H_DCRM_DEC
 */
bool L3GD20H::getDecrementMode() {
    I2Cdev::readBit(devAddr, L3GD20H_RA_IG_THS_XH, L3GD20H_DCRM_BIT, 
        buffer);
    return buffer[0];
}

/** Set the 7-bit threshold for a high interrupt on the X axis
 * @param threshold New 7-bit threshold for a high interrupt on the X axis
 * @see L3GD20H_IG_THS_XH
 */
void L3GD20H::setXHighThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3GD20H_RA_IG_THS_XH, threshold);
}

/** Retrieve the threshold for a high interrupt on the X axis
 * @return X high interrupt threshold
 * @see L3GD20H_IG_THS_XH
 */
uint8_t L3GD20H::getXHighThreshold() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_IG_THS_XH, buffer);
	return buffer[0];
}

/** Set the 8-bit threshold for a low interrupt on the X axis
 * @param threshold New 8-bit threshold for a low interrupt on the X axis
 * @see L3GD20H_IG_THS_XL
 */
void L3GD20H::setXLowThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3GD20H_RA_IG_THS_XL, threshold);
}

/** Retrieve the threshold for a low interrupt on the X axis
 * @return X low interrupt threshold
 * @see L3GD20H_IG_THS_XL
 */
uint8_t L3GD20H::getXLowThreshold() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_IG_THS_XL, buffer);
	return buffer[0];
}

/** Set the 7-bit threshold for a high interrupt on the Y axis
 * @param threshold New 7-bit threshold for a high interrupt on the Y axis
 * @see L3GD20H_IG_THS_YH
 */
void L3GD20H::setYHighThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3GD20H_RA_IG_THS_YH, threshold);
}

/** Retrieve the threshold for a high interrupt on the Y axis
 * @return Y high interrupt threshold
 * @see L3GD20H_IG_THS_YH
 */
uint8_t L3GD20H::getYHighThreshold() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_IG_THS_YH, buffer);
	return buffer[0];
}

/** Set the 8-bit threshold for a low interrupt on the Y axis
 * @param threshold New 8-bit threshold for a low interrupt on the Y axis
 * @see L3GD20H_IG_THS_YL
 */
void L3GD20H::setYLowThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3GD20H_RA_IG_THS_YL, threshold);
}

/** Retrieve the threshold for a low interrupt on the Y axis
 * @return Y low interrupt threshold
 * @see L3GD20H_IG_THS_YL
 */
uint8_t L3GD20H::getYLowThreshold() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_IG_THS_YL, buffer);
	return buffer[0];
}

/** Set the 7-bit threshold for a high interrupt on the Z axis
 * @param threshold New 7-bit threshold for a high interrupt on the Z axis
 * @see L3GD20H_IG_THS_ZH
 */
void L3GD20H::setZHighThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3GD20H_RA_IG_THS_ZH, threshold);
}

/** Retrieve the threshold for a high interrupt on the Z axis
 * @return Z high interrupt threshold
 * @see L3GD20H_IG_THS_ZH
 */
uint8_t L3GD20H::getZHighThreshold() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_IG_THS_ZH, buffer);
	return buffer[0];
}

/** Set the 8-bit threshold for a low interrupt on the Z axis
 * @param threshold New 8-bit threshold for a low interrupt on the Z axis
 * @see L3GD20H_RA_IG_THS_ZL
 */
void L3GD20H::setZLowThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, L3GD20H_RA_IG_THS_ZL, threshold);
}

/** Retrieve the threshold for a low interrupt on the Z axis
 * @return Z low interrupt threshold
 * @see L3GD20H_IG_THS_ZL
 */
uint8_t L3GD20H::getZLowThreshold() {
	I2Cdev::readByte(devAddr, L3GD20H_RA_IG_THS_ZL, buffer);
	return buffer[0];
}

// IG_DURATION register, r/w

/* Set the minimum duration for an interrupt event to be recognized
 * This depends on the chosen output data rate
 * @param duration New 7-bit duration value necessary for an interrupt event to be 
 * recognized
 * @see L3GD20H_RA_IG_DURATION
 * @see L3GD20H_DUR_BIT
 * @see L3GD20H_DUR_LENGTH
 */
void L3GD20H::setDuration(uint8_t duration) {
	I2Cdev::writeBits(devAddr, L3GD20H_RA_IG_DURATION, L3GD20H_DUR_BIT,
		L3GD20H_DUR_LENGTH, duration);
}

/** Get the minimum duration for an interrupt event to be recognized
 * @return Duration value necessary for an interrupt event to be recognized
 * @see L3GD20H_RA_IG_DURATION
 * @see L3GD20H_DUR_BIT
 * @see L3GD20H_DUR_LENGTH
 */
uint8_t L3GD20H::getDuration() {
	I2Cdev::readBits(devAddr, L3GD20H_RA_IG_DURATION, 
		L3GD20H_DUR_BIT, L3GD20H_DUR_LENGTH, buffer);
	return buffer[0];
}

/** Set whether the interrupt wait feature is enabled
 * If false, the interrupt falls immediately if signal crosses the selected 
 * threshold. Otherwise, if signal crosses the selected threshold, the interrupt
 * falls only after the duration has counted number of samples at the selected 
 * data rate, written into the duration counter register.
 * @param enabled New enabled state of the interrupt wait
 * @see L3GD20H_RA_IG_DURATION
 * @see L3GD20H_WAIT_BIT
 */
void L3GD20H::setWaitEnabled(bool enabled) {
	I2Cdev::writeBit(devAddr, L3GD20H_RA_IG_DURATION, L3GD20H_WAIT_BIT,
		enabled);
}

/** Get whether the interrupt wait feature is enabled
 * @return True if the wait feature is enabled, false otherwise
 * @see L3GD20H_RA_IG_DURATION
 * @see L3GD20H_WAIT_BIT
 */
bool L3GD20H::getWaitEnabled() {
	I2Cdev::readBit(devAddr, L3GD20H_RA_IG_DURATION, 
		L3GD20H_WAIT_BIT, buffer);
	return buffer[0];
}

// LOW_ODR register, r/w

/** Set whether the DRDY/INT2 pin is active low. If enabled is true then the  
 * DRDY/INT2 pin will be active low.
 * @param enabled New enabled DRDY/INT2 active low configuration
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_DRDY_HL_BIT
 */
void L3GD20H::setINT2DataReadyActiveLowEnabled(bool enabled){
	I2Cdev::writeBit(devAddr, L3GD20H_RA_LOW_ODR, L3GD20H_DRDY_HL_BIT,
		enabled);
}

/** Get whether the DRDY/INT2 pin is active low. If true then the  
 * DRDY/INT2 pin IS active low.
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_I2C_DIS_BIT
 */
bool L3GD20H::getINT2DataReadyActiveLowEnabled(){
	I2Cdev::readBit(devAddr, L3GD20H_RA_LOW_ODR, 
		L3GD20H_DRDY_HL_BIT, buffer);
	return buffer[0];
}

/** Set whether only the SPI interface is enabled (i.e., I2C interface disabled)
 * @param enabled New SPI interface only enabled
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_I2C_DIS_BIT
 */
void L3GD20H::setSPIOnlyEnabled(bool enabled){
	I2Cdev::writeBit(devAddr, L3GD20H_RA_LOW_ODR, L3GD20H_I2C_DIS_BIT,
		enabled);
}

/** Get whether only the SPI interface is enabled (i.e., I2C interface disabled)
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_I2C_DIS_BIT
 */
bool L3GD20H::getSPIOnlyEnabled(){
	I2Cdev::readBit(devAddr, L3GD20H_RA_LOW_ODR, 
		L3GD20H_I2C_DIS_BIT, buffer);
	return buffer[0];
}

/** Reset the device.  Sets a software reset flag which is auto cleared upon boot.
 * @param reset Value that resets the device if true
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_SW_RESET_BIT
 */
void L3GD20H::setSoftwareReset(bool reset){
	I2Cdev::writeBit(devAddr, L3GD20H_RA_LOW_ODR, L3GD20H_SW_RESET_BIT,
		reset);
}

/** Set whether the low output data rate is enabled.
 * @param enabled New low output data rate enabled
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_LOW_ODR_BIT
 */
void L3GD20H::setLowODREnabled(bool enabled){
	I2Cdev::writeBit(devAddr, L3GD20H_RA_LOW_ODR, L3GD20H_LOW_ODR_BIT,
		enabled);
}

/** Get whether the low output data rate is enabled.
 * @see L3GD20H_RA_LOW_ODR
 * @see L3GD20H_LOW_ODR_BIT
 */
bool L3GD20H::getLowODREnabled(){
	I2Cdev::readBit(devAddr, L3GD20H_RA_LOW_ODR, 
		L3GD20H_LOW_ODR_BIT, buffer);
	return buffer[0];
}
