// I2Cdev library collection - BMA150 I2C device class header file
// Based on BMA150 datasheet, 29/05/2008
// 01/18/2012 by Brian McCain <bpmccain@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-01-18 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

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

#include "BMA150.h"

/** Default constructor, uses default I2C address.
 * @see BMA150_DEFAULT_ADDRESS
 */
BMA150::BMA150() {
    devAddr = BMA150_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see BMA150_DEFAULT_ADDRESS
 * @see BMA150_ADDRESS_00
 */
BMA150::BMA150(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage. This sets the full scale range of 
 * the sensor, as well as the bandwidth
 */
void BMA150::initialize() {
	setRange(BMA150_RANGE_2G);
	setBandwidth(BMA150_BW_25HZ);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool BMA150::testConnection() {
    return getDeviceID() == 0b010;
}

// CHIP_ID register
/** Get Device ID.
 * This register is used to verify the identity of the device (0b010).
 * @return Device ID (should be 2 dec)
 * @see BMA150_RA_CHIP_ID
 */
uint8_t BMA150::getDeviceID() {
    I2Cdev::readByte(devAddr, BMA150_RA_CHIP_ID, buffer);
    return buffer[0];
}


// VERSION register
/** Get Chip Revision number
 * @return Chip Revision
 * @see BMA150_RA_VERSION
 */
 uint8_t BMA150::getChipRevision() {
    I2Cdev::readByte(devAddr, BMA150_RA_VERSION, buffer);
    return buffer[0];
}
		
// AXIS registers
/** Get 3-axis accelerometer readings.
 * @param x 16-bit signed integer container for X-axis acceleration
 * @param y 16-bit signed integer container for Y-axis acceleration
 * @param z 16-bit signed integer container for Z-axis acceleration
 * @see BMA150_RA_Y_AXIS_LSB
 */

void BMA150::getAcceleration(int16_t* x, int16_t* y, int16_t* z) {
    I2Cdev::readBytes(devAddr, BMA150_RA_X_AXIS_LSB, 6, buffer);
    *x = ((((int16_t)buffer[1]) << 8) | buffer[0]) >> 6;
    *y = ((((int16_t)buffer[3]) << 8) | buffer[2]) >> 6;
    *z = ((((int16_t)buffer[5]) << 8) | buffer[4]) >> 6;
}

/** Get X-axis accelerometer reading.
 * @return X-axis acceleration measurement in 16-bit 2's complement format
 * @see BMA150_RA_X_AXIS_LSB
 */
int16_t BMA150::getAccelerationX() {
    I2Cdev::readBytes(devAddr, BMA150_RA_X_AXIS_LSB, 2, buffer);
    return ((((int16_t)buffer[1]) << 8) | buffer[0]) >> 6;
}

/** Get Y-axis accelerometer reading.
 * @return Y-axis acceleration measurement in 16-bit 2's complement format
 * @see BMA150_RA_Y_AXIS_LSB
 */
int16_t BMA150::getAccelerationY() {
    I2Cdev::readBytes(devAddr, BMA150_RA_Y_AXIS_LSB, 2, buffer);
    return ((((int16_t)buffer[1]) << 8) | buffer[0]) >> 6;
}

/** Get Z-axis accelerometer reading.
 * @return Z-axis acceleration measurement in 16-bit 2's complement format
 * @see BMA150_RA_Z_AXIS_LSB
 */
int16_t BMA150::getAccelerationZ() {
    I2Cdev::readBytes(devAddr, BMA150_RA_Z_AXIS_LSB, 2, buffer);
    return ((((int16_t)buffer[1]) << 8) | buffer[0]) >> 6;
}

/** Check for new X axis acceleration data.
 * @return New X-Axis Data Status
 * @see BMA150_RA_X_AXIS_LSB
 */
bool BMA150::newDataX() {
    I2Cdev::readBit(devAddr, BMA150_RA_X_AXIS_LSB, BMA150_X_NEW_DATA_BIT, buffer);
	return buffer[0];
}

/** Check for new Y axis acceleration data.
 * @return New Y-Axis Data Status
 * @see BMA150_RA_Y_AXIS_LSB
 */
bool BMA150::newDataY() {
    I2Cdev::readBit(devAddr, BMA150_RA_Y_AXIS_LSB, BMA150_Y_NEW_DATA_BIT, buffer);
	return buffer[0];
}

/** Check for new Z axis acceleration data.
 * @return New Z-Axis Data Status
 * @see BMA150_RA_Z_AXIS_LSB
 */
bool BMA150::newDataZ() {
    I2Cdev::readBit(devAddr, BMA150_RA_Z_AXIS_LSB, BMA150_Z_NEW_DATA_BIT, buffer);
	return buffer[0];
}
				
// TEMP register
/** Check for current temperature
 * @return Current Temperature in 0.5C increments from -30C at 00h
 * @see BMA150_RA_TEMP_RD
 */
int8_t BMA150::getTemperature() {
    I2Cdev::readByte(devAddr, BMA150_RA_TEMP_RD, buffer);
    return buffer[0];
}
		
// SMB150 registers
bool BMA150::getStatusHG() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_STATUS_HG_BIT, buffer);
	return buffer[0];
}
void BMA150::setStatusHG(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_STATUS_HG_BIT, enabled);
}

bool BMA150::getStatusLG() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_STATUS_LG_BIT, buffer);
	return buffer[0];
}
void BMA150::setStatusLG(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_STATUS_LG_BIT, enabled);
}

bool BMA150::getHGLatched() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_HG_LATCHED_BIT, buffer);
	return buffer[0];
}
void BMA150::setHGLatched(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_HG_LATCHED_BIT, enabled);
}

bool BMA150::getLGLatched() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_LG_LATCHED_BIT, buffer);
	return buffer[0];
}
void BMA150::setLGLatched(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_LG_LATCHED_BIT, enabled);
}

bool BMA150::getAlertPhase() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_ALERT_PHASE_BIT, buffer);
	return buffer[0];
}
void BMA150::setAlertPhase(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_ALERT_PHASE_BIT, enabled);
}

bool BMA150::getSTResult() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_ST_RESULT_BIT, buffer);
	return buffer[0];
}
void BMA150::setSTResult(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_STATUS, BMA150_ST_RESULT_BIT, enabled);
}






bool BMA150::getSleep() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_SLEEP_BIT, buffer);
	return buffer[0];
}
void BMA150::setSleep(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_SLEEP_BIT, enabled);
}

bool BMA150::getSoftReset() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_SOFT_RESET_BIT, buffer);
	return buffer[0];
}
void BMA150::setSoftReset(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_SOFT_RESET_BIT, enabled);
}

bool BMA150::getSelfTest0() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_ST0_BIT, buffer);
	return buffer[0];
}
void BMA150::setSelfTest0(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_ST0_BIT, enabled);
}

bool BMA150::getSelfTest1() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_ST1_BIT, buffer);
	return buffer[0];
}
void BMA150::setSelfTest1(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_ST1_BIT, enabled);
}

bool BMA150::getEEW() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_EEW_BIT, buffer);
	return buffer[0];
}
void BMA150::setEEW(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_EEW_BIT, enabled);
}

bool BMA150::getUpdateImage() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_UPDATE_IMAGE_BIT, buffer);
	return buffer[0];
}
void BMA150::setUpdateImage(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_UPDATE_IMAGE_BIT, enabled);
}

bool BMA150::getResetINT() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_RESET_INT_BIT, buffer);
	return buffer[0];
}
void BMA150::setResetINT(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CTRL, BMA150_RESET_INT_BIT, enabled);
}




bool BMA150::getEnableLG() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ENABLE_LG_BIT, buffer);
	return buffer[0];
}
void BMA150::setEnableLG(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ENABLE_LG_BIT, enabled);
}

bool BMA150::getEnableHG() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ENABLE_HG_BIT, buffer);
	return buffer[0];
}
void BMA150::setEnableHG(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ENABLE_HG_BIT, enabled);
}

int8_t BMA150::getCounterLG() {
    I2Cdev::readBits(devAddr, BMA150_RA_SMB150_CONF1, BMA150_COUNTER_LG_BIT, BMA150_COUNTER_LG_LENGTH, buffer);
    return buffer[0];
}

void BMA150::setCounterLG(int8_t counter_lg) {
    I2Cdev::writeBits(devAddr, BMA150_RA_SMB150_CONF1, BMA150_COUNTER_LG_BIT, BMA150_COUNTER_LG_LENGTH, counter_lg);
}

int8_t BMA150::getCounterHG() {
    I2Cdev::readBits(devAddr, BMA150_RA_SMB150_CONF1, BMA150_COUNTER_HG_BIT, BMA150_COUNTER_HG_LENGTH, buffer);
    return buffer[0];
}

void BMA150::setCounterHG(int8_t counter_hg) {
    I2Cdev::writeBits(devAddr, BMA150_RA_SMB150_CONF1, BMA150_COUNTER_HG_BIT, BMA150_COUNTER_HG_LENGTH, counter_hg);
}

bool BMA150::getAnyMotion() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ANY_MOTION_BIT, buffer);
	return buffer[0];
}
void BMA150::setAnyMotion(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ANY_MOTION_BIT, enabled);
}

bool BMA150::getAlert() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ALERT_BIT, buffer);
	return buffer[0];
}
void BMA150::setAlert(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF1, BMA150_ALERT_BIT, enabled);
}






bool BMA150::getWakeUp() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_WAKE_UP_BIT, buffer);
	return buffer[0];
}
void BMA150::setWakeUp(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_WAKE_UP_BIT, enabled);
}

int8_t BMA150::getWakeUpPause() {
    I2Cdev::readBits(devAddr, BMA150_RA_SMB150_CONF1, BMA150_WAKE_UP_PAUSE_BIT, BMA150_WAKE_UP_PAUSE_LENGTH, buffer);
    return buffer[0];
}

void BMA150::setWakeUpPause(int8_t wake_up_pause) {
    I2Cdev::writeBits(devAddr, BMA150_RA_SMB150_CONF1, BMA150_WAKE_UP_PAUSE_BIT, BMA150_WAKE_UP_PAUSE_LENGTH, wake_up_pause);
}

bool BMA150::getShadowDis() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_SHADOW_DIS_BIT, buffer);
	return buffer[0];
}
void BMA150::setShadowDis(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_SHADOW_DIS_BIT, enabled);
}

bool BMA150::getLatchInt() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_LATCH_INT_BIT, buffer);
	return buffer[0];
}
void BMA150::setLatchInt(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_LATCH_INT_BIT, enabled);
}

bool BMA150::getNewDataInt() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_NEW_DATA_INT_BIT, buffer);
	return buffer[0];
}
void BMA150::setNewDataInt(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_NEW_DATA_INT_BIT, enabled);
}

bool BMA150::getEnableAdvInt() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_ENABLE_ADV_INT_BIT, buffer);
	return buffer[0];
}
void BMA150::setEnableAdvInt(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_ENABLE_ADV_INT_BIT, enabled);
}

bool BMA150::getSPI4() {
    I2Cdev::readBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_SPI4_BIT, buffer);
	return buffer[0];
}
void BMA150::setSPI4(bool enabled) {
    I2Cdev::writeBit(devAddr, BMA150_RA_SMB150_CONF2, BMA150_SPI4_BIT, enabled);
}

		
// LG / HG registers
uint8_t BMA150::getLGThreshold() {
    I2Cdev::readByte(devAddr, BMA150_RA_LG_THRESHOLD, buffer);
    return buffer[0];
}

void BMA150::setLGThreshold(uint8_t lgthres) {
    I2Cdev::writeByte(devAddr, BMA150_RA_LG_THRESHOLD, lgthres);
}


uint8_t BMA150::getLGDuration() {
    I2Cdev::readByte(devAddr, BMA150_RA_LG_DURATION, buffer);
    return buffer[0];
}

void BMA150::setLGDuration(uint8_t lgdur) {
    I2Cdev::writeByte(devAddr, BMA150_RA_LG_DURATION, lgdur);
}


uint8_t BMA150::getHGThreshold() {
    I2Cdev::readByte(devAddr, BMA150_RA_HG_THRESHOLD, buffer);
    return buffer[0];
}

void BMA150::setHGThreshold(uint8_t hgthres) {
    I2Cdev::writeByte(devAddr, BMA150_RA_HG_THRESHOLD, hgthres);
}


uint8_t BMA150::getHGDuration() {
    I2Cdev::readByte(devAddr, BMA150_RA_HG_DURATION, buffer);
    return buffer[0];
}

void BMA150::setHGDuration(uint8_t hgdur) {
    I2Cdev::writeByte(devAddr, BMA150_RA_HG_DURATION, hgdur);
}


// MOTION_THRS register
uint8_t BMA150::getMotionThreshold() {
    I2Cdev::readByte(devAddr, BMA150_RA_MOTION_THRS, buffer);
    return buffer[0];
}

void BMA150::setMotionThreshold(uint8_t mot_thres) {
    I2Cdev::writeByte(devAddr, BMA150_RA_MOTION_THRS, mot_thres);
}

// HYSTERESIS register
uint8_t BMA150::getLGHysteresis() {
    I2Cdev::readBits(devAddr, BMA150_RA_HYSTERESIS, BMA150_LG_HYST_BIT, BMA150_LG_HYST_LENGTH, buffer);
    return buffer[0];
}

void BMA150::setLGHysteresis(uint8_t lg_hyst) {
    I2Cdev::writeBits(devAddr, BMA150_RA_HYSTERESIS, BMA150_LG_HYST_BIT, BMA150_LG_HYST_LENGTH,lg_hyst);
}

uint8_t BMA150::getHGHysteresis() {
    I2Cdev::readBits(devAddr, BMA150_RA_HYSTERESIS, BMA150_HG_HYST_BIT, BMA150_HG_HYST_LENGTH, buffer);
    return buffer[0];
}

void BMA150::setHGHysteresis(uint8_t hg_hyst) {
    I2Cdev::writeBits(devAddr, BMA150_RA_HYSTERESIS, BMA150_HG_HYST_BIT, BMA150_HG_HYST_LENGTH,hg_hyst);
}

uint8_t BMA150::getMotionDuration() {
    I2Cdev::readBits(devAddr, BMA150_RA_HYSTERESIS, BMA150_ANY_MOTION_DUR_BIT, BMA150_ANY_MOTION_DUR_LENGTH, buffer);
    return buffer[0];
}

void BMA150::setMotionDuration(uint8_t mot_dur) {
    I2Cdev::writeBits(devAddr, BMA150_RA_HYSTERESIS, BMA150_ANY_MOTION_DUR_BIT, BMA150_ANY_MOTION_DUR_LENGTH, mot_dur);
}

	
// CUSTOMER registers
uint8_t BMA150::getCustom1() {
    I2Cdev::readByte(devAddr, BMA150_RA_CUSTOMER1, buffer);
    return buffer[0];
}

void BMA150::setCustom1(uint8_t custom1) {
    I2Cdev::writeByte(devAddr, BMA150_RA_CUSTOMER1, custom1);
}

uint8_t BMA150::getCustom2() {
    I2Cdev::readByte(devAddr, BMA150_RA_CUSTOMER2, buffer);
    return buffer[0];
}

void BMA150::setCustom2(uint8_t custom2) {
    I2Cdev::writeByte(devAddr, BMA150_RA_CUSTOMER2, custom2);
}
	
// RANGE / BANDWIDTH registers

/** Get Sensor Full Range
 * @return Current Sensor Full Scale Range
 * 0 = +/- 2G
 * 1 = +/- 4G
 * 2 = +/- 8G
 * @see BMA150_RA_RANGE_BWIDTH
 * @see BMA150_RANGE_BIT
 * @see BMA150_RANGE_LENGTH
 */
uint8_t BMA150::getRange() {
    I2Cdev::readBits(devAddr, BMA150_RA_RANGE_BWIDTH, BMA150_RANGE_BIT, BMA150_RANGE_LENGTH, buffer);
    return buffer[0];
}

/** Set Sensor Full Range
 * @param range New full-scale range value
 * @see getRange()
 * @see BMA150_RA_RANGE_BWIDTH
 * @see BMA150_RANGE_BIT
 * @see BMA150_RANGE_LENGTH
 */
void BMA150::setRange(uint8_t range) {
    I2Cdev::writeBits(devAddr, BMA150_RA_RANGE_BWIDTH, BMA150_RANGE_BIT, BMA150_RANGE_LENGTH, range);
}


/** Get digital filter bandwidth.
 * The bandwidth parameter is used to setup the digital filtering of ADC output data to obtain
 * the desired bandwidth.
 * @return Current Sensor Bandwidth
 * 0 = 25Hz
 * 1 = 50Hz
 * 2 = 100Hz
 * 3 = 190Hz
 * 4 = 375Hz
 * 5 = 750Hz
 * 6 = 1500Hz
 * @see BMA150_RA_RANGE_BWIDTH
 * @see BMA150_RANGE_BIT
 * @see BMA150_RANGE_LENGTH
 */
uint8_t BMA150::getBandwidth() {
    I2Cdev::readBits(devAddr, BMA150_RA_RANGE_BWIDTH, BMA150_RANGE_BIT, BMA150_RANGE_LENGTH, buffer);
    return buffer[0];
}

/** Set Sensor Full Range
 * @param bandwidth New bandwidth value
 * @see getBandwidth()
 * @see BMA150_RA_RANGE_BWIDTH
 * @see BMA150_RANGE_BIT
 * @see BMA150_RANGE_LENGTH
 */
void BMA150::setBandwidth(uint8_t bandwidth) {
    I2Cdev::writeBits(devAddr, BMA150_RA_RANGE_BWIDTH, BMA150_BANDWIDTH_BIT, BMA150_BANDWIDTH_LENGTH, bandwidth);
}