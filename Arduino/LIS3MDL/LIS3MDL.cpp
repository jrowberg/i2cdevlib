/**
 * @brief Based on ST Devices LIS3MDL datasheet, May 2017
 * 21 December 2022 by Braidan Duffy <legohead259@gmail.com>
 * Updates should (hopefully) always be available at https://github.com/Legohead259/i2cdevlib
 * 
 * CHANGELOG:
 * 2022-12-01 - Initial Release
 */

/** ============================================================================
 * I2Cdev device library code is placed under the MIT license
 * Copyright (c) 2011 Braidan Duffy, Jeff Rowberg

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
================================================================================= */

#include "LIS3MDL.h"

/** 
 * @brief Default constructor, uses default I2C address.
 * @see LIS3MDL_DEFAULT_ADDRESS
 */
LIS3MDL::LIS3MDL() {
    devAddr = LIS3MDL_DEFAULT_ADDRESS;
}

/** 
 * @brief Specific address constructor.
 * @param address I2C address
 * @see LIS3MDL_ADDRESS
 */
LIS3MDL::LIS3MDL(uint8_t address) {
    devAddr = address;
}

/** 
 * @brief Power on and prepare for general usage.
 * All values are default
 * @see LIS3MDL_RA_CTRL1
 * @see LIS3MDL_RA_CTRL2
 * @see LIS3MDL_RA_CTRL3
 * @see LIS3MDL_RA_CTRL4
 * @see LIS3MDL_RA_CTRL5
 */
bool LIS3MDL::initialize() {
    return  testConnection() &&
            I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL1, 0b00010000) &&
            I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL2, 0b00000000) &&
            I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL3, 0b00000011) &&
            I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL4, 0b00000000) &&
            I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL5, 0b00000000);
}

/** 
 * @brief Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool LIS3MDL::testConnection() {
    return getDeviceID() == LIS3MDL_DEVICE_ID;
}


// ====================================
// === WHO_AM_I register, read-only ===
// ====================================


/**
 * @brief Query the device for it's WHOAMI ID
 * 
 * @return The WHOAMI ID of the device (should be 0x1C or 0x1E)
 * @see LIS3MDL_RA_WHO_AM_I
 */
uint8_t LIS3MDL::getDeviceID() {
    I2Cdev::readByte(devAddr, LIS3MDL_RA_WHO_AM_I, buffer);
    return buffer[0];
}


// ==============================
// === CONTROL2 REGISTER, R/W ===
// ==============================


/**
 * @brief Enable the temperature sensor on the device
 * 
 * @param en sensor enabled
 */
void LIS3MDL::enableTempSensor(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_TEMP_EN_BIT, en);
}

/**
 * @brief Get if the temperature sensor is enabled on the device
 * 
 * @return true if the sensor is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getTempSensorEnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_TEMP_EN_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Set the XY operating mode
 * 
 * @param mode the new operating mode. Note: This must be one of the four modes (0x00-03) designated by the datasheet
 * 
 * @see LIS3MDL_OM_LP_MODE
 * @see LIS3MDL_OM_MP_MODE
 * @see LIS3MDL_OM_HP_MODE
 * @see LIS3MDL_OM_UHP_MODE
 */
void LIS3MDL::setXYOpMode(uint8_t mode) {
    I2Cdev::writeBits(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_OM_BIT, LIS3MDL_OM_LENGTH, mode);
}

/**
 * @brief Returns the XY operating mode
 * 
 * @return the current operating mode (0x00-03)
 * 
 * @see LIS3MDL_OM_LP_MODE
 * @see LIS3MDL_OM_MP_MODE
 * @see LIS3MDL_OM_HP_MODE
 * @see LIS3MDL_OM_UHP_MODE
 */
uint8_t LIS3MDL::getXYOpMode() {
    I2Cdev::readBits(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_OM_BIT, LIS3MDL_OM_LENGTH, buffer);
    return buffer[0];
}

/**
 * @brief Sets the data rate of the device
 * 
 * @param rate the new data rate. Note this must be one of eight rates (0x00-07) specified in the datasheet
 * 
 * @see LIS3MDL_DATA_RATE_0_625_HZ
 * @see LIS3MDL_DATA_RATE_1_25_HZ
 * @see LIS3MDL_DATA_RATE_2_5_HZ
 * @see LIS3MDL_DATA_RATE_5_HZ
 * @see LIS3MDL_DATA_RATE_10_HZ
 * @see LIS3MDL_DATA_RATE_20_HZ
 * @see LIS3MDL_DATA_RATE_40_HZ
 * @see LIS3MDL_DATA_RATE_80_HZ
 */
void LIS3MDL::setDataRate(uint8_t rate) {
    I2Cdev::writeBits(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_DO_BIT, LIS3MDL_DO_LENGTH, rate);
}

/**
 * @brief Returns the current data rate
 * 
 * @return the current data rate (0x00-07)
 *
 * @see LIS3MDL_DATA_RATE_0_625_HZ
 * @see LIS3MDL_DATA_RATE_1_25_HZ
 * @see LIS3MDL_DATA_RATE_2_5_HZ
 * @see LIS3MDL_DATA_RATE_5_HZ
 * @see LIS3MDL_DATA_RATE_10_HZ
 * @see LIS3MDL_DATA_RATE_20_HZ
 * @see LIS3MDL_DATA_RATE_40_HZ
 * @see LIS3MDL_DATA_RATE_80_HZ
 */
uint8_t LIS3MDL::getDataRate() {
    I2Cdev::readBits(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_DO_BIT, LIS3MDL_DO_LENGTH, buffer);
    return buffer[0];
}

/**
 * @brief Enable the FAST_ODR on the device
 * 
 * @param en FAST_ODR enabled
 */
void LIS3MDL::enableFastODR(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_FAST_ODR_BIT, en);
}

/**
 * @brief Get if FAST_ODR is enabled on the device
 * 
 * @return true if FAST_ODR is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getFastODREnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_FAST_ODR_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Enable the SELF_TEST on the device
 * 
 * @param en SELF_TEST enabled
 */
void LIS3MDL::enableSelfTest(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_ST_BIT, en);
}

/**
 * @brief Get if SELF_TEST is enabled on the device
 * 
 * @return true if SELF_TEST is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getSelfTestEnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL1, LIS3MDL_ST_BIT, buffer);
    return buffer[0];
}


// ==============================
// === CONTROL2 REGISTER, R/W ===
// ==============================


/**
 * @brief Sets the scale of the magnetometer
 * 
 * @param rate the new scale. Note this must be one of four rates (0x00-03) specified in the datasheet
 * 
 * @see LIS3MDL_FULL_SCALE_4_G
 * @see LIS3MDL_FULL_SCALE_8_G
 * @see LIS3MDL_FULL_SCALE_12_G
 * @see LIS3MDL_FULL_SCALE_16_G
 */
void LIS3MDL::setFullScale(uint8_t scale) {
    I2Cdev::writeBits(devAddr, LIS3MDL_RA_CTRL2, LIS3MDL_FS_BIT, LIS3MDL_FS_LENGTH, scale);
}

/**
 * @brief Returns the current scale
 * 
 * @return the current scale (0x00-03)
 *
 * @see LIS3MDL_FULL_SCALE_4_G
 * @see LIS3MDL_FULL_SCALE_8_G
 * @see LIS3MDL_FULL_SCALE_12_G
 * @see LIS3MDL_FULL_SCALE_16_G
 */
uint8_t LIS3MDL::getFullScale() {
    I2Cdev::readBits(devAddr, LIS3MDL_RA_CTRL2, LIS3MDL_FS_BIT, LIS3MDL_FS_LENGTH, buffer);
    return buffer[0];
}

/**
 * @brief Write a single bit to the CTRL2 register, rebooting the device
 * Note: This wipes all the memory content and restores defaults
 */
void LIS3MDL::reboot() {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL2, LIS3MDL_REBOOT_BIT, 1);
}

/**
 * @brief Write a single bit to the CTRL2 register, soft resetting the device
 * Note: this resets configuration and user registers
 */
void LIS3MDL::reset() {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL2, LIS3MDL_SOFT_RST_BIT, 1);
}


// ==============================
// === CONTROL3 REGISTER, R/W ===
// ==============================


/**
 * @brief Enable low-power mode on the device
 * 
 * @param en low-power enabled
 */
void LIS3MDL::enableLowPowerMode(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL3, LIS3MDL_LP_BIT, en);
}

/**
 * @brief Get if low-power mode is enabled on the device
 * 
 * @return true if low-power mode is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getLowPowerModeEnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL3, LIS3MDL_LP_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Sets the SPI interface mode
 * 
 * @param mode the SPI interface. False (0) is the 4-wire interface
 * True (1) is the 3-wire interface 
 */
void LIS3MDL::setSPIInterfaceMode(bool mode) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL3, LIS3MDL_SIM_BIT, mode);
}

/**
 * @brief Returns the current SPI interface mode
 * 
 * @return the current mode (0x00-01). False (0) is the 4-wire interface
 * True (1) is the 3-wire interface
 */
bool LIS3MDL::getSPIInterfaceMode() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL3, LIS3MDL_SIM_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Sets the operating mode of the device
 * 
 * @param rate the new operating mode. Note this must be one of three rates (0x00-02) specified in the datasheet
 * 
 * @see LIS3MDL_MD_CC_MODE
 * @see LIS3MDL_MD_SC_MODE
 * @see LIS3MDL_MD_PD_MODE
 */
void LIS3MDL::setOperatingMode(uint8_t mode) {
    I2Cdev::writeBits(devAddr, LIS3MDL_RA_CTRL3, LIS3MDL_MD_BIT, LIS3MDL_MD_LENGTH, mode);
}

/**
 * @brief Returns the current operating mode
 * 
 * @return the current mode (0x00-02)
 *
 * @see LIS3MDL_MD_CC_MODE
 * @see LIS3MDL_MD_SC_MODE
 * @see LIS3MDL_MD_PD_MODE
 */
uint8_t LIS3MDL::getOperatingMode() {
    I2Cdev::readBits(devAddr, LIS3MDL_RA_CTRL3, LIS3MDL_MD_BIT, LIS3MDL_MD_LENGTH, buffer);
    return buffer[0];
}


// ==============================
// === CONTROL4 REGISTER, R/W ===
// ==============================


/**
 * @brief Set the Z-axis operating mode
 * 
 * @param mode the new operating mode. Note: This must be one of the four modes (0x00-03) designated by the datasheet
 * 
 * @see LIS3MDL_OMZ_LP_MODE
 * @see LIS3MDL_OMZ_MP_MODE
 * @see LIS3MDL_OMZ_HP_MODE
 * @see LIS3MDL_OMZ_UHP_MODE
 */
void LIS3MDL::setZOpMode(uint8_t mode) {
    I2Cdev::writeBits(devAddr, LIS3MDL_RA_CTRL4, LIS3MDL_OMZ_BIT, LIS3MDL_OMZ_LENGTH, mode);
}

/**
 * @brief Returns the Z-axis operating mode
 * 
 * @return the current operating mode (0x00-03)
 * 
 * @see LIS3MDL_OMZ_LP_MODE
 * @see LIS3MDL_OMZ_MP_MODE
 * @see LIS3MDL_OMZ_HP_MODE
 * @see LIS3MDL_OMZ_UHP_MODE
 */
uint8_t LIS3MDL::getZOpMode() {
    I2Cdev::readBits(devAddr, LIS3MDL_RA_CTRL4, LIS3MDL_OMZ_BIT, LIS3MDL_OMZ_LENGTH, buffer);
    return buffer[0];
}

/**
 * @brief Sets the data endianness
 * 
 * @param endianness The endianness of the data. False (0) is LSB first
 * True (1) is MSB first
 */
void LIS3MDL::setEndianness(bool endianness) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL4, LIS3MDL_BLE_BIT, endianness);
}

/**
 * @brief Returns the current data endianness
 * 
 * @return the current endianness (0x00-01). False (0) is LSB first
 * True (1) is MSB first
 */
bool LIS3MDL::getEndianness() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL4, LIS3MDL_BLE_BIT, buffer);
    return buffer[0];
}


// ===============================
// === CONTROL5 REGISTERS, R/W ===
// ===============================


/**
 * @brief Enable FAST_READ on the device
 * 
 * @param en FAST_READ enabled
 */
void LIS3MDL::enableFastRead(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL5, LIS3MDL_FAST_READ_BIT, en);
}

/**
 * @brief Get if FAST_READ is enabled on the device
 * 
 * @return true if FAST_READ mode is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getFastReadEnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL5, LIS3MDL_FAST_READ_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Enable Block Data Update on the device
 * 
 * @param en Block Data Update enabled
 */
void LIS3MDL::enableBlockDataUpdate(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_RA_CTRL5, LIS3MDL_BDU_BIT, en);
}

/**
 * @brief Get if Block Data Update is enabled on the device
 * 
 * @return true if Block Data Update mode is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getBlockDataUpdateEnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_CTRL5, LIS3MDL_BDU_BIT, buffer);
    return buffer[0];
}


// ===================================
// === STATUS REGISTERS, READ-ONLY ===
// ===================================


/**
 * @brief Return the entire status register of the device
 * 
 * @return The entire status register
 * @see LIS3MDL_ZYXOR_BIT
 * @see LIS3MDL_ZOR_BIT
 * @see LIS3MDL_YOR_BIT
 * @see LIS3MDL_XOR_BIT
 * @see LIS3MDL_ZYXDA_BIT
 * @see LIS3MDL_ZDA_BIT
 * @see LIS3MDL_YDA_BIT
 * @see LIS3MDL_XDA_BIT
 */
uint8_t LIS3MDL::getStatus() {
    I2Cdev::readByte(devAddr, LIS3MDL_RA_STATUS, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the X-, Y-, Z-axis overrun
 * 
 * @return The status of the X-, Y-, Z-axis overrun 
 */
bool LIS3MDL::getXYZDataOverrun() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_ZYXOR_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the Z-axis overrun
 * 
 * @return The status of the Z-axis overrun 
 */
bool LIS3MDL::getZDataOverrun() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_ZOR_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the Y-axis overrun
 * 
 * @return The status of the Y-axis overrun 
 */
bool LIS3MDL::getYDataOverrun() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_YOR_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the X-axis overrun
 * 
 * @return The status of the X-axis overrun 
 */
bool LIS3MDL::getXDataOverrun() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_XOR_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the X-, Y-, Z-axis data available
 * 
 * @return The status of the X-, Y-, Z-axis data available 
 */
bool LIS3MDL::getXYZDataAvailable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_ZYXDA_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the Z-axis data available
 * 
 * @return The status of the Z-axis data available 
 */
bool LIS3MDL::getZDataAvailable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_ZDA_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the Y-axis data available
 * 
 * @return The status of the Y-axis data available 
 */
bool LIS3MDL::getYDataAvailable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_YDA_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Get the status of the X-axis data available
 * 
 * @return The status of the X-axis data available 
 */
bool LIS3MDL::getXDataAvailable() {
    I2Cdev::readBit(devAddr, LIS3MDL_RA_STATUS, LIS3MDL_XDA_BIT, buffer);
    return buffer[0];
}


// ==================================
// === OUT_* REGISTERS, READ-ONLY ===
// ==================================


/**
 * @brief Get the total magnetic field detected by the device
 * 
 * @param x X-axis reading in mGauss
 * @param y Y-axis reading in mGauss
 * @param z Z-axis reading in mGauss
 */
void LIS3MDL::getMagneticField(uint16_t* x, uint16_t* y, uint16_t* z) {
    *x = getMagneticFieldX();
    *y = getMagneticFieldY();
    *z = getMagneticFieldZ();
}

/**
 * @brief Get the magnetic field for the X-axis
 * 
 * @return The strength of the magnetic field in the X-axis in mGauss
 */
uint16_t LIS3MDL::getMagneticFieldX() {
    I2Cdev::readBytes(devAddr, LIS3MDL_OUT_X_L, 2, buffer);
    uint16_t _val = !getEndianness() ? (((int16_t) buffer[1]) << 8) | buffer[0] : (((int16_t) buffer[0]) << 8) | buffer[1];

    switch (getFullScale()) {
        case LIS3MDL_FULL_SCALE_4_G:
            return _val / 6842 * 1000;
        case LIS3MDL_FULL_SCALE_8_G:
            return _val / 3421 * 1000;
        case LIS3MDL_FULL_SCALE_12_G:
            return _val / 2281 * 1000;
        case LIS3MDL_FULL_SCALE_16_G:
            return _val / 1711 * 1000;
        default:
            return _val / 6842 * 1000;
    };
}

/**
 * @brief Get the magnetic field for the Y-axis
 * 
 * @return The strength of the magnetic field in the Y-axis in mGauss
 */
uint16_t LIS3MDL::getMagneticFieldY() {
    I2Cdev::readBytes(devAddr, LIS3MDL_OUT_Y_L, 2, buffer);
    uint16_t _val = !getEndianness() ? (((int16_t) buffer[1]) << 8) | buffer[0] : (((int16_t) buffer[0]) << 8) | buffer[1];

    switch (getFullScale()) {
        case LIS3MDL_FULL_SCALE_4_G:
            return _val / 6842 * 1000;
        case LIS3MDL_FULL_SCALE_8_G:
            return _val / 3421 * 1000;
        case LIS3MDL_FULL_SCALE_12_G:
            return _val / 2281 * 1000;
        case LIS3MDL_FULL_SCALE_16_G:
            return _val / 1711 * 1000;
        default:
            return _val / 6842 * 1000;
    };
}

/**
 * @brief Get the magnetic field for the Z-axis
 * 
 * @return The strength of the magnetic field in the Z-axis in mGauss
 */
uint16_t LIS3MDL::getMagneticFieldZ() {
    I2Cdev::readBytes(devAddr, LIS3MDL_OUT_Z_L, 2, buffer);
    uint16_t _val = !getEndianness() ? (((int16_t) buffer[1]) << 8) | buffer[0] : (((int16_t) buffer[0]) << 8) | buffer[1];

    switch (getFullScale()) {
        case LIS3MDL_FULL_SCALE_4_G:
            return _val / 6842 * 1000;
        case LIS3MDL_FULL_SCALE_8_G:
            return _val / 3421 * 1000;
        case LIS3MDL_FULL_SCALE_12_G:
            return _val / 2281 * 1000;
        case LIS3MDL_FULL_SCALE_16_G:
            return _val / 1711 * 1000;
        default:
            return _val / 6842 * 1000;
    };
}

/**
 * @brief Get the device temperature as reported by the internal sensor
 * 
 * @return The temperature in degrees celsius
 */
uint16_t LIS3MDL::getTemperature() {
    I2Cdev::readBytes(devAddr, LIS3MDL_OUT_TEMP_OUT_L, 2, buffer);
    uint16_t _val = !getEndianness() ? (((int16_t) buffer[1]) << 8) | buffer[0] : (((int16_t) buffer[0]) << 8) | buffer[1];

    return _val / 8;
}


// =============================
// === INT_CFG REGISTER, R/W ===
// =============================


/**
 * @brief Enable interrupt generation for the X-axis on the device
 * 
 * @param en Interrupt generation for the X-axis enabled
 */
void LIS3MDL::enableInterruptGenerationX(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_XIEN_BIT, en);
}

/**
 * @brief Get if interrupt generation for the X-axis is enabled on the device
 * 
 * @return true if interrupt generation for the X-axis is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getInterruptGenerationEnableX() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_YIEN_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Enable interrupt generation for the Y-axis on the device
 * 
 * @param en Interrupt generation for the Y-axis enabled
 */
void LIS3MDL::enableInterruptGenerationY(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_YIEN_BIT, en);
}

/**
 * @brief Get if interrupt generation for the Y-axis is enabled on the device
 * 
 * @return true if interrupt generation for the Y-axis is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getInterruptGenerationEnableY() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_YIEN_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Enable interrupt generation for the Z-axis on the device
 * 
 * @param en Interrupt generation for the Z-axis enabled
 */
void LIS3MDL::enableInterruptGenerationZ(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_ZIEN_BIT, en);
}

/**
 * @brief Get if interrupt generation for the Z-axis is enabled on the device
 * 
 * @return true if interrupt generation for the Z-axis is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getInterruptGenerationEnableZ() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_ZIEN_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Sets the interrupt generation mode
 * 
 * @param mode if the interrupt generation is active HIGH (true) or LOW (false)
 */
void LIS3MDL::setInterruptMode(bool mode) {
    I2Cdev::writeBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_IEA_BIT, mode);
}

/**
 * @brief Returns the current interrupt generation mode
 * 
 * @return the current interrupt generation mode (0x00-01). False (0) is active LOW
 * True (1) is active HIGH
 */
bool LIS3MDL::getInterruptMode() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_IEA_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Enable interrupt latching.
 * When enabled, the interrupt can only be cleared by reading the INT_SRC register
 * 
 * @param en Interrupt latching enabled
 */
void LIS3MDL::enableLatchIntRequest(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_LIR_BIT, en);
}

/**
 * @brief Get if interrupt latch is enabled
 * 
 * @return true if interrupt latch is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getLatchIntRequestEnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_LIR_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Enable interrupt.
 * 
 * @param en Interrupt enabled
 */
void LIS3MDL::enableInterrupt(bool en) {
    I2Cdev::writeBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_IEN_BIT, en);
}

/**
 * @brief Get if interrupt is enabled
 * 
 * @return true if interrupt is enabled,
 * @return false otherwise
 */
bool LIS3MDL::getInterruptEnable() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_CFG, LIS3MDL_IEN_BIT, buffer);
    return buffer[0];
}


// =============================
// === INT_SRC REGISTER, R/W ===
// =============================

/**
 * @brief Returns the INT_SRC register to clear the interrupt
 * This function only needs to be called when the interrupt latch is enabled and the interrupt has been triggered
 * By reading the INT_SRC register, the interrupt will reset
 * 
 * @return the INT_SRC register
 */
uint8_t LIS3MDL::clearInt() {
    I2Cdev::readByte(devAddr, LIS3MDL_INT_SRC, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the value of the X-, Y-, and Z-axis values is positive relative to the interrupt threshold
 * True indicates the value is on the positive side.
 * False indicates the value has not exceed the interrupt threshold or that it is on the negative side
 */
void LIS3MDL::getPosIntThreshold(bool *x, bool *y, bool *z) {
    *x = getPosIntThresholdX();
    *y = getPosIntThresholdY();
    *z = getPosIntThresholdZ();
}

/**
 * @brief Returns if the value of the X-axis relative to its interrupt threshold is positive.
 * 
 * @return if the X-axis value is positive of its interrupt threshold.
 * @return True indicates the value is on the positive side.
 * @return False indicates the value has not exceed the interrupt threshold or that it is on the negative side
 */
bool LIS3MDL::getPosIntThresholdX() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_PTH_X_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the value of the Y-axis relative to its interrupt threshold is positive.
 * 
 * @return if the Y-axis value is positive of its interrupt threshold.
 * @return True indicates the value is on the positive side.
 * @return False indicates the value has not exceed the interrupt threshold or that it is on the negative side
 */
bool LIS3MDL::getPosIntThresholdY() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_PTH_Y_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the value of the Z-axis relative to its interrupt threshold is positive.
 * 
 * @return if the Z-axis value is positive of its interrupt threshold.
 * @return True indicates the value is on the positive side.
 * @return False indicates the value has not exceed the interrupt threshold or that it is on the negative side
 */
bool LIS3MDL::getPosIntThresholdZ() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_PTH_Z_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the value of the X-, Y-, and Z-axis values is negative relative to the interrupt threshold
 * True indicates the value is on the negative side.
 * False indicates the value has not exceed the interrupt threshold or that it is on the positive side
 */
void LIS3MDL::getNegIntThreshold(bool *x, bool *y, bool *z) {
    *x = getPosIntThresholdX();
    *y = getPosIntThresholdY();
    *z = getPosIntThresholdZ();
}

/**
 * @brief Returns if the value of the X-axis relative to its interrupt threshold is negative.
 * 
 * @return if the X-axis value is negative of its interrupt threshold.
 * @return True indicates the value is on the negative side.
 * @return False indicates the value has not exceed the interrupt threshold or that it is on the positive side
 */
bool LIS3MDL::getNegIntThresholdX() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_NTH_X_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the value of the Y-axis relative to its interrupt threshold is negative.
 * 
 * @return if the Y-axis value is negative of its interrupt threshold.
 * @return True indicates the value is on the negative side.
 * @return False indicates the value has not exceed the interrupt threshold or that it is on the positive side
 */
bool LIS3MDL::getNegIntThresholdY() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_NTH_Y_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the value of the Z-axis relative to its interrupt threshold is negative.
 * 
 * @return if the Z-axis value is negative of its interrupt threshold.
 * @return True indicates the value is on the negative side.
 * @return False indicates the value has not exceed the interrupt threshold or that it is on the positive side
 */
bool LIS3MDL::getNegIntThresholdZ() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_NTH_Z_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the internal measurement range has overflowed on the magnetic field reading.
 * 
 * @return True if the internal measurement range has overflowed,
 * @return false otherwise
 */
bool LIS3MDL::getInternalMeasurementRangeOverflow() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_MROI_BIT, buffer);
    return buffer[0];
}

/**
 * @brief Returns if the interrupt has been triggered
 * 
 * @return True if the interrupt has been triggered,
 * @return false otherwise
 */
bool LIS3MDL::getInterruptTriggered() {
    I2Cdev::readBit(devAddr, LIS3MDL_INT_SRC, LIS3MDL_INT_BIT, buffer);
    return buffer[0];
}


// ==============================
// === INT_THS REGISTERS, R/W ===
// ==============================


/**
 * @brief Set the interrupt threshold value for the device
 * 
 * @param ths The threshold at which the interrupt will be triggered, in Gauss
 */
void LIS3MDL::setInterruptThreshold(uint16_t ths) {
    I2Cdev::writeBytes(devAddr, LIS3MDL_OUT_THS_L, 2, ths);
}

/**
 * @brief Return the interrupt threshold value for the device
 * 
 * @return the interrupt threshold value, in Gauss
 */
uint16_t LIS3MDL::getInterruptThreshold() {
    I2Cdev::readBytes(devAddr, LIS3MDL_OUT_THS_L, 2, buffer);
    return (((int16_t) buffer[1]) << 8) | buffer[0];
}