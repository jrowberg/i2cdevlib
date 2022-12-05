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

#ifndef LIS3MDL_H
#define LIS3MDL_H

#include "I2Cdev.h"


// ========================
// === DEVICE ADDRESSES ===
// ========================


#define LIS3MDL_ADDRESS_AD0_LOW     0x1C
#define LIS3MDL_ADDRESS_AD0_HIGH    0x1E
#define LIS3MDL_DEFAULT_ADDRESS     0x1C
#define LIS3MDL_DEVICE_ID           0x3D


// ========================
// === REGISTER MAPPING ===
// ========================


#define LIS3MDL_RA_WHO_AM_I     0x0F
#define LIS3MDL_RA_CTRL1        0x20
#define LIS3MDL_RA_CTRL2        0x21
#define LIS3MDL_RA_CTRL3        0x22
#define LIS3MDL_RA_CTRL4        0x23
#define LIS3MDL_RA_CTRL5        0x24
#define LIS3MDL_RA_STATUS       0x27
#define LIS3MDL_OUT_X_L         0x28
#define LIS3MDL_OUT_X_H         0x29
#define LIS3MDL_OUT_Y_L         0x2A
#define LIS3MDL_OUT_Y_H         0x2B
#define LIS3MDL_OUT_Z_L         0x2C
#define LIS3MDL_OUT_Z_H         0x2D
#define LIS3MDL_OUT_TEMP_OUT_L  0x2E
#define LIS3MDL_OUT_TEMP_OUT_H  0x2F
#define LIS3MDL_INT_CFG         0x30
#define LIS3MDL_INT_SRC         0x31
#define LIS3MDL_OUT_THS_L       0x32
#define LIS3MDL_OUT_THS_H       0x33


// ================================
// === REGISTER BIT DEFINITIONS ===
// ================================


// --- CTRL_REG1 ---
#define LIS3MDL_TEMP_EN_BIT     7 
#define LIS3MDL_OM_BIT          6
#define LIS3MDL_OM_LENGTH       2
#define LIS3MDL_DO_BIT          4
#define LIS3MDL_DO_LENGTH       3
#define LIS3MDL_FAST_ODR_BIT    1
#define LIS3MDL_ST_BIT          0

// --- CTRL_REG2 ---
#define LIS3MDL_FS_BIT          6
#define LIS3MDL_FS_LENGTH       2
#define LIS3MDL_REBOOT_BIT      3
#define LIS3MDL_SOFT_RST_BIT    2

// --- CTRL_REG3 ---
#define LIS3MDL_LP_BIT      5
#define LIS3MDL_SIM_BIT     2
#define LIS3MDL_MD_BIT      1
#define LIS3MDL_MD_LENGTH   2

// --- CTRL_REG4 ---
#define LIS3MDL_OMZ_BIT     3
#define LIS3MDL_OMZ_LENGTH  2
#define LIS3MDL_BLE_BIT     1

// --- CTRL_REG5 ---
#define LIS3MDL_FAST_READ_BIT   7
#define LIS3MDL_BDU_BIT         6

// --- STATUS_REG ---
#define LIS3MDL_ZYXOR_BIT   7
#define LIS3MDL_ZOR_BIT     6
#define LIS3MDL_YOR_BIT     5
#define LIS3MDL_XOR_BIT     4
#define LIS3MDL_ZYXDA_BIT   3
#define LIS3MDL_ZDA_BIT     2
#define LIS3MDL_YDA_BIT     1
#define LIS3MDL_XDA_BIT     0

// --- INT_CFG ---
#define LIS3MDL_XIEN_BIT    7
#define LIS3MDL_YIEN_BIT    6
#define LIS3MDL_ZIEN_BIT    5
#define LIS3MDL_IEA_BIT     2
#define LIS3MDL_LIR_BIT     1
#define LIS3MDL_IEN_BIT     0

// --- INT_SRC ---
#define LIS3MDL_PTH_X_BIT   7
#define LIS3MDL_PTH_Y_BIT   6
#define LIS3MDL_PTH_Z_BIT   5
#define LIS3MDL_NTH_X_BIT   4
#define LIS3MDL_NTH_Y_BIT   3
#define LIS3MDL_NTH_Z_BIT   2
#define LIS3MDL_MROI_BIT    1
#define LIS3MDL_INT_BIT     0

// --- INT_THS_L ---
#define LIS3MDL_THS_L_BIT       7
#define LIS3MDL_THS_L_LENGTH    8

// --- INT_THS_H ---
#define LIS3MDL_THS_H_BIT       6
#define LIS3MDL_THS_H_LENGTH    7


// =======================================
// === PREDEFINED CONFIGURATION VALUES ===
// =======================================


// --- Table 21: X and Y axes operating mode selection ---
#define LIS3MDL_OM_LP_MODE      0x00 // Low-power mode
#define LIS3MDL_OM_MP_MODE      0x01 // Medium-performance mode
#define LIS3MDL_OM_HP_MODE      0x02 // High-performance mode
#define LIS3MDL_OM_UHP_MODE     0x03 // Ultra-high-performance mode

// --- Table 22: Output data rate configuration
#define LIS3MDL_DATA_RATE_0_625_HZ  0x00
#define LIS3MDL_DATA_RATE_1_25_HZ   0x01
#define LIS3MDL_DATA_RATE_2_5_HZ    0x02
#define LIS3MDL_DATA_RATE_5_HZ      0x03
#define LIS3MDL_DATA_RATE_10_HZ     0x04
#define LIS3MDL_DATA_RATE_20_HZ     0x05
#define LIS3MDL_DATA_RATE_40_HZ     0x06
#define LIS3MDL_DATA_RATE_80_HZ     0x07

// --- Table 25: Full-scale selection ---
#define LIS3MDL_FULL_SCALE_4_G  0x00
#define LIS3MDL_FULL_SCALE_8_G  0x01
#define LIS3MDL_FULL_SCALE_12_G 0x02
#define LIS3MDL_FULL_SCALE_16_G 0x03

// --- Table 28: System operating mode selection ---
#define LIS3MDL_MD_CC_MODE  0x00 // Continuous-conversion mode
#define LIS3MDL_MD_SC_MODE  0x01 // Single-conversion mode (only valid for 0.625Hz to 80Hz)
#define LIS3MDL_MD_PD_MODE  0x02 // Power-down mode

// --- Table 31: Z-axis operating mode selection ---
#define LIS3MDL_OMZ_LP_MODE     0x00 // Low-power mode
#define LIS3MDL_OMZ_MP_MODE     0x01 // Medium-performance mode
#define LIS3MDL_OMZ_HP_MODE     0x02 // High-performance mode
#define LIS3MDL_OMZ_UHP_MODE    0x03 // Ultra-high-performance mode

class LIS3MDL {
    public:
        LIS3MDL();
        LIS3MDL(uint8_t address);

        bool initialize();
        bool testConnection();

        // WHO_AM_I register, read-only
        uint8_t getDeviceID();

        // CONTROL1 register, r/w
        void enableTempSensor(bool en);
        bool getTempSensorEnable();
        void setXYOpMode(uint8_t mode);
        uint8_t getXYOpMode();
        void setDataRate(uint8_t rate);
        uint8_t getDataRate();
        void enableFastODR(bool en);
        bool getFastODREnable();
        void enableSelfTest(bool en);
        bool getSelfTestEnable();

        // CONTROL2 register, r/w
        void setFullScale(uint8_t scale);
        uint8_t getFullScale();
        void reboot();
        void reset();

        // CONTROL3 register, r/w
        void enableLowPowerMode(bool en);
        bool getLowPowerModeEnable();
        void setSPIInterfaceMode(bool mode);
        bool getSPIInterfaceMode();
        void setOperatingMode(uint8_t mode);
        uint8_t getOperatingMode();

        // CONTROL4 register, r/w
        void setZOpMode(uint8_t mode);
        uint8_t getZOpMode();
        void setEndianness(bool endianness);
        bool getEndianness();

        // CONTROL5 register, r/w
        void enableFastRead(bool en);
        bool getFastReadEnable();
        void enableBlockDataUpdate(bool en);
        bool getBlockDataUpdateEnable();

        // STATUS register, read-only
        uint8_t getStatus();
        bool getXYZDataOverrun();
        bool getZDataOverrun();
        bool getYDataOverrun();
        bool getXDataOverrun();
        bool getXYZDataAvailable();
        bool getZDataAvailable();
        bool getYDataAvailable();
        bool getXDataAvailable();

        // OUT_* register, read-only
        void getMagneticField(uint16_t* x, uint16_t* y, uint16_t* z);
        uint16_t getMagneticFieldX();
        uint16_t getMagneticFieldY();
        uint16_t getMagneticFieldZ();
        uint16_t getTemperature();

        // INT_CFG register, r/w
        void enableInterruptGenerationX(bool en);
        bool getInterruptGenerationEnableX();
        void enableInterruptGenerationY(bool en);
        bool getInterruptGenerationEnableY();
        void enableInterruptGenerationZ(bool en);
        bool getInterruptGenerationEnableZ();
        void setInterruptMode(bool mode);
        bool getInterruptMode();
        void enableLatchIntRequest(bool en);
        bool getLatchIntRequestEnable();
        void enableInterrupt(bool en);
        bool getInterruptEnable();

        // INT_SRC register, r/w
        uint8_t clearInt();
        bool getPosIntThreshold(bool *x, bool *y, bool *z);
        bool getPosIntThresholdX(); 
        bool getPosIntThresholdY(); 
        bool getPosIntThresholdZ();
        bool getNegIntThreshold(bool *x, bool *y, bool *z);
        bool getNegIntThresholdX(); 
        bool getNegIntThresholdY(); 
        bool getNegIntThresholdZ();
        bool getInternalMeasurementRangeOverflow();
        bool getInterruptTriggered();

        // INT_THS registers, r/w
        void setInterruptThreshold(uint16_t ths);
        uint16_t getInterruptThreshold();

    private:
        uint8_t devAddr;
        uint8_t buffer[6];
};

#endif /* LIS3MDL_H */