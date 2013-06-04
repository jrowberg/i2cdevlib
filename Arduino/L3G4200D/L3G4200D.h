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

#ifndef _L3G4200D_H_
#define _L3G4200D_H_

#include "I2Cdev.h"

#define L3G4200D_ADDRESS           0x69
#define L3G4200D_DEFAULT_ADDRESS   0x69

#define L3G4200D_RA_WHO_AM_I       0x0F
#define L3G4200D_RA_CTRL_REG1      0x20
#define L3G4200D_RA_CTRL_REG2      0x21
#define L3G4200D_RA_CTRL_REG3      0x22
#define L3G4200D_RA_CTRL_REG4      0x23
#define L3G4200D_RA_CTRL_REG5      0x24
#define L3G4200D_RA_REFERENCE      0x25
#define L3G4200D_RA_OUT_TEMP       0x26
#define L3G4200D_RA_STATUS_REG     0x27
#define L3G4200D_RA_OUT_X_L        0x28
#define L3G4200D_RA_OUT_X_H        0x29
#define L3G4200D_RA_OUT_Y_L        0x2A
#define L3G4200D_RA_OUT_Y_H        0x2B
#define L3G4200D_RA_OUT_Z_L        0x2C
#define L3G4200D_RA_OUT_Z_H        0x2D
#define L3G4200D_RA_FIFO_CTRL_REG  0x2E
#define L3G4200D_RA_FIFO_SRC_REG   0x2F
#define L3G4200D_RA_INT1_CFG       0x30
#define L3G4200D_RA_INT1_SRC       0x31
#define L3G4200D_RA_INT1_THS_XH    0x32
#define L3G4200D_RA_INT1_THS_XL    0X33
#define L3G4200D_RA_INT1_THS_YH    0X34
#define L3G4200D_RA_INT1_THS_YL    0x35
#define L3G4200D_RA_INT1_THS_ZH    0X36
#define L3G4200D_RA_INT1_THS_ZL    0x37
#define L3G4200D_RA_INT1_DURATION  0X38

#define L3G4200D_ODR_BIT           0
#define L3G4200D_ODR_LENGTH        2
#define L3G4200D_BW_BIT            2
#define L3G4200D_BW_LENGTH         2
#define L3G4200D_PD_BIT            4
#define L3G4200D_ZEN_BIT           5
#define L3G4200D_YEN_BIT           6
#define L3G4200D_XEN_BIT           7
#define L3G4200D_HPM_HPCF_BIT      0
#define L3G4200D_HPM_HPCF_LENGTH   8
#define L3G4200D_I1_INT1_BIT       0
#define L3G4200D_I1_BOOT_BIT       1
#define L3G4200D_H_LACTIVE_BIT     2
#define L3G4200D_PP_OD_BIT         3
#define L3G4200D_I2_DRDY_BIT       4
#define L3G4200D_I2_WTM_BIT        5
#define L3G4200D_I2_ORUN_BIT       6
#define L3G4200D_I2_EMPTY_BIT      7
#define L3G4200D_BDU_BIT           0
#define L3G4200D_BLE_BIT           1
#define L3G4200D_FS_BIT            2
#define L3G4200D_FS_LENGTH         2
#define L3G4200D_ST_BIT            5
#define L3G4200D_ST_LENGTH         2
#define L3G4200D_SIM_BIT           7
#define L3G4200D_BOOT_BIT          0
#define L3G4200D_FIFO_EN_BIT       1
#define L3G4200D_HPEN_BIT          3
#define L3G4200D_INT1_SEL_BIT      4
#define L3G4200D_INT1_SEL_LENGTH   2
#define L3G4200D_OUT_SEL_BIT       6
#define L3G4200D_OUT_SEL_LENGTH    2
#define L3G4200D_REF_BIT           0
#define L3G4200D_REF_LENGTH        8
#define L3G4200D_FIFO_MODE_BIT     0
#define L3G4200D_FIFO_MODE_LENGTH  3
#define L3G4200D_FIFO_WM_BIT       3
#define L3G4200D_FIFO_WM_LENGTH    5
#define L3G4200D_INT1_AND_OR_BIT   0
#define L3G4200D_INT1_LIR_BIT      1
#define L3G4200D_ZHIE_BIT          2
#define L3G4200D_ZLIE_BIT          3
#define L3G4200D_YHIE_BIT          4
#define L3G4200D_YLIE_BIT          5
#define L3G4200D_XHIE_BIT          6
#define L3G4200D_XLIE_BIT          7
#define L3G4200D_INT1_WAIT_BIT     0
#define L3G4200D_INT1_DUR_BIT      1
#define L3G4200D_INT1_DUR_LENGTH   7

#define L3G4200D_RATE_100          0b00
#define L3G4200D_RATE_200          0b01
#define L3G4200D_RATE_400          0b10
#define L3G4200D_RATE_800          0b11

#define L3G4200D_BW_LOW            0b00
#define L3G4200D_BW_MED_LOW        0b01
#define L3G4200D_BW_MED_HIGH       0b10
#define L3G4200D_BW_HIGH           0b11

#define L3G4200D_FS_250            0b00
#define L3G4200D_FS_500            0b01
#define L3G4200D_FS_2000           0b10

#define L3G4200D_SELF_TEST_NORMAL  0b00
#define L3G4200D_SELF_TEST_0       0b01
#define L3G4200D_SELF_TEST_1       0b11

#define L3G4200D_FM_BYPASS         0b000
#define L3G4200D_FM_FIFO           0b001
#define L3G4200D_FM_STREAM         0b010
#define L3G4200D_FM_STREAM_FIFO    0b011
#define L3G4200D_FM_BYPASS_STREAM  0b100

class L3G4200D {
    public:
        L3G4200D();
        L3G4200D(uint8_t address);

        void initialize();
        bool testConnection();

// ----------------------------------------------------------------------------
// STUB TODO:
// Declare methods to fully cover all available functionality provided by the
// device, according to the datasheet and/or register map. Unless there is very
// clear reason not to, try to follow the get/set naming convention for all
// values, for instance:
//   - uint8_t getThreshold()
//   - void setThreshold(uint8_t threshold)
//   - uint8_t getRate()
//   - void setRate(uint8_t rate)
//
// Some methods may be named differently if it makes sense. As long as all
// functionality is covered, that's the important part. The methods here are
// only examples and should not be kept for your real device.
// ----------------------------------------------------------------------------

        // MEASURE1 register, read-only
        uint8_t getMeasurement1();

        // MEASURE2 register, read-only
        uint8_t getMeasurement2();

        // MEASURE3 register, read-only
        uint8_t getMeasurement3();

        // CONFIG1 register, r/w
        void reset();               // <-- special method that resets entire device
        bool getFIFOEnabled();
        void setFIFOEnabled(bool enabled);

        // CONFIG2 register, r/w
        uint8_t getInterruptMode();
        void setInterruptMode(uint8_t mode);
        uint8_t getRate();
        void setRate(uint8_t rate);

        // DATA_* registers, r/w
        uint16_t getData();
        void setData(uint16_t value);

        // WHO_AM_I register, read-only
        uint8_t getDeviceID();

// ----------------------------------------------------------------------------
// STUB TODO:
// Declare private object helper variables or local storage for particular
// device settings, if required. All devices need a "devAddr" variable to store
// the I2C slave address for easy access. Most devices also need a buffer for
// reads (the I2Cdev class' read methods use a pointer for the storage location
// of any read data). The buffer should be of type "uint8_t" if the device uses
// 8-bit registers, or type "uint16_t" if the device uses 16-bit registers.
// Many devices will not require more member variables than this.
// ----------------------------------------------------------------------------
    private:
        uint8_t devAddr;
        uint8_t buffer[6];
};

#endif /* _L3G4200D_H_ */
