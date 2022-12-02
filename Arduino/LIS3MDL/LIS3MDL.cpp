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

/* ============================================================================
   I2Cdev Class Quick Primer:

   The I2Cdev class provides simple methods for reading and writing from I2C
   device registers without messing with the underlying TWI/I2C functions. You
   just specify the device address, register address, and source or destination
   data according to which action you are doing. Here is the list of relevant
   function prototypes from the I2Cdev class (more info in the .cpp/.h files):

    static int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
    static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);

    static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
    static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
    static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
    static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
    static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
    static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
    static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
    static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);

   Note that ALL OF THESE METHODS ARE STATIC. No I2Cdev object is needed; just
   use the static class methods.
   
   Also note that the first two parameters of every one of these methods are
   the same: "devAddr" and "regAddr". For every method, you need to specify the
   target/slave address and the register address.
   
   If your device uses 8-bit registers, you will want to use the following:
       readBit, readBits, readByte, readBytes
       writeBit, writeBits, writeByte, writeBytes
       
   ...but if it uses 16-bit registers, you will want to use these instead:
       readBitW, readBitsW, readWord, readWords
       writeBitW, writeBitsW, writeWord, writeWords
       
   Here's a sample of how to use a few of the methods. Note that in each case, 
   the "buffer" variable is a uint8_t array or pointer, and the "value" variable
   (in three of the write examples) is a uint8_t single byte. The multi-byte
   write methods still require an array or pointer.

       READ 1 BIT FROM DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       bytesRead = I2Cdev::readBit(0x68, 0x02, 4, buffer);

       READ 3 BITS FROM DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       bytesRead = I2Cdev::readBits(0x68, 0x02, 4, 3, buffer);

       READ 1 BYTE FROM DEVICE 0x68, REGISTER 0x02
       bytesRead = I2Cdev::readByte(0x68, 0x02, buffer);

       READ 2 BYTES FROM DEVICE 0x68, REGISTER 0x02 (AND 0x03 FOR 2ND BYTE)
       bytesRead = I2Cdev::readBytes(0x68, 0x02, 2, buffer);

       WRITE 1 BIT TO DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       status = I2Cdev::writeBit(0x68, 0x02, 4, value);

       WRITE 3 BITS TO DEVICE 0x68, REGISTER 0x02, BIT POSITION 4
       status = I2Cdev::writeBits(0x68, 0x02, 4, 3, value);

       WRITE 1 BYTE TO DEVICE 0x68, REGISTER 0x02
       status = I2Cdev::writeByte(0x68, 0x02, value);

       WRITE 2 BYTES TO DEVICE 0x68, REGISTER 0x02 (AND 0x03 FOR 2ND BYTE)
       status = I2Cdev::writeBytes(0x68, 0x02, 2, buffer);
       
   The word-based methods are exactly the same, except they use 16-bit variables
   instead of 8-bit ones.

   ============================================================================ */

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
    bool _success = false;
    _success = _success && testConnection();
    _success = _success && I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL1, 0b00010000) &&
    _success = _success && I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL2, 0b00000000) &&
    _success = _success && I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL3, 0b00000011) &&
    _success = _success && I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL4, 0b00000000) &&
    _success = _success && I2Cdev::writeByte(devAddr, LIS3MDL_RA_CTRL5, 0b00000000);
    return _success;
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
uint8_t LIS3MDL::getDataRate() {
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
uint8_t LIS3MDL::getSPIInterfaceMode() {
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
uint8_t LIS3MDL::getEndianness() {
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