// I2Cdev library collection - LSM303DLHC I2C device class header file
// Based on ST LSM303DLHC datasheet REV 2, 11/2013
// 3/10/2015 by Nate Costello <natecostello at gmail dot com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2015-03-15 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 [Author Name], Jeff Rowberg

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

#ifndef _LSM303DLHC_H_
#define _LSM303DLHC_H_

#include "I2Cdev.h"

// ----------------------------------------------------------------------------
// STUB TODO:
// List all possible device I2C addresses here, if they are predefined. Some
// devices only have one possible address, in which case that one should be
// defined as both [LSM303DLHC]_ADDRESS and [LSM303DLHC]_DEFAULT_ADDRESS for
// consistency. The *_DEFAULT address will be used if a device object is
// created without an address provided in the constructor. Note that I2C
// devices conventionally use 7-bit addresses, so these will generally be
// between 0x00 and 0x7F.
// The LSM303DLHC uses two device addresses, one for the accerometer, and on
// for the megnetometer.  
// ----------------------------------------------------------------------------
#define LSM303DLHC_ADDRESS_A            0x19
#define LSM303DLHC_ADDRESS_M            0x1E
#define LSM303DLHC_DEFAULT_ADDRESS_A    0x19
#define LSM303DLHC_DEFAULT_ADDRESS_M    0x1E

// ----------------------------------------------------------------------------
// STUB TODO:
// List all registers that this device has. The goal for all device libraries
// is to have complete coverage of all possible functions, so be sure to add
// everything in the datasheet. Register address constants should use the extra
// prefix "RA_", followed by the datasheet's given name for each register.
// ----------------------------------------------------------------------------
#define LSM303DLHC_RA_CTRL_REG1_A       0x20    //  rw  0000 0111
#define LSM303DLHC_RA_CTRL_REG2_A       0x21    //  rw  0000 0000
#define LSM303DLHC_RA_CTRL_REG3_A       0x22    //  rw  0000 0000
#define LSM303DLHC_RA_CTRL_REG4_A       0x23    //  rw  0000 0000
#define LSM303DLHC_RA_CTRL_REG5_A       0x24    //  rw  0000 0000
#define LSM303DLHC_RA_CTRL_REG6_A       0x25    //  rw  0000 0000
#define LSM303DLHC_RA_REFERENCE_A       0x06    //  rw  0000 0000
#define LSM303DLHC_RA_STATUS_REG_A      0x27    //  r   0000 0000
#define LSM303DLHC_RA_OUT_X_L_A         0x28    //  r
#define LSM303DLHC_RA_OUT_X_H_A         0x29    //  r
#define LSM303DLHC_RA_OUT_Y_L_A         0x2A    //  r
#define LSM303DLHC_RA_OUT_Y_H_A         0x2B    //  r
#define LSM303DLHC_RA_OUT_Z_L_A         0x2C    //  r
#define LSM303DLHC_RA_OUT_Z_H_A         0x2D    //  r
#define LSM303DLHC_RA_FIFO_CTRL_REG_A   0x2E    //  rw  0000 0000
#define LSM303DLHC_RA_FIFO_SRC_REG_A    0x2F    //  r
#define LSM303DLHC_RA_INT1_CFG_A        0x30    //  rw  0000 0000
#define LSM303DLHC_RA_INT1_SRC_A        0x31    //  r   0000 0000
#define LSM303DLHC_RA_INT1_THS_A        0x32    //  rw  0000 0000
#define LSM303DLHC_RA_INT1_DURATION_A   0x33    //  rw  0000 0000
#define LSM303DLHC_RA_INT2_CFG_A        0x34    //  rw  0000 0000
#define LSM303DLHC_RA_INT2_SRC_A        0x35    //  r   0000 0000
#define LSM303DLHC_RA_INT2_THS_A        0x36    //  rw  0000 0000
#define LSM303DLHC_RA_INT2_DURATION_A   0x37    //  rw  0000 0000
#define LSM303DLHC_RA_CLICK_CFG_A       0x38    //  rw  0000 0000
#define LSM303DLHC_RA_CLICK_SRC_A       0x39    //  rw  0000 0000
#define LSM303DLHC_RA_CLICK_THS_A       0x3A    //  rw  0000 0000
#define LSM303DLHC_RA_TIME_LIMIT_A      0x3B    //  rw  0000 0000
#define LSM303DLHC_RA_TIME_LATENCY_A    0x3C    //  rw  0000 0000
#define LSM303DLHC_RA_TIME_WINDOW_A     0x3D    //  rw  0000 0000

#define LSM303DLHC_RA_CRA_REG_M         0x00    //  rw  0001 0000
#define LSM303DLHC_RA_CRB_REG_M         0x01    //  rw  0010 0000
#define LSM303DLHC_RA_MR_REG_M          0x02    //  rw  0000 0011
#define LSM303DLHC_RA_OUT_X_H_M         0x03    //  r
#define LSM303DLHC_RA_OUT_X_L_M         0x04    //  r
#define LSM303DLHC_RA_OUT_Z_H_M         0x05    //  r
#define LSM303DLHC_RA_OUT_Z_L_M         0x06    //  r
#define LSM303DLHC_RA_OUT_Y_H_M         0x07    //  r
#define LSM303DLHC_RA_OUT_Y_L_M         0x08    //  r
#define LSM303DLHC_RA_SR_REG_M          0x09    //  r   0000 0000
#define LSM303DLHC_RA_IRA_REG_M         0x0A    //  r   0100 1000
#define LSM303DLHC_RA_IRB_REG_M         0x0B    //  r   0011 0100
#define LSM303DLHC_RA_IRC_REG_M         0x0C    //  r   0011 0011
#define LSM303DLHC_RA_TEMP_OUT_H_M      0x31    //  r
#define LSM303DLHC_RA_TEMP_OUT_L_M      0x32    //  r

// ----------------------------------------------------------------------------
// STUB TODO:
// List register structures wherever necessary. Bit position constants should
// end in "_BIT", and are defined from 7 to 0, with 7 being the left/MSB and
// 0 being the right/LSB. If the device uses 16-bit registers instead of the
// more common 8-bit registers, the range is 15 to 0. Field length constants
// should end in "_LENGTH", but otherwise match the name of bit position
// constant that it complements. Fields that are a single bit in length don't
// need a separate field length constant.
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// STUB TODO:
// List any special predefined values for each register according to the
// datasheet. For example, MEMS devices often provide different options for
// measurement rates, say 25Hz, 50Hz, 100Hz, and 200Hz. These are typically
// represented by arbitrary bit values, say 0b00, 0b01, 0b10, and 0b11 (or 0x0,
// 0x1, 0x2, and 0x3). Defining them here makes it easy to know which options
// are available.
// ----------------------------------------------------------------------------

//CTRL_REG1_A
#define LSM303DLHC_ODR_BIT              7
#define LSM303DLHC_ODR_LENGTH           4
#define LSM303DLHC_LPEN_BIT             3
#define LSM303DLHC_ZEN_BIT              2
#define LSM303DLHC_YEN_BIT              1
#define LSM303DLHC_XEN_BIT              0

#define LSM303DLHC_ODR_RATE_POWERDOWN       0b0000
#define LSM303DLHC_ODR_RATE_1               0b0001
#define LSM303DLHC_ODR_RATE_10              0b0010
#define LSM303DLHC_ODR_RATE_25              0b0011
#define LSM303DLHC_ODR_RATE_50              0b0100
#define LSM303DLHC_ODR_RATE_100             0b0101
#define LSM303DLHC_ODR_RATE_200             0b0110
#define LSM303DLHC_ODR_RATE_400             0b0111
#define LSM303DLHC_ODR_RATE_1620_LP         0b1000
#define LSM303DLHC_ODR_RATE_1344_N_5376_LP  0b1001

//CTRL_REG2_A
#define LSM303DLHC_HPM_BIT              7
#define LSM303DLHC_HPM_LENGTH           2
#define LSM303DLHC_HPCF_BIT             5
#define LSM303DLHC_HPCF_LENGTH          2
#define LSM303DLHC_FDS_BIT              3
#define LSM303DLHC_HPCLICK_BIT          2
#define LSM303DLHC_HPIS2_BIT            1
#define LSM303DLHC_HPIS1_BIT            0

#define LSM303DLHC_HPM_HRF              0b00
#define LSM303DLHC_HPM_REFERANCE        0b01
#define LSM303DLHC_HPM_NORMAL           0b10
#define LSM303DLHC_HPM_AUTORESET        0b11

#define LSM303DLHC_HPCF_1               0b00
#define LSM303DLHC_HPCF_2               0b01
#define LSM303DLHC_HPCF_3               0b10
#define LSM303DLHC_HPCF_4               0b11

//CTRL_REG3_A
#define LSM303DLHC_I1_CLICK_BIT         7
#define LSM303DLHC_I1_AOI1_BIT          6
#define LSM303DLHC_I1_AOI2_BIT          5
#define LSM303DLHC_I1_DRDY1_BIT         4
#define LSM303DLHC_I1_DRDY2_BIT         3
#define LSM303DLHC_I1_WTM_BIT           2
#define LSM303DLHC_I1_OVERRUN_BIT       1

//CTRL_REG4_A
#define LSM303DLHC_BDU_BIT              7
#define LSM303DLHC_BLE_BIT              6
#define LSM303DLHC_FS_BIT               5
#define LSM303DLHC_FS_LENGTH            2
#define LSM303DLHC_HR_BIT               3
#define LSM303DLHC_SIM_BIT              0

#define LSM303DLHC_LITTLE_ENDIAN        0
#define LSM303DLHC_BIG_ENDIAN           1
#define LSM303DLHC_FS_2                 0b00
#define LSM303DLHC_FS_4                 0b01
#define LSM303DLHC_FS_8                 0b10
#define LSM303DLHC_FS_16                0b11
#define LSM303DLHC_SIM_3W               1
#define LSM303DLHC_SIM_4W               0

//CTRL_REG5_A
#define LSM303DLHC_BOOT_BIT             7
#define LSM303DLHC_FIFO_EN_BIT          6
#define LSM303DLHC_LIR_INT1_BIT         3
#define LSM303DLHC_D4D_INT1_BIT         2
#define LSM303DLHC_LIR_INT2_BIT         1
#define LSM303DLHC_D4D_INT2_BIT         0

//CTRL_REG6_A
#define LSM303DLHC_I2_CLICK_BIT         7
#define LSM303DLHC_I2_INT1_BIT          6
#define LSM303DLHC_I2_INT2_BIT          5
#define LSM303DLHC_BOOT_I1_BIT          4
#define LSM303DLHC_P2_ACT_BIT           3
#define LSM303DLHC_H_ACTIVE_BIT         1

//REFERENCE_A

//STATUS_REG_A
#define LSM303DLHC_ZYXOR_BIT            7
#define LSM303DLHC_ZOR_BIT              6
#define LSM303DLHC_YOR_BIT              5
#define LSM303DLHC_XOR_BIT              4
#define LSM303DLHC_ZYXDA_BIT            3
#define LSM303DLHC_ZDA_BIT              2
#define LSM303DLHC_YDA_BIT              1
#define LSM303DLHC_XDA_BIT              0

//FIFO_CTRL_REG_A
#define LSM303DLHC_FM_BIT               7
#define LSM303DLHC_FM_LENGTH            2
#define LSM303DLHC_TR_BIT               5
#define LSM303DLHC_FTH_BIT              4
#define LSM303DLHC_FTH_LENGTH           5

#define LSM303DLHC_FM_BYBASS            0b00
#define LSM303DLHC_FM_FIFO              0b01
#define LSM303DLHC_FM_STREAM            0b10
#define LSM303DLHC_FM_TRIGGER           0b11
#define LSM303DLHC_TR_INT1              0
#define LSM303DLHC_TR_INT2              1

//FIFO_SRC_REG_A
#define LSM303DLHC_WTM_BIT              7
#define LSM303DLHC_OVRN_FIFO_BIT        6
#define LSM303DLHC_EMPTY_BIT            5
#define LSM303DLHC_FSS_BIT              4
#define LSM303DLHC_FSS_LENGTH           5

//INT1_CFG_A
#define LSM303DLHC_INT1_AOI_BIT              7
#define LSM303DLHC_INT1_6D_BIT               6
#define LSM303DLHC_INT1_ZHIE_ZUPE_BIT        5
#define LSM303DLHC_INT1_ZLIE_ZDOWNE_BIT      4
#define LSM303DLHC_INT1_YHIE_YUPE_BIT        3
#define LSM303DLHC_INT1_YLIE_YDOWNE_BIT      2
#define LSM303DLHC_INT1_XHIE_XUPE_BIT        1
#define LSM303DLHC_INT1_XLIE_XDOWNE_BIT      0

//INT1_SRC_A
#define LSM303DLHC_INT1_IA_BIT                   6
#define LSM303DLHC_INT1_ZH_BIT                   5
#define LSM303DLHC_INT1_ZL_BIT                   4
#define LSM303DLHC_INT1_YH_BIT                   3
#define LSM303DLHC_INT1_YL_BIT                   2
#define LSM303DLHC_INT1_XH_BIT                   1
#define LSM303DLHC_INT1_XL_BIT                   0

//INT1_THS_A
#define LSM303DLHC_INT1_THS_BIT                 6
#define LSM303DLHC_INT1_THS_LENGTH              7

//INT1_DURATION_A
#define LSM303DLHC_INT1_DURATION_BIT            6
#define LSM303DLHC_INT1_DURATION_LENGTH         7

//INT2_CFG_A
#define LSM303DLHC_INT2_AOI_BIT                  7
#define LSM303DLHC_INT2_6D_BIT                   6
#define LSM303DLHC_INT2_ZHIE_BIT                 5
#define LSM303DLHC_INT2_ZLIE_BIT                 4
#define LSM303DLHC_INT2_YHIE_BIT                 3
#define LSM303DLHC_INT2_YLIE_BIT                 2
#define LSM303DLHC_INT2_XHIE_BIT                 1
#define LSM303DLHC_INT2_XLIE_BIT                 0

//INT2_SRC_A
#define LSM303DLHC_INT2_IA_BIT                   6
#define LSM303DLHC_INT2_ZH_BIT                   5
#define LSM303DLHC_INT2_ZL_BIT                   4
#define LSM303DLHC_INT2_YH_BIT                   3
#define LSM303DLHC_INT2_YL_BIT                   2
#define LSM303DLHC_INT2_XH_BIT                   1
#define LSM303DLHC_INT2_XL_BIT                   0

//INT2_THS_A
#define LSM303DLHC_INT2_THS_BIT                 6
#define LSM303DLHC_INT2_THS_LENGTH              7

//INT2_DURATION_A
#define LSM303DLHC_INT2_DURATION_BIT            6
#define LSM303DLHC_INT2_DURATION_LENGTH         7

//CLICK_CFG_A
#define LSM303DLHC_CLICK_ZD_BIT               5
#define LSM303DLHC_CLICK_ZS_BIT               4
#define LSM303DLHC_CLICK_YD_BIT               3
#define LSM303DLHC_CLICK_YS_BIT               2
#define LSM303DLHC_CLICK_XD_BIT               1
#define LSM303DLHC_CLICK_XS_BIT               0

//CLICK_SRC_A
#define LSM303DLHC_CLICK_IA_BIT               6
#define LSM303DLHC_CLICK_DCLICK_BIT           5
#define LSM303DLHC_CLICK_SCLICK_BIT           4
#define LSM303DLHC_CLICK_SIGN_BIT             3
#define LSM303DLHC_CLICK_Z_BIT                2
#define LSM303DLHC_CLICK_Y_BIT                1
#define LSM303DLHC_CLICK_X_BIT                0

//CLICK_THS_A
#define LSM303DLHC_CLICK_THS_BIT                 6
#define LSM303DLHC_CLICK_THS_LENGTH              7

//TIME_LIMIT_A
#define LSM303DLHC_TLI_BIT                      6
#define LSM303DLHC_TLI_LENGTH                   7

//TIME_LATENCY_A

//TIME_WINDOW_A

//CRA_REG_M
#define LSM303DLHC_TEMP_EN_BIT              7
#define LSM303DLHC_DO_BIT                   4
#define LSM303DLHC_DO_LENGTH                3

#define LSM303DLHC_DO_RATE_0                0b000
#define LSM303DLHC_DO_RATE_1                0b001
#define LSM303DLHC_DO_RATE_3                0b010
#define LSM303DLHC_DO_RATE_7                0b011
#define LSM303DLHC_DO_RATE_15               0b100
#define LSM303DLHC_DO_RATE_30               0b101
#define LSM303DLHC_DO_RATE_75               0b110
#define LSM303DLHC_DO_RATE_220              0b111

//CRB_REG_M
#define LSM303DLHC_GN_BIT                   7
#define LSM303DLHC_GN_LENGTH                3

#define LSM303DLHC_GN_1100                  0b001
#define LSM303DLHC_GN_855                   0b010
#define LSM303DLHC_GN_670                   0b011             
#define LSM303DLHC_GN_450                   0b100             
#define LSM303DLHC_GN_400                   0b101                
#define LSM303DLHC_GN_330                   0b110                
#define LSM303DLHC_GN_230                   0b111

//MR_REG_M
#define LSM303DLHC_MD_BIT                   1
#define LSM303DLHC_MD_LENGTH                2

#define LSM303DLHC_MD_CONTINUOUS            0b00
#define LSM303DLHC_MD_SINGLE                0b01
#define LSM303DLHC_MD_SLEEP                 0b10

//OUT_X_H_M
//OUT_X_L_M
//OUT_Y_H_M
//OUT_Y_L_M
//OUT_Z_H_M
//OUT_Z_L_M

//SR_REG_M
#define LSM303DLHC_M_DRDY_BIT                 0
#define LSM303DLHC_M_LOCK_BIT                 1

//IRA_REG_M
//IRB_REG_M
//IRC_REG_M

//TEMP_OUT_H_M
//TEMP_OUT_L_M



class LSM303DLHC {
    public:
        LSM303DLHC();
        LSM303DLHC(uint8_t addressA, uint8_t addressM);

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
        //CTRL_REG1_A, r/w
        void setAccelOutputDataRate(uint16_t rate);
        uint16_t getAccelOutputDataRate();
        void setAccelLowPowerEnabled(bool enabled);
        bool getAccelLowPowerEnabled();
        void setAccelZEnabled(bool enabled);
        bool getAccelZEnabled();
        void setAccelYEnabled(bool enabled);
        bool getAccelYEnabled();
        void setAccelXEnabled(bool enabled);
        bool getAccelXEnabled();

        //CTRL_REG2_A r/w
        void setAccelHighPassMode(uint8_t mode);
        uint8_t getAccelHighPassMode();
        void setAccelHighPassFilterCutOffFrequencyLevel(uint8_t level);
        uint8_t getAccelHighPassFilterCutOffFrequencyLevel();


        //CTRL_REG3_A r/w
        void setAccelINT1ClickEnabled(bool enabled);    //routes Click interrupt to INT1 pin
        bool getAccelINT1ClickEnabled();
        void setAccelINT1AOI1Enabled(bool enabled);     //routes AOI1 interrupt to INT1 pin
        bool getAccelINT1AOI1Enabled();
        void setAccelINT1AOI2Enabled(bool enabled);     //routes AOI2 interrupt to INT1 pin
        bool getAccelINT1AOI2Enabled();
        void setAccelINT1DataReady1Enabled(bool enabled);   //routes DataReady1 int to INT1..
        bool getAccelINT1DataReady1Enabled();
        void setAccelINT1DataReady2Enabled(bool enabled);   //routes DataReady2 int to INT1..
        bool getAccelINT1DataReady2Enabled();
        void setAccelINT1FIFOWatermarkEnabled(bool enabled);//routes FIFO WTM int to INT1 pin
        bool getAccelINT1FIFOWatermarkEnabled();
        void setAccelINT1FIFOOverunEnabled(bool enabled);//routes FIFO Overrun int to INT1..
        bool getAccelINT1FIFOOverunEnabled();

        //CTRL_REG4_A r/w
        void setAccelBlockDataUpdateEnabled(bool enabled);
        bool getAccelBlockDataUpdateEnabled();
        void setAccelEndianMode(bool endianness);
        bool getAccelEndianMode();
        void setAccelFullScale(uint8_t scale);
        uint8_t getAccelFullScale();
        void setAccelHighResOutputEnabled(bool enabled);
        bool getAccelHighResOutputEnabled();
        void setAccelSPIMode(bool mode);
        bool getAccelSPIMode();

        //CTRL_REG5_A r/w
        void rebootAccelMemoryContent();
        void setAccelFIFOEnabled(bool enabled);
        bool getAccelFIFOEnabled();
        void setAccelInterrupt1RequestLatched(bool latched);
        bool getAccelInterrupt1RequestLatched();
        void setAccelInterrupt2RequestLatched(bool latched);
        bool getAccelInterrupt2RequestLatched();
        void setAccelDetect4DInterrupt1Enabled(bool enabled);
        bool getAccelDetect4DInterrupt1Enabled();
        void setAccelDetect4DInterrupt2Enabled(bool enabled);
        bool getAccelDetect4DInterrupt2Enabled();
        
        //CTRL_REG6_A r/w
        void setAccelINT2ClickEnabled(bool enabled);
        bool getAccelINT2ClickEnabled();
        void setAccelINT2Interrupt1Enabled(bool enabled);
        bool getAccelINT2Interrupt1Enabled();
        void setAccelINT2Interrupt2Enabled(bool enabled);
        bool getAccelINT2Interrupt2Enabled();

        void setAccelRebootMemoryContentINT2Enabled(bool enabled);
        bool getAccelRebootMemoryContentINT2Enabled();
        // void setAccelActiveFunctionStatusINT2Enabled(bool enabled);
        // bool getAccelActiveFunctionStatusINT2Enabled();
        void setAccelInterruptActiveLowEnabled(bool enabled);
        bool getAccelInterruptActiveLowEnabled();

        //REFERENCE_A r/w
        void setAccelInterruptReference(uint8_t reference);
        uint8_t getAccelInterruptReference();

        //STATUS_REG_A r
        bool getAccelXYZOverrun();
        bool getAccelZOverrun();
        bool getAccelYOverrun();
        bool getAccelXOverrun();
        bool getAccelXYZDataAvailable();
        bool getAccelZDataAvailable();
        bool getAccelYDataAvailable();
        bool getAccelXDataAvailable();

        //OUT_*_A, r
        // OUT_* registers, read-only
        void getAcceleration(int16_t* x, int16_t* y, int16_t* z);
        int16_t getAccelerationX();
        int16_t getAccelerationY();
        int16_t getAccelerationZ();
        
        //FIFO_CTRL_REG_A, rw
        void setAccelFIFOMode(uint8_t mode);
        uint8_t getAccelFIFOMode();
        void setAccelFIFOTriggerINT(bool tirgger);
        bool getAccelFIFOTriggerINT();
        void setAccelFIFOThreshold(uint8_t fth);
        uint8_t getAccelFIFOThreshold();

        //FIFO_SRC_REG_A, r
        bool getAccelFIFOAtWatermark();
        bool getAccelFIFOOverrun();
        bool getAccelFIFOEmpty();
        uint8_t getAccelFIFOStoredSamples();
        
        //Int1_CFG_A, wr
        void setAccelInterrupt1Combination(bool combination);
        bool getAccelInterrupt1Combination();
        void setAccelInterrupt16DEnabled(bool enabled);
        bool getAccelInterrupt16DEnabled();

        void setAccelZHighUpInterrupt1Enabled(bool enabled);
        bool getAccelZHighUpInterrupt1Enabled();
        void setAccelYHighUpInterrupt1Enabled(bool enabled);
        bool getAccelYHighUpInterrupt1Enabled();
        void setAccelXHighUpInterrupt1Enabled(bool enabled);
        bool getAccelXHighUpInterrupt1Enabled();
        void setAccelZLowDownInterrupt1Enabled(bool enabled);
        bool getAccelZLowDownInterrupt1Enabled();
        void setAccelYLowDownInterrupt1Enabled(bool enabled);
        bool getAccelYLowDownInterrupt1Enabled();
        void setAccelXLowDownInterrupt1Enabled(bool enabled);
        bool getAccelXLowDownInterrupt1Enabled();

        //INT1_SRC_A, r
        uint8_t getAccelInterrupt1Source(); //this should be read entirely since it clears

        //INT1_THS_A, rw
        void setAccelInterrupt1Threshold(uint8_t value);
        uint8_t getAccelInterupt1Threshold();

        //INT1_DURATION_A, rw
        void setAccelInterrupt1Duration(uint8_t value);
        uint8_t getAccelInterrupt1Duration();

        //INT2_CFG_A, rw
        void setAccelInterrupt2Combination(bool combination);
        bool getAccelInterrupt2Combination();
        void setAccelInterrupt26DEnabled(bool enabled);
        bool getAccelInterrupt26DEnabled();

        void setAccelZHighInterrupt2Enabled(bool enabled);
        bool getAccelZHighInterrupt2Enabled();
        void setAccelYHighInterrupt2Enabled(bool enabled);
        bool getAccelYHighInterrupt2Enabled();
        void setAccelXHighInterrupt2Enabled(bool enabled);
        bool getAccelXHighInterrupt2Enabled();
        void setAccelZLowInterrupt2Enabled(bool enabled);
        bool getAccelZLowInterrupt2Enabled();
        void setAccelYLowInterrupt2Enabled(bool enabled);
        bool getAccelYLowInterrupt2Enabled();
        void setAccelXLowInterrupt2Enabled(bool enabled);
        bool getAccelXLowInterrupt2Enabled();

        //INT2_SRC_A, r
        uint8_t getAccelInterrupt2Source(); //this should be read entirely since it clears

        //INT2_THS_A, rw
        void setAccelInterrupt2Threshold(uint8_t value);
        uint8_t getAccelInterupt2Threshold();

        //INT2_DURATION_A, rw
        void setAccelInterrupt2Duration(uint8_t value);
        uint8_t getAccelInterrupt2Duration();

        //CLICK_CFG_A, rw
        void setAccelZDoubleClickEnabled(bool enabled);
        bool getAccelZDoubleClickEnabled();
        void setAccelZSingleClickEnabled(bool enabled);
        bool getAccelZSingleClickEnabled();
        void setAccelYDoubleClickEnabled(bool enabled);
        bool getAccelYDoubleClickEnabled();
        void setAccelYSingleClickEnabled(bool enabled);
        bool getAccelYSingleClickEnabled();
        void setAccelXDoubleClickEnabled(bool enabled);
        bool getAccelXDoubleClickEnabled();
        void setAccelXSingleClickEnabled(bool enabled);
        bool getAccelXSingleClickEnabled();

        //CLICK_SRC_A, rw?
        //I think the documentation is incorrect for DCLICK
        //and SCLICK.  I think these are flags, not enables.
        //For now we will assume this is readonly and simply
        //grab the whole byte consistant with the interrupt
        //source registers.
        uint8_t getAccelClickSource();

        //CLICK_THS_A, rw
        void setAcceLClickThreshold(uint8_t value);
        uint8_t getAccelClickThreshold();

        //TIME_LIMIT_A, rw
        void setAcceLClickTimeLimit(uint8_t value);
        uint8_t getAccelClickTimeLimit();

        //TIME_LATENCY_A, rw
        void setAcceLClickTimeLatency(uint8_t value);
        uint8_t getAccelClickTimeLatency();

        //TIME_WINDOW_A, rw
        void setAcceLClickTimeWindow(uint8_t value);
        uint8_t getAccelClickTimeWindow();

        //CRA_REG_M_A, rw
        void setMagTemperatureEnabled(bool enabled);
        bool getMagTemperatureEnabled();
        void setMagOutputDataRate(uint8_t rate);
        uint8_t getMagOutputDataRate();

        //CRB_REG_M, rw
        void setMagGain(uint16_t gain);
        uint16_t getMagGain();

        //MR_REG_M, rw
        void setMagMode(uint8_t);
        uint8_t getMagMode();

        //OUT_____M, r
        void getMag(int16_t* x, int16_t* y, int16_t* z);
        int16_t getMagX();
        int16_t getMagY();
        int16_t getMagZ();

        //SR_REG_M, r
        bool getMagOutputDataRegisterLock();
        bool getMagDataReady();

        //TEMP_OUT_*_M
        int16_t getTemperature();

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
        uint8_t devAddrA;
        uint8_t devAddrM;
        uint8_t buffer[6];
        bool endianMode;
};

#endif /* _LSM303DLHC_H_ */
