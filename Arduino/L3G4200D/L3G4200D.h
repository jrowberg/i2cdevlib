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
#define L3G4200D_HPM_BIT           2
#define L3G4200D_HPM_LENGTH        2
#define L3G4200D_HPCF_BIT          4
#define L3G4200D_HPCF_LENGTH       4
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

#define L3G4200D_HPM_HRF           0b00
#define L3G4200D_HPM_REFERENCE     0b01
#define L3G4200D_HPM_NORMAL        0b10
#define L3G4200D_HPM_AUTORESET     0b11

#define L3G4200D_HPCF1             0b0000
#define L3G4200D_HPCF2             0b0001
#define L3G4200D_HPCF3             0b0010
#define L3G4200D_HPCF4             0b0011
#define L3G4200D_HPCF5             0b0100
#define L3G4200D_HPCF6             0b0101
#define L3G4200D_HPCF7             0b0110
#define L3G4200D_HPCF8             0b0111
#define L3G4200D_HPCF9             0b1000
#define L3G4200D_HPCF10            0b1001

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

		// WHO_AM_I register, read-only
        uint8_t getDeviceID();
		
		// CTRL_REG1 register, r/w
		void setOutputDataRate(uint16_t rate);
		uint16_t getOutputDataRate();
		void setBandwidthCutOff(uint8_t cutOff);
		uint8_t getBandwidthCutOff();
		void setPowerDown(bool enabled);
		bool getPowerDown();
		void setZEnabled(bool enabled);
		bool getZEnabled();
		void setYEnabled(bool enabled);
		bool getYEnabled();
		void setXEnabled(bool enabled);
		bool getXEnabled();

		// CTRL_REG2 register, r/w
		void setHighPassMode(uint8_t mode);
		uint8_t getHighPassMode();
		void setHighPassFilterCutOffFrequencyLevel(uint8_t level);
		uint8_t getHighPassFilterCutOffFrequencyLevel();

		// CTRL_REG3 register, r/w
		void setInt1InterruptEnabled(bool enabled);
		bool getInt1InterruptEnabled();
		void setInt1BootStatusEnabled(bool enabled);
		bool getInt1BootStatusEnabled();
		void interruptActiveInt1Config();
		void setPushPull(bool pushPull);	// Alternative: Open Drain
		bool getIsPushPull();
		void setInt2DateReadyEnabled(bool enabled);
		bool getInt2DateReadyEnabled();	// This may be a typo on the datasheet
		void setInt2FIFOWatermarkEnabled(bool enabled);
		bool getInt2FIFOWatermarkEnabled();
		void setInt2FIFOInterruptEnabled(bool enabled);
		bool getInt2FIFOInterruptEnabled();
		void setInt2EmptyInterruptEnabled(bool enabled);
		bool getInt2EmptyInterruptEnabled();
		
		// CTRL_REG4 register, r/w
		void setBlockDataUpdateEnabled(bool enabled);
		bool getBlockDataUpdateEnabled();
		void setBigEndianEnabled(bool enabled);	// Alternative: Little Endian
		bool getBigEndianEnabled();
		void setFullScale(uint16_t scale);
		uint16_t getFullScale();
		void setSelfTestMode(uint8_t mode);
		uint8_t getSelfTestMode();
		void setSPIMode4WireEnabled(bool enabled);	// Alternative: 3 wire
		bool getSPIMode4WireEnabled();

		// CTRL_REG5 register, r/w
		void rebootMemoryContent();
		void setFIFOEnabled(bool enabled);
		bool getFIFOEnabled();
		void setHighPassFilterEnabled(bool enabled);
		bool getHighPassFilterEnabled();
		void setDataFilter(uint8_t filter);
		uint8_t getDataFilter();
		void setDataInterrupt(uint8_t filter);
		uint8_t getDataInterruptFilter();
		
		// REFERENCE/DATACAPTURE register, r/w
		void setInterruptRefernce(uint8_t reference);
		uint8_t getInterruptReference();
		
		// OUT_TEMP register, read-only
		uint8_t getTemperature();

		// STATUS register, read-only
		bool getXYZOverrun();
		bool getZOverrun();
		bool getYOverrun();
		bool getXOverrun();
		bool getXYZDataAvailable();
		bool getZDataAvailable();
		bool getYDataAvailable();
		bool getXDataAvailable();

        // OUT_* registers, read-only
        void getRate(int16_t* x, int16_t* y, int16_t* z);
        int16_t getRateX();
		int16_t getRateY();
		int16_t getRateZ();
		
		// FIFO_CTRL register, r/w
		void setFIFOMode(uint8_t mode);
		uint8_t getFIFOMode();
		void setFIFOThreshold(uint8_t thresh);
		uint8_t getFIFOThreshold();
		
		// FIFO_SRC register, read-only
		bool getFIFOWatermarkLow();	// Alternative: >= Watermark level
		bool getFIFOOverrun();
		bool getFIFOEmpty();
		uint8_t getFIFOStoredDataLevel();
		
		// TODO INT1_* registers

    private:
        uint8_t devAddr;
        uint8_t buffer[6];
};

#endif /* _L3G4200D_H_ */
