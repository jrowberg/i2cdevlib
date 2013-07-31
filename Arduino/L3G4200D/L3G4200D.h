// I2Cdev library collection - L3G4200D I2C device class header file
// Based on STMicroelectronics L3G4200D datasheet rev. 3, 12/2010
// 7/31/2013 by Jonathan Arnett <j3rn@j3rn.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2013-07-31 - initial release

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
#define L3G4200D_RA_STATUS         0x27
#define L3G4200D_RA_OUT_X_L        0x28
#define L3G4200D_RA_OUT_X_H        0x29
#define L3G4200D_RA_OUT_Y_L        0x2A
#define L3G4200D_RA_OUT_Y_H        0x2B
#define L3G4200D_RA_OUT_Z_L        0x2C
#define L3G4200D_RA_OUT_Z_H        0x2D
#define L3G4200D_RA_FIFO_CTRL  	   0x2E
#define L3G4200D_RA_FIFO_SRC	   0x2F
#define L3G4200D_RA_INT1_CFG       0x30
#define L3G4200D_RA_INT1_SRC       0x31
#define L3G4200D_RA_INT1_THS_XH    0x32
#define L3G4200D_RA_INT1_THS_XL    0X33
#define L3G4200D_RA_INT1_THS_YH    0X34
#define L3G4200D_RA_INT1_THS_YL    0x35
#define L3G4200D_RA_INT1_THS_ZH    0X36
#define L3G4200D_RA_INT1_THS_ZL    0x37
#define L3G4200D_RA_INT1_DURATION  0X38

#define L3G4200D_ODR_BIT           7
#define L3G4200D_ODR_LENGTH        2
#define L3G4200D_BW_BIT            5
#define L3G4200D_BW_LENGTH         2
#define L3G4200D_PD_BIT            3
#define L3G4200D_ZEN_BIT           2
#define L3G4200D_YEN_BIT           1
#define L3G4200D_XEN_BIT           0

#define L3G4200D_RATE_100          0b00
#define L3G4200D_RATE_200          0b01
#define L3G4200D_RATE_400          0b10
#define L3G4200D_RATE_800          0b11

#define L3G4200D_BW_LOW            0b00
#define L3G4200D_BW_MED_LOW        0b01
#define L3G4200D_BW_MED_HIGH       0b10
#define L3G4200D_BW_HIGH           0b11

#define L3G4200D_HPM_BIT           5
#define L3G4200D_HPM_LENGTH        2
#define L3G4200D_HPCF_BIT          3
#define L3G4200D_HPCF_LENGTH       4

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

#define L3G4200D_I1_INT1_BIT       7
#define L3G4200D_I1_BOOT_BIT       6
#define L3G4200D_H_LACTIVE_BIT     5
#define L3G4200D_PP_OD_BIT         4
#define L3G4200D_I2_DRDY_BIT       3
#define L3G4200D_I2_WTM_BIT        2
#define L3G4200D_I2_ORUN_BIT       1
#define L3G4200D_I2_EMPTY_BIT      0

#define L3G4200D_PUSH_PULL         1
#define L3G4200D_OPEN_DRAIN        0

#define L3G4200D_BDU_BIT           7
#define L3G4200D_BLE_BIT           6
#define L3G4200D_FS_BIT            5
#define L3G4200D_FS_LENGTH         2
#define L3G4200D_ST_BIT            2
#define L3G4200D_ST_LENGTH         2
#define L3G4200D_SIM_BIT           0

#define L3G4200D_BIG_ENDIAN        1
#define L3G4200D_LITTLE_ENDIAN     0

#define L3G4200D_FS_250            0b00
#define L3G4200D_FS_500            0b01
#define L3G4200D_FS_2000           0b10

#define L3G4200D_SELF_TEST_NORMAL  0b00
#define L3G4200D_SELF_TEST_0       0b01
#define L3G4200D_SELF_TEST_1       0b11

#define L3G4200D_SPI_4_WIRE        1
#define L3G4200D_SPI_3_WIRE        0

#define L3G4200D_BOOT_BIT          7
#define L3G4200D_FIFO_EN_BIT       6
#define L3G4200D_HPEN_BIT          4
#define L3G4200D_INT1_SEL_BIT      3
#define L3G4200D_INT1_SEL_LENGTH   2
#define L3G4200D_OUT_SEL_BIT       1
#define L3G4200D_OUT_SEL_LENGTH    2

#define L3G4200D_NON_HIGH_PASS     0b00
#define L3G4200D_HIGH_PASS         0b01
#define L3G4200D_LOW_PASS          0b10
#define L3G4200D_LOW_HIGH_PASS     0b11

#define L3G4200D_ZYXOR_BIT         7
#define L3G4200D_ZOR_BIT           6
#define L3G4200D_YOR_BIT           5
#define L3G4200D_XOR_BIT           4
#define L3G4200D_ZYXDA_BIT         3
#define L3G4200D_ZDA_BIT           2
#define L3G4200D_YDA_BIT           1
#define L3G4200D_XDA_BIT           0

#define L3G4200D_FIFO_MODE_BIT     7
#define L3G4200D_FIFO_MODE_LENGTH  3
#define L3G4200D_FIFO_WTM_BIT      4
#define L3G4200D_FIFO_WTM_LENGTH   5

#define L3G4200D_FM_BYPASS         0b000
#define L3G4200D_FM_FIFO           0b001
#define L3G4200D_FM_STREAM         0b010
#define L3G4200D_FM_STREAM_FIFO    0b011
#define L3G4200D_FM_BYPASS_STREAM  0b100

#define L3G4200D_FIFO_STATUS_BIT   7
#define L3G4200D_FIFO_OVRN_BIT     6
#define L3G4200D_FIFO_EMPTY_BIT    5
#define L3G4200D_FIFO_FSS_BIT      4
#define L3G4200D_FIFO_FSS_LENGTH   5

#define L3G4200D_INT1_AND_OR_BIT   7
#define L3G4200D_INT1_LIR_BIT      6
#define L3G4200D_ZHIE_BIT          5
#define L3G4200D_ZLIE_BIT          4
#define L3G4200D_YHIE_BIT          3
#define L3G4200D_YLIE_BIT          2
#define L3G4200D_XHIE_BIT          1
#define L3G4200D_XLIE_BIT          0

#define L3G4200D_INT1_OR           0
#define L3G4200D_INT1_AND          1

#define L3G4200D_INT1_IA_BIT       6
#define L3G4200D_INT1_ZH_BIT       5
#define L3G4200D_INT1_ZL_BIT       4
#define L3G4200D_INT1_YH_BIT       3
#define L3G4200D_INT1_YL_BIT       2
#define L3G4200D_INT1_XH_BIT       1
#define L3G4200D_INT1_XL_BIT       0

#define L3G4200D_INT1_WAIT_BIT     7
#define L3G4200D_INT1_DUR_BIT      6
#define L3G4200D_INT1_DUR_LENGTH   7


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
		void setBandwidthCutOffMode(uint8_t mode);
		uint8_t getBandwidthCutOffMode();
		float getBandwidthCutOff();
		void setPowerOn(bool on);
		bool getPowerOn();
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
		void setINT1InterruptEnabled(bool enabled);
		bool getINT1InterruptEnabled();
		void setINT1BootStatusEnabled(bool enabled);
		bool getINT1BootStatusEnabled();
		void interruptActiveINT1Config();
		void setOutputMode(bool mode);
		bool getOutputMode();
		void setINT2DataReadyEnabled(bool enabled);
		bool getINT2DataReadyEnabled();
		void setINT2FIFOWatermarkInterruptEnabled(bool enabled);
		bool getINT2FIFOWatermarkInterruptEnabled();
		void setINT2FIFOOverrunInterruptEnabled(bool enabled);
		bool getINT2FIFOOverrunInterruptEnabled();
		void setINT2FIFOEmptyInterruptEnabled(bool enabled);
		bool getINT2FIFOEmptyInterruptEnabled();
		
		// CTRL_REG4 register, r/w
		void setBlockDataUpdateEnabled(bool enabled);
		bool getBlockDataUpdateEnabled();
		void setEndianMode(bool endianness);
		bool getEndianMode();
		void setFullScale(uint16_t scale);
		uint16_t getFullScale();
		void setSelfTestMode(uint8_t mode);
		uint8_t getSelfTestMode();
		void setSPIMode(bool mode);
		bool getSPIMode();

		// CTRL_REG5 register, r/w
		void rebootMemoryContent();
		void setFIFOEnabled(bool enabled);
		bool getFIFOEnabled();
		void setHighPassFilterEnabled(bool enabled);
		bool getHighPassFilterEnabled();
		void setDataFilter(uint8_t filter);
		uint8_t getDataFilter();
		
		// REFERENCE/DATACAPTURE register, r/w
		void setInterruptReference(uint8_t reference);
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
        void getAngularVelocity(int16_t* x, int16_t* y, int16_t* z);
        int16_t getAngularVelocityX();
		int16_t getAngularVelocityY();
		int16_t getAngularVelocityZ();
		
		// FIFO_CTRL register, r/w
		void setFIFOMode(uint8_t mode);
		uint8_t getFIFOMode();
		void setFIFOThreshold(uint8_t wtm);
		uint8_t getFIFOThreshold();
		
		// FIFO_SRC register, read-only
		bool getFIFOAtWatermark();
		bool getFIFOOverrun();
		bool getFIFOEmpty();
		uint8_t getFIFOStoredDataLevel();
		
		// INT1_CFG register, r/w
		void setInterruptCombination(bool combination);
		bool getInterruptCombination();
		void setInterruptRequestLatched(bool latched);
		bool getInterruptRequestLatched();
		void setZHighInterruptEnabled(bool enabled);
		bool getZHighInterruptEnabled();
		void setYHighInterruptEnabled(bool enabled);
		bool getYHighInterruptEnabled();
		void setXHighInterruptEnabled(bool enabled);
		bool getXHighInterruptEnabled();
		void setZLowInterruptEnabled(bool enabled);
		bool getZLowInterruptEnabled();
		void setYLowInterruptEnabled(bool enabled);
		bool getYLowInterruptEnabled();
		void setXLowInterruptEnabled(bool enabled);
		bool getXLowInterruptEnabled();

		// INT1_SRC register, read-only
		bool getInterruptActive();
		bool getZHigh();
		bool getZLow();
		bool getYHigh();
		bool getYLow();
		bool getXHigh();
		bool getXLow();
		
		// INT1_THS_* registers, r/w
		void setXHighThreshold(uint8_t threshold);
		uint8_t getXHighThreshold();
		void setXLowThreshold(uint8_t threshold);
		uint8_t getXLowThreshold();
		void setYHighThreshold(uint8_t threshold);
		uint8_t getYHighThreshold();
		void setYLowThreshold(uint8_t threshold);
		uint8_t getYLowThreshold();
		void setZHighThreshold(uint8_t threshold);
		uint8_t getZHighThreshold();
		void setZLowThreshold(uint8_t threshold);
		uint8_t getZLowThreshold();

		// INT1_DURATION register, r/w
		void setDuration(uint8_t duration);
		uint8_t getDuration();
		void setWaitEnabled(bool enabled);
		bool getWaitEnabled();

    private:
        uint8_t devAddr;
        uint8_t buffer[6];
};

#endif /* _L3G4200D_H_ */
