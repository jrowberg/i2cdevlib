// I2Cdev library collection - MPU6050 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 8/24/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

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

#include "MPU6050.h"

/** Default constructor, uses default I2C address.
 * @see MPU6050_DEFAULT_ADDRESS
 */
MPU6050::MPU6050() {
    devAddr = MPU6050_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see MPU6050_DEFAULT_ADDRESS
 * @see MPU6050_ADDRESS_AD0_LOW
 * @see MPU6050_ADDRESS_AD0_HIGH
 */
MPU6050::MPU6050(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void MPU6050::initialize() {
    setClockSource(MPU6050_CLOCK_PLL_XGYRO);
    setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    setSleepEnabled(false);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool MPU6050::testConnection() {
    return getDeviceID() == 0b110100;
}

// AUX_VDDIO register

uint8_t MPU6050::getAuxVDDIOLevel() {
    I2Cdev::readBit(devAddr, MPU6050_RA_AUX_VDDIO, MPU6050_VDDIO_BIT, buffer);
    return buffer[0];
}
void MPU6050::setAuxVDDIOLevel(uint8_t level) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_AUX_VDDIO, MPU6050_VDDIO_BIT, level);
}

// SMPLRT_DIV register

/** Get sample rate.
 * @return Current sample rate
 * @see MPU6050_RA_SMPLRT_DIV
 */
uint8_t MPU6050::getRate() {
    return I2Cdev::readByte(devAddr, MPU6050_RA_SMPLRT_DIV, buffer);
    return buffer[0];
}
/** Set sample rate.
 * @param rate New sample rate
 * @see getRate()
 * @see MPU6050_RA_SMPLRT_DIV
 */
void MPU6050::setRate(uint8_t rate) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_SMPLRT_DIV, rate);
}

// CONFIG register

uint8_t MPU6050::getExternalFrameSync() {
    I2Cdev::readBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_EXT_SYNC_SET_BIT, MPU6050_CFG_EXT_SYNC_SET_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setExternalFrameSync(uint8_t sync) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_EXT_SYNC_SET_BIT, MPU6050_CFG_EXT_SYNC_SET_LENGTH, sync);
}
/** Get digital low-pass filter bandwidth.
 * The DLPF_CFG parameter sets the digital low pass filter configuration. It
 * also determines the internal sampling rate used by the device as shown in
 * the table below.
 *
 * DLPF_CFG | Low-Pass Filter Bandwidth | Internal Sample Rate
 * ---------+---------------------------+---------------------
 * 0        | 256Hz                     | 8kHz
 * 1        | 188Hz                     | 1kHz
 * 2        | 98Hz                      | 1kHz
 * 3        | 42Hz                      | 1kHz
 * 4        | 20Hz                      | 1kHz
 * 5        | 10Hz                      | 1kHz
 * 6        | 5Hz                       | 1kHz
 * 7        | Reserved                  | Reserved
 *
 * @return DLFP bandwidth setting
 * @see MPU6050_RA_CONFIG
 * @see MPU6050_CFG_DLPF_CFG_BIT
 * @see MPU6050_CFG_DLPF_CFG_LENGTH
 */
uint8_t MPU6050::getDLPFBandwidth() {
    I2Cdev::readBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, buffer);
    return buffer[0];
}
/** Set digital low-pass filter bandwidth.
 * @param bandwidth New DLFP bandwidth setting
 * @see getDLPFBandwidth()
 * @see MPU6050_DLPF_BW_256
 * @see MPU6050_RA_CONFIG
 * @see MPU6050_CFG_DLPF_CFG_BIT
 * @see MPU6050_CFG_DLPF_CFG_LENGTH
 */
void MPU6050::setDLPFBandwidth(uint8_t bandwidth) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, bandwidth);
}

// GYRO_CONFIG register

/** Set full-scale range.
 * The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
 * as described in the table below. The power-on-reset value of FS_SEL is 00h.
 * Set to 03h for proper operation.
 *
 * 0 = Reserved
 * 1 = Reserved
 * 2 = Reserved
 * 3 = +/- 2000 degrees/sec
 *
 * @return Current full-scale range setting
 * @see MPU6050_FULLSCALE_2000
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
uint8_t MPU6050::getFullScaleGyroRange() {
    I2Cdev::readBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, buffer);
    return buffer[0];
}
/** Set full-scale range setting.
 * @param range New full-scale range value
 * @see getFullScaleRange()
 * @see MPU6050_FULLSCALE_2000
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void MPU6050::setFullScaleGyroRange(uint8_t range) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

// ACCEL_CONFIG register

bool MPU6050::getAccelXSelfTest() {
    I2Cdev::readBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_XA_ST_BIT, buffer);
    return buffer[0];
}
void MPU6050::setAccelXSelfTest(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_XA_ST_BIT, enabled);
}
bool MPU6050::getAccelYSelfTest() {
    I2Cdev::readBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_YA_ST_BIT, buffer);
    return buffer[0];
}
void MPU6050::setAccelYSelfTest(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_YA_ST_BIT, enabled);
}
bool MPU6050::getAccelZSelfTest() {
    I2Cdev::readBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ZA_ST_BIT, buffer);
    return buffer[0];
}
void MPU6050::setAccelZSelfTest(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ZA_ST_BIT, enabled);
}
uint8_t MPU6050::getFullScaleAccelRange() {
    I2Cdev::readBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setFullScaleAccelRange(uint8_t range) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}
uint8_t MPU6050::getDHPFBandwidth() {
    I2Cdev::readBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ACCEL_HPF_BIT, MPU6050_ACONFIG_ACCEL_HPF_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setDHPFBandwidth(uint8_t bandwidth) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_ACCEL_HPF_BIT, MPU6050_ACONFIG_ACCEL_HPF_LENGTH, bandwidth);
}

// FF_THR register

uint8_t MPU6050::getFreefallAccelerationThreshold() {
    I2Cdev::readByte(devAddr, MPU6050_RA_FF_THR, buffer);
    return buffer[0];
}
void MPU6050::setFreefallAccelerationThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_FF_THR, threshold);
}

// FF_DUR register

uint8_t MPU6050::getFreefallDuration() {
    I2Cdev::readByte(devAddr, MPU6050_RA_FF_DUR, buffer);
    return buffer[0];
}
void MPU6050::setFreefallDuration(uint8_t duration) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_FF_DUR, duration);
}

// MOT_THR register

uint8_t MPU6050::getMotionDetectionThreshold() {
    I2Cdev::readByte(devAddr, MPU6050_RA_MOT_THR, buffer);
    return buffer[0];
}
void MPU6050::setMotionDetectionThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_MOT_THR, threshold);
}

// MOT_DUR register

uint8_t MPU6050::getMotionDetectionDuration() {
    I2Cdev::readByte(devAddr, MPU6050_RA_MOT_DUR, buffer);
    return buffer[0];
}
void MPU6050::setMotionDetectionDuration(uint8_t duration) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_MOT_DUR, duration);
}

// ZRMOT_THR register

uint8_t MPU6050::getZeroMotionDetectionThreshold() {
    I2Cdev::readByte(devAddr, MPU6050_RA_ZRMOT_THR, buffer);
    return buffer[0];
}
void MPU6050::setZeroMotionDetectionThreshold(uint8_t threshold) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_ZRMOT_THR, threshold);
}

// ZRMOT_DUR register

uint8_t MPU6050::getZeroMotionDetectionDuration() {
    I2Cdev::readByte(devAddr, MPU6050_RA_ZRMOT_DUR, buffer);
    return buffer[0];
}
void MPU6050::setZeroMotionDetectionDuration(uint8_t duration) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_ZRMOT_DUR, duration);
}

// FIFO_EN register

bool MPU6050::getTempFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_TEMP_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setTempFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_TEMP_FIFO_EN_BIT, enabled);
}
bool MPU6050::getXGyroFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_XG_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setXGryoFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_XG_FIFO_EN_BIT, enabled);
}
bool MPU6050::getYGyroFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_YG_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setYGyroFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_YG_FIFO_EN_BIT, enabled);
}
bool MPU6050::getZGyroFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_ZG_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setZGyroFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_ZG_FIFO_EN_BIT, enabled);
}
bool MPU6050::getAccelFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_ACCEL_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setAccelFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_ACCEL_FIFO_EN_BIT, enabled);
}
bool MPU6050::getSlave2FIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_SLV2_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlave2FIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_SLV2_FIFO_EN_BIT, enabled);
}
bool MPU6050::getSlave1FIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_SLV1_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlave1FIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_SLV1_FIFO_EN_BIT, enabled);
}
bool MPU6050::getSlave0FIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_SLV0_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlave0FIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_FIFO_EN, MPU6050_SLV0_FIFO_EN_BIT, enabled);
}

// I2C_MST_CTRL register

bool MPU6050::getMultiMasterEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_MULT_MST_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setMultiMasterEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_MULT_MST_EN_BIT, enabled);
}
bool MPU6050::getWaitForExternalSensorEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_WAIT_FOR_ES_BIT, buffer);
    return buffer[0];
}
void MPU6050::setWaitForExternalSensorEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_WAIT_FOR_ES_BIT, enabled);
}
bool MPU6050::getSlave3FIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_SLV_3_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlave3FIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_SLV_3_FIFO_EN_BIT, enabled);
}
bool MPU6050::getSlaveReadWriteTransitionEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_I2C_MST_P_NSR_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlaveReadWriteTransitionEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_I2C_MST_P_NSR_BIT, enabled);
}
uint8_t MPU6050::getMasterClockSpeed() {
    I2Cdev::readBits(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_I2C_MST_CLK_BIT, MPU6050_I2C_MST_CLK_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setMasterClockSpeed(uint8_t speed) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_I2C_MST_CTRL, MPU6050_I2C_MST_CLK_BIT, MPU6050_I2C_MST_CLK_LENGTH, speed);
}

// I2C_SLV* registers (Slave 0-3)

uint8_t MPU6050::getSlaveAddress(uint8_t num) {
    if (num > 3) return 0;
    I2Cdev::readByte(devAddr, MPU6050_RA_I2C_SLV0_ADDR + num*3, buffer);
    return buffer[0];
}
void MPU6050::setSlaveAddress(uint8_t num, uint8_t address) {
    if (num > 3) return;
    I2Cdev::writeByte(devAddr, MPU6050_RA_I2C_SLV0_ADDR + num*3, address);
}
uint8_t MPU6050::getSlaveRegister(uint8_t num) {
    if (num > 3) return 0;
    I2Cdev::readByte(devAddr, MPU6050_RA_I2C_SLV0_REG + num*3, buffer);
    return buffer[0];
}
void MPU6050::setSlaveRegister(uint8_t num, uint8_t reg) {
    if (num > 3) return;
    I2Cdev::writeByte(devAddr, MPU6050_RA_I2C_SLV0_REG + num*3, reg);
}
bool MPU6050::getSlaveEnabled(uint8_t num) {
    if (num > 3) return 0;
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlaveEnabled(uint8_t num, bool enabled) {
    if (num > 3) return;
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_EN_BIT, enabled);
}
bool MPU6050::getSlaveWordByteSwap(uint8_t num) {
    if (num > 3) return 0;
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_BYTE_SW_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlaveWordByteSwap(uint8_t num, bool enabled) {
    if (num > 3) return;
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_BYTE_SW_BIT, enabled);
}
bool MPU6050::getSlaveWriteMode(uint8_t num) {
    if (num > 3) return 0;
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_REG_DIS_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlaveWriteMode(uint8_t num, bool mode) {
    if (num > 3) return;
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_REG_DIS_BIT, mode);
}
bool MPU6050::getSlaveGroupIntoWords(uint8_t num) {
    if (num > 3) return 0;
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_GRP_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlaveGroupIntoWords(uint8_t num, bool enabled) {
    if (num > 3) return;
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_GRP_BIT, enabled);
}
uint8_t MPU6050::getSlaveDataLength(uint8_t num) {
    if (num > 3) return 0;
    I2Cdev::readBits(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_LEN_BIT, MPU6050_I2C_SLV_LEN_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setSlaveDataLength(uint8_t num, uint8_t length) {
    if (num > 3) return;
    I2Cdev::writeBits(devAddr, MPU6050_RA_I2C_SLV0_CTRL + num*3, MPU6050_I2C_SLV_LEN_BIT, MPU6050_I2C_SLV_LEN_LENGTH, length);
}

// I2C_SLV* registers (Slave 4)

uint8_t MPU6050::getSlave4Address() {
    I2Cdev::readByte(devAddr, MPU6050_RA_I2C_SLV4_ADDR, buffer);
    return buffer[0];
}
void MPU6050::setSlave4Address(uint8_t address) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_I2C_SLV4_ADDR, address);
}
uint8_t MPU6050::getSlave4Register() {
    I2Cdev::readByte(devAddr, MPU6050_RA_I2C_SLV4_REG, buffer);
    return buffer[0];
}
void MPU6050::setSlave4Register(uint8_t reg) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_I2C_SLV4_REG, reg);
}
void MPU6050::setSlave4OutputByte(uint8_t data) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_I2C_SLV4_DO, data);
}
bool MPU6050::getSlave4Enabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlave4Enabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_EN_BIT, enabled);
}
bool MPU6050::getSlave4InterruptEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_INT_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlave4InterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_INT_EN_BIT, enabled);
}
bool MPU6050::getSlave4WriteMode() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_REG_DIS_BIT, buffer);
    return buffer[0];
}
void MPU6050::setSlave4WriteMode(bool mode) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_REG_DIS_BIT, mode);
}
uint8_t MPU6050::getSlave4MasterDelay() {
    I2Cdev::readBits(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_MST_DLY_BIT, MPU6050_I2C_SLV4_MST_DLY_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setSlave4MasterDelay(uint8_t delay) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_I2C_SLV4_CTRL, MPU6050_I2C_SLV4_MST_DLY_BIT, MPU6050_I2C_SLV4_MST_DLY_LENGTH, delay);
}
uint8_t MPU6050::getSlate4InputByte() {
    I2Cdev::readByte(devAddr, MPU6050_RA_I2C_SLV4_DI, buffer);
    return buffer[0];
}

// I2C_MST_STATUS register

bool MPU6050::getPassthroughStatus() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_PASS_THROUGH_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getSlave4IsDone() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_I2C_SLV4_DONE_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getLostArbitration() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_I2C_LOST_ARB_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getSlave4Nack() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_I2C_SLV4_NACK_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getSlave3Nack() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_I2C_SLV3_NACK_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getSlave2Nack() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_I2C_SLV2_NACK_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getSlave1Nack() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_I2C_SLV1_NACK_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getSlave0Nack() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_STATUS, MPU6050_MST_I2C_SLV0_NACK_BIT, buffer);
    return buffer[0];
}

// INT_PIN_CFG register

/** Get interrupt logic level mode.
 * Will be set 0 for active-high, 1 for active-low.
 * @return Current interrupt mode (0=active-high, 1=active-low)
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_INT_LEVEL_BIT
 */
bool MPU6050::getInterruptMode() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_LEVEL_BIT, buffer);
    return buffer[0];
}
/** Set interrupt logic level mode.
 * @param mode New interrupt mode (0=active-high, 1=active-low)
 * @see getInterruptMode()
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_INT_LEVEL_BIT
 */
void MPU6050::setInterruptMode(bool mode) {
   I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_LEVEL_BIT, mode);
}
/** Get interrupt drive mode.
 * Will be set 0 for push-pull, 1 for open-drain.
 * @return Current interrupt drive mode (0=push-pull, 1=open-drain)
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_INT_OPEN_BIT
 */
bool MPU6050::getInterruptDrive() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_OPEN_BIT, buffer);
    return buffer[0];
}
/** Set interrupt drive mode.
 * @param drive New interrupt drive mode (0=push-pull, 1=open-drain)
 * @see getInterruptDrive()
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_INT_OPEN_BIT
 */
void MPU6050::setInterruptDrive(bool drive) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_OPEN_BIT, drive);
}
/** Get interrupt latch mode.
 * Will be set 0 for 50us-pulse, 1 for latch-until-int-cleared.
 * @return Current latch mode (0=50us-pulse, 1=latch-until-int-cleared)
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_LATCH_INT_EN_BIT
 */
bool MPU6050::getInterruptLatch() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_LATCH_INT_EN_BIT, buffer);
    return buffer[0];
}
/** Set interrupt latch mode.
 * @param latch New latch mode (0=50us-pulse, 1=latch-until-int-cleared)
 * @see getInterruptLatch()
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_LATCH_INT_EN_BIT
 */
void MPU6050::setInterruptLatch(bool latch) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_LATCH_INT_EN_BIT, latch);
}
/** Get interrupt latch clear mode.
 * Will be set 0 for status-read-only, 1 for any-register-read.
 * @return Current latch clear mode (0=status-read-only, 1=any-register-read)
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_INT_RD_CLEAR_BIT
 */
bool MPU6050::getInterruptLatchClear() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_RD_CLEAR_BIT, buffer);
    return buffer[0];
}
/** Set interrupt latch clear mode.
 * @param clear New latch clear mode (0=status-read-only, 1=any-register-read)
 * @see getInterruptLatchClear()
 * @see MPU6050_RA_INT_PIN_CFG
 * @see MPU6050_INTCFG_INT_RD_CLEAR_BIT
 */
void MPU6050::setInterruptLatchClear(bool clear) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_INT_RD_CLEAR_BIT, clear);
}
bool MPU6050::getFSyncInterruptLevel() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT, buffer);
    return buffer[0];
}
void MPU6050::setFSyncInterruptLevel(bool level) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT, level);
}
bool MPU6050::getFSyncInterruptEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_FSYNC_INT_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setFSyncInterruptEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_FSYNC_INT_EN_BIT, enabled);
}
bool MPU6050::getI2CBypassEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setI2CBypassEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}
bool MPU6050::getClockOutputEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_CLKOUT_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setClockOutputEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_CLKOUT_EN_BIT, enabled);
}

// INT_ENABLE register

bool MPU6050::getIntFreefallEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_FF_BIT, buffer);
    return buffer[0];
}
void MPU6050::setIntFreefallEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_FF_BIT, enabled);
}
bool MPU6050::getIntMotionEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_MOT_BIT, buffer);
    return buffer[0];
}
void MPU6050::setIntMotionEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_MOT_BIT, enabled);
}
bool MPU6050::getIntZeroMotionEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_ZMOT_BIT, buffer);
    return buffer[0];
}
void MPU6050::setIntZeroMotionEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_ZMOT_BIT, enabled);
}
bool MPU6050::getIntFIFOBufferOverflowEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_FIFO_OFLOW_BIT, buffer);
    return buffer[0];
}
void MPU6050::setIntFIFOBufferOverflowEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_FIFO_OFLOW_BIT, enabled);
}
bool MPU6050::getIntI2CMasterEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_I2C_MST_INT_BIT, buffer);
    return buffer[0];
}
void MPU6050::setIntI2CMasterEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_I2C_MST_INT_BIT, enabled);
}
/** Get "data ready" interrupt enabled setting.
 * Will be set 0 for disabled, 1 for enabled.
 * @return Current interrupt enabled setting
 * @see MPU6050_RA_INT_ENABLE
 * @see MPU6050_INTERRUPT_DATA_RDY_BIT
 */
bool MPU6050::getIntDataReadyEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, buffer);
    return buffer[0];
}
/** Set "data ready" interrupt enabled setting.
 * @param enabled New interrupt enabled setting
 * @see getIntDataReadyEnabled()
 * @see MPU6050_RA_INT_CFG
 * @see MPU6050_INTERRUPT_DATA_RDY_BIT
 */
void MPU6050::setIntDataReadyEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_INT_ENABLE, MPU6050_INTERRUPT_DATA_RDY_BIT, enabled);
}

// INT_STATUS register

bool MPU6050::getIntFreefallStatus() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_STATUS, MPU6050_INTERRUPT_FF_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getIntMotionStatus() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_STATUS, MPU6050_INTERRUPT_MOT_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getIntZeroMotionStatus() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_STATUS, MPU6050_INTERRUPT_ZMOT_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getIntFIFOBufferOverflowStatus() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_STATUS, MPU6050_INTERRUPT_FIFO_OFLOW_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getIntI2CMasterStatus() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_STATUS, MPU6050_INTERRUPT_I2C_MST_INT_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getIntDataReadyStatus() {
    I2Cdev::readBit(devAddr, MPU6050_RA_INT_STATUS, MPU6050_INTERRUPT_DATA_RDY_BIT, buffer);
    return buffer[0];
}

// ACCEL_*OUT_* registers

void MPU6050::getMotion9(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz, int16_t* mx, int16_t* my, int16_t* mz) {
    getMotion6(ax, ay, az, gx, gy, gz);
    // TODO: magnetometer integration
}
void MPU6050::getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 14, buffer);
    *ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    *ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    *az = (((int16_t)buffer[4]) << 8) | buffer[5];
    *gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    *gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    *gz = (((int16_t)buffer[12]) << 8) | buffer[13];
}
/** Get 3-axis accelerometer readings.
 * @param x 16-bit signed integer container for X-axis acceleration
 * @param y 16-bit signed integer container for Y-axis acceleration
 * @param z 16-bit signed integer container for Z-axis acceleration
 * @see MPU6050_RA_GYRO_XOUT_H
 */
void MPU6050::getAcceleration(int16_t* x, int16_t* y, int16_t* z) {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 6, buffer);
    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[2]) << 8) | buffer[3];
    *z = (((int16_t)buffer[4]) << 8) | buffer[5];
}
/** Get X-axis accelerometer reading.
 * @return X-axis acceleration measurement in 16-bit 2's complement format
 * @see MPU6050_RA_ACCEL_XOUT_H
 */
int16_t MPU6050::getAccelerationX() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Y-axis accelerometer reading.
 * @return Y-axis acceleration measurement in 16-bit 2's complement format
 * @see MPU6050_RA_ACCEL_YOUT_H
 */
int16_t MPU6050::getAccelerationY() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_YOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Z-axis accelerometer reading.
 * @return Z-axis acceleration measurement in 16-bit 2's complement format
 * @see MPU6050_RA_ACCEL_ZOUT_H
 */
int16_t MPU6050::getAccelerationZ() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_ACCEL_ZOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

// TEMP_OUT_* registers

/** Get current internal temperature.
 * @return Temperature reading in 16-bit 2's complement format
 * @see MPU6050_RA_TEMP_OUT_H
 */
int16_t MPU6050::getTemperature() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_TEMP_OUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

// GYRO_*OUT_* registers

/** Get 3-axis gyroscope readings.
 * @param x 16-bit signed integer container for X-axis rotation
 * @param y 16-bit signed integer container for Y-axis rotation
 * @param z 16-bit signed integer container for Z-axis rotation
 * @see MPU6050_RA_GYRO_XOUT_H
 */
void MPU6050::getRotation(int16_t* x, int16_t* y, int16_t* z) {
    I2Cdev::readBytes(devAddr, MPU6050_RA_GYRO_XOUT_H, 6, buffer);
    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[2]) << 8) | buffer[3];
    *z = (((int16_t)buffer[4]) << 8) | buffer[5];
}
/** Get X-axis gyroscope reading.
 * @return X-axis rotation measurement in 16-bit 2's complement format
 * @see MPU6050_RA_GYRO_XOUT_H
 */
int16_t MPU6050::getRotationX() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_GYRO_XOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Y-axis gyroscope reading.
 * @return Y-axis rotation measurement in 16-bit 2's complement format
 * @see MPU6050_RA_GYRO_YOUT_H
 */
int16_t MPU6050::getRotationY() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_GYRO_YOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Z-axis gyroscope reading.
 * @return Z-axis rotation measurement in 16-bit 2's complement format
 * @see MPU6050_RA_GYRO_ZOUT_H
 */
int16_t MPU6050::getRotationZ() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_GYRO_ZOUT_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}

// EXT_SENS_DATA_* registers

uint8_t MPU6050::getExternalSensorByte(int position) {
    I2Cdev::readByte(devAddr, MPU6050_RA_EXT_SENS_DATA_00 + position, buffer);
    return buffer[0];
}
uint16_t MPU6050::getExternalSensorWord(int position) {
    I2Cdev::readBytes(devAddr, MPU6050_RA_EXT_SENS_DATA_00 + position, 2, buffer);
    return (((uint16_t)buffer[0]) << 8) | buffer[1];
}
uint32_t MPU6050::getExternalSensorDWord(int position) {
    I2Cdev::readBytes(devAddr, MPU6050_RA_EXT_SENS_DATA_00 + position, 4, buffer);
    return (((uint32_t)buffer[0]) << 24) | (((uint32_t)buffer[1]) << 16) | (((uint16_t)buffer[2]) << 8) | buffer[3];
}

// MOT_DETECT_STATUS register

bool MPU6050::getXNegMotionDetected() {
    I2Cdev::readBit(devAddr, MPU6050_RA_MOT_DETECT_STATUS, MPU6050_MOTION_MOT_XNEG_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getXPosMotionDetected() {
    I2Cdev::readBit(devAddr, MPU6050_RA_MOT_DETECT_STATUS, MPU6050_MOTION_MOT_XPOS_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getYNegMotionDetected() {
    I2Cdev::readBit(devAddr, MPU6050_RA_MOT_DETECT_STATUS, MPU6050_MOTION_MOT_YNEG_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getYPosMotionDetected() {
    I2Cdev::readBit(devAddr, MPU6050_RA_MOT_DETECT_STATUS, MPU6050_MOTION_MOT_YPOS_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getZNegMotionDetected() {
    I2Cdev::readBit(devAddr, MPU6050_RA_MOT_DETECT_STATUS, MPU6050_MOTION_MOT_ZNEG_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getZPosMotionDetected() {
    I2Cdev::readBit(devAddr, MPU6050_RA_MOT_DETECT_STATUS, MPU6050_MOTION_MOT_ZPOS_BIT, buffer);
    return buffer[0];
}
bool MPU6050::getZeroMotionDetected() {
    I2Cdev::readBit(devAddr, MPU6050_RA_MOT_DETECT_STATUS, MPU6050_MOTION_MOT_ZRMOT_BIT, buffer);
    return buffer[0];
}

// I2C_SLV*_DO register

void MPU6050::setSlaveOutputByte(uint8_t num, uint8_t data) {
    if (num > 3) return;
    I2Cdev::writeByte(devAddr, MPU6050_RA_I2C_SLV0_DO + num, data);
}

// I2C_MST_DELAY_CTRL register

bool MPU6050::getExternalShadowDelayEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_DELAY_CTRL, MPU6050_DELAYCTRL_DELAY_ES_SHADOW_BIT, buffer);
    return buffer[0];
}
void MPU6050::setExternalShadowDelayEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_MST_DELAY_CTRL, MPU6050_DELAYCTRL_DELAY_ES_SHADOW_BIT, enabled);
}
bool MPU6050::getSlaveDelayEnabled(uint8_t num) {
    // MPU6050_DELAYCTRL_I2C_SLV4_DLY_EN_BIT is 4, SLV3 is 3, etc.
    if (num > 4) return 0;
    I2Cdev::readBit(devAddr, MPU6050_RA_I2C_MST_DELAY_CTRL, num, buffer);
    return buffer[0];
}
void MPU6050::setSlaveDelayEnabled(uint8_t num, bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_I2C_MST_DELAY_CTRL, num, enabled);
}

// SIGNAL_PATH_RESET register

void MPU6050::resetGyroscopePath() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_SIGNAL_PATH_RESET, MPU6050_PATHRESET_GYRO_RESET_BIT, true);
}
void MPU6050::resetAccelerometerPath() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_SIGNAL_PATH_RESET, MPU6050_PATHRESET_ACCEL_RESET_BIT, true);
}
void MPU6050::resetTemperaturePath() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_SIGNAL_PATH_RESET, MPU6050_PATHRESET_TEMP_RESET_BIT, true);
}

// MOT_DETECT_CTRL register

uint8_t MPU6050::getAcceleratorPowerOnDelay() {
    I2Cdev::readBits(devAddr, MPU6050_RA_MOT_DETECT_CTRL, MPU6050_DETECT_ACCEL_ON_DELAY_BIT, MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setAcceleratorPowerOnDelay(uint8_t delay) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_MOT_DETECT_CTRL, MPU6050_DETECT_ACCEL_ON_DELAY_BIT, MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH, delay);
}
uint8_t MPU6050::getFreefallDetectionCounterDecrement() {
    I2Cdev::readBits(devAddr, MPU6050_RA_MOT_DETECT_CTRL, MPU6050_DETECT_FF_COUNT_BIT, MPU6050_DETECT_FF_COUNT_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setFreefallDetectionCounterDecrement(uint8_t decrement) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_MOT_DETECT_CTRL, MPU6050_DETECT_FF_COUNT_BIT, MPU6050_DETECT_FF_COUNT_LENGTH, decrement);
}
uint8_t MPU6050::getMotionDetectionCounterDecrement() {
    I2Cdev::readBits(devAddr, MPU6050_RA_MOT_DETECT_CTRL, MPU6050_DETECT_MOT_COUNT_BIT, MPU6050_DETECT_MOT_COUNT_LENGTH, buffer);
    return buffer[0];
}
void MPU6050::setMotionDetectionCounterDecrement(uint8_t decrement) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_MOT_DETECT_CTRL, MPU6050_DETECT_MOT_COUNT_BIT, MPU6050_DETECT_MOT_COUNT_LENGTH, decrement);
}

// USER_CTRL register

bool MPU6050::getFIFOEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setFIFOEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, enabled);
}
bool MPU6050::getI2CMasterModeEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, buffer);
    return buffer[0];
}
void MPU6050::setI2CMasterModeEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}
void MPU6050::switchSPIEnabled(bool enabled) {
    //I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_IF_DIS_BIT, enabled);
}
void MPU6050::resetFIFO() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_RESET_BIT, true);
}
void MPU6050::resetI2CMaster() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_RESET_BIT, true);
}
void MPU6050::resetSensors() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_SIG_COND_RESET_BIT, true);
}

// PWR_MGMT_1 register

/** Trigger a full device reset.
 * A small delay of ~50ms may be desirable after triggering a reset.
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_DEVICE_RESET_BIT
 */
void MPU6050::reset() {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_DEVICE_RESET_BIT, true);
}
/** Get sleep mode status.
 * Setting the SLEEP bit in the register puts the device into very low power
 * sleep mode. In this mode, only the serial interface and internal registers
 * remain active, allowing for a very low standby current. Clearing this bit
 * puts the device back into normal mode. To save power, the individual standby
 * selections for each of the gyros should be used if any gyro axis is not used
 * by the application.
 * @return Current sleep mode enabled status
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
bool MPU6050::getSleepEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, buffer);
    return buffer[0];
}
/** Set sleep mode status.
 * @param enabled New sleep mode enabled status
 * @see getSleepEnabled()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_SLEEP_BIT
 */
void MPU6050::setSleepEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}
bool MPU6050::getWakeCycleEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CYCLE_BIT, buffer);
    return buffer[0];
}
void MPU6050::setWakeCycleEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CYCLE_BIT, enabled);
}
bool MPU6050::getTempSensorEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_TEMP_DIS_BIT, buffer);
    return buffer[0];
}
void MPU6050::setTempSensorEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_TEMP_DIS_BIT, enabled);
}
/** Get clock source setting.
 * @return Current clock source setting
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CLK_SEL_BIT
 * @see MPU6050_PWR1_CLK_SEL_LENGTH
 */
uint8_t MPU6050::getClockSource() {
    I2Cdev::readBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLK_SEL_BIT, MPU6050_PWR1_CLK_SEL_LENGTH, buffer);
    return buffer[0];
}
/** Set clock source setting.
 * On power up, the ITG-3200 defaults to the internal oscillator. It is highly recommended that the device is configured to use one of the gyros (or an external clock) as the clock reference, due to the improved stability.
 *
 * The CLK_SEL setting determines the device clock source as follows:
 *
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Reserved
 *
 * @param source New clock source setting
 * @see getClockSource()
 * @see MPU6050_RA_PWR_MGMT_1
 * @see MPU6050_PWR1_CLK_SEL_BIT
 * @see MPU6050_PWR1_CLK_SEL_LENGTH
 */
void MPU6050::setClockSource(uint8_t source) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLK_SEL_BIT, MPU6050_PWR1_CLK_SEL_LENGTH, source);
}

// PWR_MGMT_2 register

/** Get X-axis accelerometer standby enabled status.
 * If enabled, the X-axis will not gather or report data (or use power).
 * @return Current X-axis standby enabled status
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_XA_BIT
 */
bool MPU6050::getStandbyXAccelEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_XA_BIT, buffer);
    return buffer[0];
}
/** Set X-axis accelerometer standby enabled status.
 * @param New X-axis standby enabled status
 * @see getStandbyXAccelEnabled()
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_XA_BIT
 */
void MPU6050::setStandbyXAccelEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_XA_BIT, enabled);
}
/** Get Y-axis accelerometer standby enabled status.
 * If enabled, the Y-axis will not gather or report data (or use power).
 * @return Current Y-axis standby enabled status
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_YA_BIT
 */
bool MPU6050::getStandbyYAccelEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_YA_BIT, buffer);
    return buffer[0];
}
/** Set Y-axis accelerometer standby enabled status.
 * @param New Y-axis standby enabled status
 * @see getStandbyYAccelEnabled()
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_YA_BIT
 */
void MPU6050::setStandbyYAccelEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_YA_BIT, enabled);
}
/** Get Z-axis accelerometer standby enabled status.
 * If enabled, the Z-axis will not gather or report data (or use power).
 * @return Current Z-axis standby enabled status
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_ZA_BIT
 */
bool MPU6050::getStandbyZAccelEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_ZA_BIT, buffer);
    return buffer[0];
}
/** Set Z-axis accelerometer standby enabled status.
 * @param New Z-axis standby enabled status
 * @see getStandbyZAccelEnabled()
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_ZA_BIT
 */
void MPU6050::setStandbyZAccelEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_ZA_BIT, enabled);
}
/** Get X-axis gyroscope standby enabled status.
 * If enabled, the X-axis will not gather or report data (or use power).
 * @return Current X-axis standby enabled status
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_XG_BIT
 */
bool MPU6050::getStandbyXGyroEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_XG_BIT, buffer);
    return buffer[0];
}
/** Set X-axis gyroscope standby enabled status.
 * @param New X-axis standby enabled status
 * @see getStandbyXGyroEnabled()
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_XG_BIT
 */
void MPU6050::setStandbyXGyroEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_XG_BIT, enabled);
}
/** Get Y-axis gyroscope standby enabled status.
 * If enabled, the Y-axis will not gather or report data (or use power).
 * @return Current Y-axis standby enabled status
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_YG_BIT
 */
bool MPU6050::getStandbyYGyroEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_YG_BIT, buffer);
    return buffer[0];
}
/** Set Y-axis gyroscope standby enabled status.
 * @param New Y-axis standby enabled status
 * @see getStandbyYGyroEnabled()
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_YG_BIT
 */
void MPU6050::setStandbyYGyroEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_YG_BIT, enabled);
}
/** Get Z-axis gyroscope standby enabled status.
 * If enabled, the Z-axis will not gather or report data (or use power).
 * @return Current Z-axis standby enabled status
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_ZG_BIT
 */
bool MPU6050::getStandbyZGyroEnabled() {
    I2Cdev::readBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_ZG_BIT, buffer);
    return buffer[0];
}
/** Set Z-axis gyroscope standby enabled status.
 * @param New Z-axis standby enabled status
 * @see getStandbyZGyroEnabled()
 * @see MPU6050_RA_PWR_MGMT_2
 * @see MPU6050_PWR2_STBY_ZG_BIT
 */
void MPU6050::setStandbyZGyroEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, MPU6050_RA_PWR_MGMT_2, MPU6050_PWR2_STBY_ZG_BIT, enabled);
}

// FIFO_COUNT* registers

uint16_t MPU6050::getFIFOCount() {
    I2Cdev::readBytes(devAddr, MPU6050_RA_FIFO_COUNTH, 2, buffer);
    return (((uint16_t)buffer[0]) << 8) | buffer[1];
}

// FIFO_R_W register

uint8_t MPU6050::getFIFOByte() {
    I2Cdev::readByte(devAddr, MPU6050_RA_FIFO_R_W, buffer);
    return buffer[0];
}
void MPU6050::setFIFOByte(uint8_t data) {
    I2Cdev::writeByte(devAddr, MPU6050_RA_FIFO_R_W, data);
}

// WHO_AM_I register

/** Get Device ID.
 * This register is used to verify the identity of the device (0b110100).
 * @return Device ID (should be 0x68, 104 dec, 150 oct)
 * @see MPU6050_RA_WHO_AM_I
 * @see MPU6050_WHO_AM_I_BIT
 * @see MPU6050_WHO_AM_I_LENGTH
 */
uint8_t MPU6050::getDeviceID() {
    I2Cdev::readBits(devAddr, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, buffer);
    return buffer[0];
}
/** Set Device ID.
 * Write a new ID into the WHO_AM_I register (no idea why this should ever be
 * necessary though).
 * @param id New device ID to set.
 * @see getDeviceID()
 * @see MPU6050_RA_WHO_AM_I
 * @see MPU6050_WHO_AM_I_BIT
 * @see MPU6050_WHO_AM_I_LENGTH
 */
void MPU6050::setDeviceID(uint8_t id) {
    I2Cdev::writeBits(devAddr, MPU6050_RA_WHO_AM_I, MPU6050_WHO_AM_I_BIT, MPU6050_WHO_AM_I_LENGTH, id);
}
