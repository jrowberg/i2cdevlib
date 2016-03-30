// I2Cdev library collection - L3GD20H I2C device class header file
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

#ifndef _L3GD20H_H_
#define _L3GD20H_H_

#include "I2Cdev.h"

#define L3GD20H_ADDRESS           0x6B // I think this is correct.  See SAD in doc.
#define L3GD20H_DEFAULT_ADDRESS   0x6B // I think this is correct.  See SAD in doc.

#define L3GD20H_RA_WHO_AM_I       0x0F
#define L3GD20H_RA_CTRL1      0x20
#define L3GD20H_RA_CTRL2      0x21
#define L3GD20H_RA_CTRL3      0x22
#define L3GD20H_RA_CTRL4      0x23
#define L3GD20H_RA_CTRL5      0x24
#define L3GD20H_RA_REFERENCE      0x25
#define L3GD20H_RA_OUT_TEMP       0x26
#define L3GD20H_RA_STATUS         0x27
#define L3GD20H_RA_OUT_X_L        0x28
#define L3GD20H_RA_OUT_X_H        0x29
#define L3GD20H_RA_OUT_Y_L        0x2A
#define L3GD20H_RA_OUT_Y_H        0x2B
#define L3GD20H_RA_OUT_Z_L        0x2C
#define L3GD20H_RA_OUT_Z_H        0x2D
#define L3GD20H_RA_FIFO_CTRL  	   0x2E
#define L3GD20H_RA_FIFO_SRC	   0x2F
#define L3GD20H_RA_IG_CFG       0x30
#define L3GD20H_RA_IG_SRC       0x31
#define L3GD20H_RA_IG_THS_XH    0x32
#define L3GD20H_RA_IG_THS_XL    0X33
#define L3GD20H_RA_IG_THS_YH    0X34
#define L3GD20H_RA_IG_THS_YL    0x35
#define L3GD20H_RA_IG_THS_ZH    0X36
#define L3GD20H_RA_IG_THS_ZL    0x37
#define L3GD20H_RA_IG_DURATION  0X38
#define L3GD20H_RA_LOW_ODR		0x39

#define L3GD20H_ODR_BIT           7
#define L3GD20H_ODR_LENGTH        2
#define L3GD20H_BW_BIT            5
#define L3GD20H_BW_LENGTH         2
#define L3GD20H_PD_BIT            3
#define L3GD20H_ZEN_BIT           2
#define L3GD20H_YEN_BIT           1
#define L3GD20H_XEN_BIT           0

#define L3GD20H_RATE_100_12          0b00 //selection of high vs low rate is via Low_ODR
#define L3GD20H_RATE_200_25          0b01 //selection of high vs low rate is via Low_ODR
#define L3GD20H_RATE_400_50          0b10 //selection of high vs low rate is via Low_ODR
#define L3GD20H_RATE_800_50          0b11 //selection of high vs low rate is via Low_ODR

#define L3GD20H_BW_LOW            0b00
#define L3GD20H_BW_MED_LOW        0b01
#define L3GD20H_BW_MED_HIGH       0b10
#define L3GD20H_BW_HIGH           0b11

#define L3GD20H_HPM_BIT           5
#define L3GD20H_HPM_LENGTH        2
#define L3GD20H_HPCF_BIT          3
#define L3GD20H_HPCF_LENGTH       4

#define L3GD20H_HPM_HRF           0b00 //this resets on reading REFERENCE
#define L3GD20H_HPM_REFERENCE     0b01
#define L3GD20H_HPM_NORMAL        0b10
#define L3GD20H_HPM_AUTORESET     0b11

#define L3GD20H_HPCF1             0b0000
#define L3GD20H_HPCF2             0b0001
#define L3GD20H_HPCF3             0b0010
#define L3GD20H_HPCF4             0b0011
#define L3GD20H_HPCF5             0b0100
#define L3GD20H_HPCF6             0b0101
#define L3GD20H_HPCF7             0b0110
#define L3GD20H_HPCF8             0b0111
#define L3GD20H_HPCF9             0b1000
#define L3GD20H_HPCF10            0b1001

#define L3GD20H_INT1_IG_BIT       7
#define L3GD20H_INT1_BOOT_BIT     6
#define L3GD20H_H_LACTIVE_BIT     5
#define L3GD20H_PP_OD_BIT         4
#define L3GD20H_INT2_DRDY_BIT     3
#define L3GD20H_INT2_FTH_BIT      2
#define L3GD20H_INT2_ORUN_BIT     1
#define L3GD20H_INT2_EMPTY_BIT    0

#define L3GD20H_PUSH_PULL         0
#define L3GD20H_OPEN_DRAIN        1

#define L3GD20H_BDU_BIT           7
#define L3GD20H_BLE_BIT           6
#define L3GD20H_FS_BIT            5
#define L3GD20H_FS_LENGTH         2
#define L3GD20H_IMPEN_BIT		  3 //new for this IC: Level sensitive latched enalble
#define L3GD20H_ST_BIT            2
#define L3GD20H_ST_LENGTH         2
#define L3GD20H_SIM_BIT           0

#define L3GD20H_BIG_ENDIAN        1
#define L3GD20H_LITTLE_ENDIAN     0

#define L3GD20H_FS_250            0b00
#define L3GD20H_FS_500            0b01
#define L3GD20H_FS_2000           0b10

#define L3GD20H_SELF_TEST_NORMAL  0b00
#define L3GD20H_SELF_TEST_0       0b01
#define L3GD20H_SELF_TEST_1       0b11

#define L3GD20H_SPI_4_WIRE        0
#define L3GD20H_SPI_3_WIRE        1

#define L3GD20H_BOOT_BIT          7
#define L3GD20H_FIFO_EN_BIT       6
#define L3GD20H_STOPONFTH_BIT	  5
#define L3GD20H_HPEN_BIT          4
#define L3GD20H_IG_SEL_BIT        3
#define L3GD20H_IG_SEL_LENGTH     2
#define L3GD20H_OUT_SEL_BIT       1
#define L3GD20H_OUT_SEL_LENGTH    2

#define L3GD20H_NON_HIGH_PASS     0b00 
#define L3GD20H_HIGH_PASS         0b01 
#define L3GD20H_LOW_PASS          0b10 //depends on HPEN
#define L3GD20H_LOW_HIGH_PASS     0b11 //depends on HPEN

#define L3GD20H_ZYXOR_BIT         7
#define L3GD20H_ZOR_BIT           6
#define L3GD20H_YOR_BIT           5
#define L3GD20H_XOR_BIT           4
#define L3GD20H_ZYXDA_BIT         3
#define L3GD20H_ZDA_BIT           2
#define L3GD20H_YDA_BIT           1
#define L3GD20H_XDA_BIT           0

#define L3GD20H_FIFO_MODE_BIT     7
#define L3GD20H_FIFO_MODE_LENGTH  3
#define L3GD20H_FIFO_TH_BIT       4
#define L3GD20H_FIFO_TH_LENGTH    5

#define L3GD20H_FM_BYPASS         0b000
#define L3GD20H_FM_FIFO           0b001
#define L3GD20H_FM_STREAM         0b010
#define L3GD20H_FM_STREAM_FIFO    0b011
#define L3GD20H_FM_BYPASS_STREAM  0b100
#define L3GD20H_FM_DYNAMIC_STREAM 0b110
#define L3GD20H_FM_BYPASS_FIFO    0b111


#define L3GD20H_FIFO_TH_STATUS_BIT   7
#define L3GD20H_OVRN_BIT     		 6
#define L3GD20H_EMPTY_BIT    		 5
#define L3GD20H_FIFO_FSS_BIT      	 4
#define L3GD20H_FIFO_FSS_LENGTH   	 5

#define L3GD20H_AND_OR_BIT   	  7
#define L3GD20H_LIR_BIT      	  6
#define L3GD20H_ZHIE_BIT          5
#define L3GD20H_ZLIE_BIT          4
#define L3GD20H_YHIE_BIT          3
#define L3GD20H_YLIE_BIT          2
#define L3GD20H_XHIE_BIT          1
#define L3GD20H_XLIE_BIT          0

#define L3GD20H_AND_OR_OR 		  0
#define L3GD20H_AND_OR_AND     	  1

#define L3GD20H_IA_BIT            6
#define L3GD20H_ZH_BIT            5
#define L3GD20H_ZL_BIT            4
#define L3GD20H_YH_BIT            3
#define L3GD20H_YL_BIT            2
#define L3GD20H_XH_BIT            1
#define L3GD20H_XL_BIT            0

#define L3GD20H_DCRM_BIT		  7

#define L3GD20H_DCRM_RESET		  0
#define L3GD20H_DCRM_DEC		  1

#define L3GD20H_WAIT_BIT          7
#define L3GD20H_DUR_BIT           6
#define L3GD20H_DUR_LENGTH        7

#define L3GD20H_LOW_ODR_BIT		  0
#define L3GD20H_SW_RESET_BIT	  2
#define L3GD20H_I2C_DIS_BIT		  3
#define L3GD20H_DRDY_HL_BIT		  5



class L3GD20H {
    public:
        L3GD20H();
        L3GD20H(uint8_t address);

        void initialize();
        bool testConnection();

		// WHO_AM_I register, read-only
        uint8_t getDeviceID();
		
		// CTRL1 register, r/w
		void setOutputDataRate(uint16_t rate);
		uint16_t getOutputDataRate();
		void setBandwidthCutOffMode(uint8_t mode);
		uint8_t getBandwidthCutOffMode();
		// float getBandwidthCutOff();
		void setPowerOn(bool on);
		bool getPowerOn();
		void setZEnabled(bool enabled);
		bool getZEnabled();
		void setYEnabled(bool enabled);
		bool getYEnabled();
		void setXEnabled(bool enabled);
		bool getXEnabled();

		// CTRL2 register, r/w
		void setHighPassMode(uint8_t mode);
		uint8_t getHighPassMode();
		void setHighPassFilterCutOffFrequencyLevel(uint8_t level);
		uint8_t getHighPassFilterCutOffFrequencyLevel();

		// CTRL3 register, r/w
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
		
		// CTRL4 register, r/w
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

		// CTRL5 register, r/w
		void rebootMemoryContent();
		void setFIFOEnabled(bool enabled);
		bool getFIFOEnabled();
		void setStopOnFIFOThresholdEnabled(bool enabled);
		bool getStopOnFIFOThresholdEnabled();
		void setHighPassFilterEnabled(bool enabled);
		bool getHighPassFilterEnabled();
		void setDataFilter(uint8_t filter);
		uint8_t getDataFilter();
		
		// REFERENCE/DATACAPTURE register, r/w
		void setHighPassFilterReference(uint8_t reference);
		uint8_t getHighPassFilterReference();
		
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
		
		// IG_CFG register, r/w
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

		// IG_SRC register, read-only
		bool getInterruptActive();
		bool getZHigh();
		bool getZLow();
		bool getYHigh();
		bool getYLow();
		bool getXHigh();
		bool getXLow();
		
		// IG_THS_* registers, r/w
		void setDecrementMode(bool mode);
		bool getDecrementMode();
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

		// IG_DURATION register, r/w
		void setDuration(uint8_t duration);
		uint8_t getDuration();
		void setWaitEnabled(bool enabled);
		bool getWaitEnabled();

		// LOW_ODR register, r/w
		void setINT2DataReadyActiveLowEnabled(bool enabled);
		bool getINT2DataReadyActiveLowEnabled();
		void setSPIOnlyEnabled(bool enabled);
		bool getSPIOnlyEnabled();
		void setSoftwareReset(bool reset);
		void setLowODREnabled(bool enabled);
		bool getLowODREnabled();





    private:
        uint8_t devAddr;
        uint8_t buffer[6];
        bool 	endianMode;
};

#endif /* _L3GD20H_H_ */
