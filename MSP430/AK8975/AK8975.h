// I2Cdev library collection - AK8975 I2C device class header file
// Based on AKM AK8975/B datasheet, 12/2009
// 8/27/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-08-27 - initial release

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

#ifndef _AK8975_H_
#define _AK8975_H_

#include "I2Cdev.h"

#define AK8975_ADDRESS_00         0x0C
#define AK8975_ADDRESS_01         0x0D
#define AK8975_ADDRESS_10         0x0E // default for InvenSense MPU-6050 evaluation board
#define AK8975_ADDRESS_11         0x0F
#define AK8975_DEFAULT_ADDRESS    AK8975_ADDRESS_00

#define AK8975_RA_WIA             0x00
#define AK8975_RA_INFO            0x01
#define AK8975_RA_ST1             0x02
#define AK8975_RA_HXL             0x03
#define AK8975_RA_HXH             0x04
#define AK8975_RA_HYL             0x05
#define AK8975_RA_HYH             0x06
#define AK8975_RA_HZL             0x07
#define AK8975_RA_HZH             0x08
#define AK8975_RA_ST2             0x09
#define AK8975_RA_CNTL            0x0A
#define AK8975_RA_RSV             0x0B // RESERVED, DO NOT USE
#define AK8975_RA_ASTC            0x0C
#define AK8975_RA_TS1             0x0D // SHIPMENT TEST, DO NOT USE
#define AK8975_RA_TS2             0x0E // SHIPMENT TEST, DO NOT USE
#define AK8975_RA_I2CDIS          0x0F
#define AK8975_RA_ASAX            0x10
#define AK8975_RA_ASAY            0x11
#define AK8975_RA_ASAZ            0x12

#define AK8975_ST1_DRDY_BIT       0

#define AK8975_ST2_HOFL_BIT       3
#define AK8975_ST2_DERR_BIT       2

#define AK8975_CNTL_MODE_BIT      3
#define AK8975_CNTL_MODE_LENGTH   4

#define AK8975_MODE_POWERDOWN     0x0
#define AK8975_MODE_SINGLE        0x1
#define AK8975_MODE_SELFTEST      0x8
#define AK8975_MODE_FUSEROM       0xF

#define AK8975_ASTC_SELF_BIT      6

#define AK8975_I2CDIS_BIT         0

class AK8975 {
    public:
        AK8975();
        AK8975(uint8_t address);
        
        void initialize();
        bool testConnection();

        // WIA register
        uint8_t getDeviceID();
        
        // INFO register
        uint8_t getInfo();
        
        // ST1 register
        bool getDataReady();
        
        // H* registers
        void getHeading(int16_t *x, int16_t *y, int16_t *z);
        int16_t getHeadingX();
        int16_t getHeadingY();
        int16_t getHeadingZ();
        
        // ST2 register
        bool getOverflowStatus();
        bool getDataError();

        // CNTL register
        uint8_t getMode();
        void setMode(uint8_t mode);
        void reset();
        
        // ASTC register
        void setSelfTest(bool enabled);
        
        // I2CDIS
        void disableI2C(); // um, why...?
        
        // ASA* registers
        void getAdjustment(int8_t *x, int8_t *y, int8_t *z);
        void setAdjustment(int8_t x, int8_t y, int8_t z);
        uint8_t getAdjustmentX();
        void setAdjustmentX(uint8_t x);
        uint8_t getAdjustmentY();
        void setAdjustmentY(uint8_t y);
        uint8_t getAdjustmentZ();
        void setAdjustmentZ(uint8_t z);

    private:
        uint8_t devAddr;
        uint8_t buffer[6];
        uint8_t mode;
};

#endif /* _AK8975_H_ */
