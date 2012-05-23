// I2Cdev library collection - AD7746 I2C device class header file
// Based on Analog Devices AD7746 Datasheet, Revision 0, 2005
// 2012-04-01 by Peteris Skorovs <pskorovs@gmail.com>
//
// This I2C device library is using (and submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-04-01 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Peteris Skorovs

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

#ifndef _AD7746_H_
#define _AD7746_H_

#include "I2Cdev.h"


#define AD7746_ADDRESS           0x48
#define AD7746_DEFAULT_ADDRESS   AD7746_ADDRESS           


#define AD7746_RA_STATUS                 0x00 // Status
#define AD7746_RA_CAP_DATA_H             0x01 // Cap data
#define AD7746_RA_CAP_DATA_M             0x02 // Cap data
#define AD7746_RA_CAP_DATA_L             0x03 // Cap data
#define AD7746_RA_VT_DATA_H              0x04 // VT data
#define AD7746_RA_VT_DATA_M              0x05 // VT data
#define AD7746_RA_VT_DATA_L              0x06 // VT data
#define AD7746_RA_CAP_SETUP              0x07 // Cap Setup
#define AD7746_RA_VT_SETUP               0x08 // VT Setup
#define AD7746_RA_EXC_SETUP              0x09 // Exc Setup
#define AD7746_RA_CONFIGURATION          0x0A // Configuration
#define AD7746_RA_CAP_DAC_A              0x0B // Cap DAC A
#define AD7746_RA_CAP_DAC_B              0x0C // Cap DAC B
#define AD7746_RA_CAP_OFF_H              0x0D 
#define AD7746_RA_CAP_OFF_L              0x0E 
#define AD7746_RA_CAP_GAIN_H             0x0F
#define AD7746_RA_CAP_GAIN_L             0x10
#define AD7746_RA_VOLT_GAIN_H            0x11
#define AD7746_RA_VOLT_GAIN_L            0x12

#define AD7746_RESET                     0xBF

// Status
#define AD7746_EXCERR_BIT                3
#define AD7746_RDY_BIT		             2
#define AD7746_RDYVT_BIT                 1
#define AD7746_RDYCAP_BIT                0

// Cap Setup
#define AD7746_CAPEN_BIT                 7
#define AD7746_CIN2_BIT                  6
#define AD7746_CAPDIFF_BIT               5
#define AD7746_CACHOP_BIT                0

#define AD7746_CAPEN                     (1 << AD7746_CAPEN_BIT)
#define AD7746_CIN2                      (1 << AD7746_CIN2_BIT)

// VT Setup
#define AD7746_VTEN_BIT                  7
#define AD7746_VTMD_BIT_1                6
#define AD7746_VTMD_BIT_0                5
#define AD7746_EXTREF_BIT                4
#define AD7746_VTSHORT_BIT               1
#define AD7746_VTCHOP_BIT	             0

#define AD7746_VTEN                      (1 << AD7746_VTEN_BIT)

#define AD7746_VTMD_INT_TEMP             0
#define AD7746_VTMD_EXT_TEMP             (1 << AD7746_VTMD_BIT_0)
#define AD7746_VTMD_VDD_MON              (1 << AD7746_VTMD_BIT_1)
#define AD7746_VTMD_VIN                  (1 << AD7746_VTMD_BIT_1) | (1 << AD7746_VTMD_BIT_0)

// Exc Setup
#define AD7746_CLKCTRL_BIT		         7 
#define AD7746_EXCON_BIT		         6                                            
#define AD7746_EXCB_BIT		             5
#define AD7746_INV_EXCB_BIT		         4
#define AD7746_EXCA_BIT		             3
#define AD7746_INV_EXCA_BIT		         2
#define AD7746_EXCLVL_BIT_1              1
#define AD7746_EXCLVL_BIT_0              0

#define AD7746_EXCA                      (1 << AD7746_EXCA_BIT)
#define AD7746_EXCB                      (1 << AD7746_EXCB_BIT)
#define AD7746_EXCON                     (1 << AD7746_EXCON_BIT)

#define AD7746_EXCLVL_VDD_X_1_8          0
#define AD7746_EXCLVL_VDD_X_1_4          (1 << AD7746_EXCLVL_BIT_0)
#define AD7746_EXCLVL_VDD_X_3_8          (1 << AD7746_EXCLVL_BIT_1)
#define AD7746_EXCLVL_VDD_X_1_2          (1 << AD7746_EXCLVL_BIT_1) | (1 << AD7746_EXCLVL_BIT_0)

// Configuration
#define AD7746_VTF_BIT_1                 7
#define AD7746_VTF_BIT_0                 6
#define AD7746_CAPF_BIT_2                5
#define AD7746_CAPF_BIT_1                4
#define AD7746_CAPF_BIT_0                3
#define AD7746_MD_BIT_2                  2
#define AD7746_MD_BIT_1                  1
#define AD7746_MD_BIT_0                  0

#define AD7746_VTF_20P1                  0
#define AD7746_VTF_32P1                  (1 << AD7746_VTF_BIT_0)
#define AD7746_VTF_62P1                  (1 << AD7746_VTF_BIT_1)
#define AD7746_VTF_122P1                 (1 << AD7746_VTF_BIT_1) | (1 << AD7746_VTF_BIT_0)

#define AD7746_CAPF_11P0                 0
#define AD7746_CAPF_11P9                 (1 << AD7746_CAPF_BIT_0)
#define AD7746_CAPF_20P0                 (1 << AD7746_CAPF_BIT_1)
#define AD7746_CAPF_38P0                 (1 << AD7746_CAPF_BIT_1) | (1 << AD7746_CAPF_BIT_0)
#define AD7746_CAPF_62P0                 (1 << AD7746_CAPF_BIT_2)
#define AD7746_CAPF_77P0                 (1 << AD7746_CAPF_BIT_2) | (1 << AD7746_CAPF_BIT_0)
#define AD7746_CAPF_92P0                 (1 << AD7746_CAPF_BIT_2) | (1 << AD7746_CAPF_BIT_1)
#define AD7746_CAPF_109P6                (1 << AD7746_CAPF_BIT_2) | (1 << AD7746_CAPF_BIT_1) | (1 << AD7746_CAPF_BIT_0)

#define AD7746_MD_IDLE                   0
#define AD7746_MD_CONTINUOUS_CONVERSION  (1 << AD7746_MD_BIT_0)
#define AD7746_MD_SINGLE_CONVERSION      (1 << AD7746_MD_BIT_1)
#define AD7746_MD_POWER_DOWN             (1 << AD7746_MD_BIT_1) | (1 << AD7746_MDF_BIT_0)
#define AD7746_MD_OFFSET_CALIBRATION     (1 << AD7746_MD_BIT_2) | (1 << AD7746_MD_BIT_0)
#define AD7746_MD_GAIN_CALIBRATION       (1 << AD7746_MD_BIT_2) | (1 << AD7746_MD_BIT_1)

// Cap DAC A
#define AD7746_DACAEN_BIT                7

#define AD7746_DACAEN                    (1 << AD7746_DACAEN_BIT)

// Cap DAC B
#define AD7746_DACBEN_BIT                7

#define AD7746_DACBEN                    (1 << AD7746_DACBEN_BIT)


#define AD7746_DAC_COEFFICIENT           0.13385826771654F // 17pF/127



class AD7746 {
    public:
        AD7746();
        AD7746(uint8_t address);

        void initialize();
        bool testConnection();
        void reset(); 

        uint32_t getCapacitance();
    
        void writeCapSetupRegister(uint8_t data);
        void writeVtSetupRegister(uint8_t data);
        void writeExcSetupRegister(uint8_t data);
        void writeConfigurationRegister(uint8_t data);
        void writeCapDacARegister(uint8_t data);
        void writeCapDacBRegister(uint8_t data);
        

    private:
        uint8_t devAddr;
        uint8_t buffer[19];
};

#endif /* _AD7746_H_ */
