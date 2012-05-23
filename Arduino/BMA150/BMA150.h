// I2Cdev library collection - BMA150 I2C device class header file
// Based on BMA150 datasheet, 29/05/2008
// 01/18/2012 by Brian McCain <bpmccain@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-01-18 - initial release

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

#ifndef _BMA150_H_
#define _BMA150_H_

#include "I2Cdev.h"

#define BMA150_ADDRESS_00           0xA1                // Default Address
#define BMA150_ADDRESS_01           0x38                // Used on the Atmel ATAVRSBIN1
#define BMA150_DEFAULT_ADDRESS      BMA150_ADDRESS_01

#define BMA150_RA_CHIP_ID           0x00
#define BMA150_RA_VERSION           0x01
#define BMA150_RA_X_AXIS_LSB        0x02
#define BMA150_RA_X_AXIS_MSB        0x03
#define BMA150_RA_Y_AXIS_LSB        0x04
#define BMA150_RA_Y_AXIS_MSB        0x05
#define BMA150_RA_Z_AXIS_LSB        0x06
#define BMA150_RA_Z_AXIS_MSB        0x07
#define BMA150_RA_TEMP_RD           0x08
#define BMA150_RA_SMB150_STATUS     0x09
#define BMA150_RA_SMB150_CTRL       0x0a
#define BMA150_RA_SMB150_CONF1      0x0b
#define BMA150_RA_LG_THRESHOLD      0x0c
#define BMA150_RA_LG_DURATION       0x0d
#define BMA150_RA_HG_THRESHOLD      0x0e
#define BMA150_RA_HG_DURATION       0x0f
#define BMA150_RA_MOTION_THRS       0x10
#define BMA150_RA_HYSTERESIS        0x11
#define BMA150_RA_CUSTOMER1         0x12
#define BMA150_RA_CUSTOMER2         0x13
#define BMA150_RA_RANGE_BWIDTH      0x14
#define BMA150_RA_SMB150_CONF2      0x15
#define BMA150_RA_OFFS_GAIN_X       0x16
#define BMA150_RA_OFFS_GAIN_Y       0x17
#define BMA150_RA_OFFS_GAIN_Z       0x18
#define BMA150_RA_OFFS_GAIN_T       0x19
#define BMA150_RA_OFFSET_X          0x1a
#define BMA150_RA_OFFSET_Y          0x1b
#define BMA150_RA_OFFSET_Z          0x1c
#define BMA150_RA_OFFSET_T          0x1d

#define BMA150_CHIP_ID_BIT             2
#define BMA150_CHIP_ID_LENGTH          3

#define BMA150_X_AXIS_LSB_BIT          7
#define BMA150_X_AXIS_LSB_LENGTH       2
#define BMA150_X_NEW_DATA_BIT          0

#define BMA150_Y_AXIS_LSB_BIT          7
#define BMA150_Y_AXIS_LSB_LENGTH       2
#define BMA150_Y_NEW_DATA_BIT          0

#define BMA150_Z_AXIS_LSB_BIT          7
#define BMA150_Z_AXIS_LSB_LENGTH       2
#define BMA150_Z_NEW_DATA_BIT          0

#define BMA150_STATUS_HG_BIT           0
#define BMA150_STATUS_LG_BIT           1
#define BMA150_HG_LATCHED_BIT          2
#define BMA150_LG_LATCHED_BIT          3
#define BMA150_ALERT_PHASE_BIT         4
#define BMA150_ST_RESULT_BIT           7

#define BMA150_SLEEP_BIT               0
#define BMA150_SOFT_RESET_BIT          1
#define BMA150_ST0_BIT                 2
#define BMA150_ST1_BIT                 3
#define BMA150_EEW_BIT                 4
#define BMA150_UPDATE_IMAGE_BIT        5
#define BMA150_RESET_INT_BIT           6

#define BMA150_ENABLE_LG_BIT           0
#define BMA150_ENABLE_HG_BIT           1
#define BMA150_COUNTER_LG_BIT          3
#define BMA150_COUNTER_LG_LENGTH       2
#define BMA150_COUNTER_HG_BIT          5
#define BMA150_COUNTER_HG_LENGTH       2
#define BMA150_ANY_MOTION_BIT          6
#define BMA150_ALERT_BIT               7

#define BMA150_LG_HYST_BIT             2
#define BMA150_LG_HYST_LENGTH          3
#define BMA150_HG_HYST_BIT             5
#define BMA150_HG_HYST_LENGTH          3
#define BMA150_ANY_MOTION_DUR_BIT      7
#define BMA150_ANY_MOTION_DUR_LENGTH   2

#define BMA150_BANDWIDTH_BIT           2
#define BMA150_BANDWIDTH_LENGTH        3
#define BMA150_RANGE_BIT               4
#define BMA150_RANGE_LENGTH            2

#define BMA150_WAKE_UP_BIT             0
#define BMA150_WAKE_UP_PAUSE_BIT       2
#define BMA150_WAKE_UP_PAUSE_LENGTH    2
#define BMA150_SHADOW_DIS_BIT          3
#define BMA150_LATCH_INT_BIT           4
#define BMA150_NEW_DATA_INT_BIT        5
#define BMA150_ENABLE_ADV_INT_BIT      6
#define BMA150_SPI4_BIT                7

/* range and bandwidth */
#define BMA150_RANGE_2G                0
#define BMA150_RANGE_4G                1
#define BMA150_RANGE_8G                2

#define BMA150_BW_25HZ                 0
#define BMA150_BW_50HZ                 1
#define BMA150_BW_100HZ                2
#define BMA150_BW_190HZ                3
#define BMA150_BW_375HZ                4
#define BMA150_BW_750HZ                5
#define BMA150_BW_1500HZ               6

/* mode settings */
#define BMA150_MODE_NORMAL             0
#define BMA150_MODE_SLEEP              1

class BMA150 {
    public:
        BMA150();
        BMA150(uint8_t address);
        
        void initialize();
        bool testConnection();

        // CHIP_ID register
        uint8_t getDeviceID();
        
        // VERSION register
        uint8_t getChipRevision();
        
        // AXIS registers
        void getAcceleration(int16_t* x, int16_t* y, int16_t* z);
        int16_t getAccelerationX();
        int16_t getAccelerationY();
        int16_t getAccelerationZ();
        bool newDataX();
        bool newDataY();
        bool newDataZ();
                
        // TEMP register
        int8_t getTemperature();
        
        // SMB150 registers
        bool getStatusHG();
        bool getStatusLG();
        bool getHGLatched();
        bool getLGLatched();
        bool getAlertPhase();
        bool getSTResult();
        void setStatusHG(bool status_hg);
        void setStatusLG(bool status_lg);
        void setHGLatched(bool hg_latched);
        void setLGLatched(bool lg_latched);
        void setAlertPhase(bool alert_phase);
        void setSTResult(bool st_result);
        
        bool getSleep();
        bool getSoftReset();
        bool getSelfTest0();
        bool getSelfTest1();
        bool getEEW();
        bool getUpdateImage();
        bool getResetINT();
        void setSleep(bool sleep);
        void setSoftReset(bool soft_reset);
        void setSelfTest0(bool st0);
        void setSelfTest1(bool st1);
        void setEEW(bool eew);
        void setUpdateImage(bool update_image);
        void setResetINT(bool reset_int);
        
        bool getEnableLG();
        bool getEnableHG();
        int8_t getCounterLG();
        int8_t getCounterHG();
        bool getAnyMotion();
        bool getAlert();
        void setEnableLG(bool enable_lg);
        void setEnableHG(bool enable_hg);
        void setCounterLG(int8_t counter_lg);
        void setCounterHG(int8_t counter_hg);
        void setAnyMotion(bool any_motion);
        void setAlert(bool alert);
        
        bool getWakeUp();
        int8_t getWakeUpPause();
        bool getShadowDis();
        bool getLatchInt();
        bool getNewDataInt();
        bool getEnableAdvInt();
        bool getSPI4();
        void setWakeUp(bool wake_up);
        void setWakeUpPause(int8_t wake_up_pause);
        void setShadowDis(bool shadow_dis);
        void setLatchInt(bool latch_int);
        void setNewDataInt(bool new_data_int);
        void setEnableAdvInt(bool enable_adv_int);
        void setSPI4(bool spi4);    
        
        // LG / HG registers
        uint8_t getLGThreshold();
        void  setLGThreshold(uint8_t lgthres);

        uint8_t getLGDuration();
        void  setLGDuration(uint8_t lgdur);

        uint8_t getHGThreshold();
        void  setHGThreshold(uint8_t hgthres);

        uint8_t getHGDuration();
        void  setHGDuration(uint8_t hgdur);

        // MOTION_THRS register
        uint8_t getMotionThreshold();
        void  setMotionThreshold(uint8_t mot_thres);
        
        // HYSTERESIS register
        uint8_t getLGHysteresis();
        void  setLGHysteresis(uint8_t lg_hys);
        uint8_t getHGHysteresis();
        void  setHGHysteresis(uint8_t hg_hys);
        uint8_t getMotionDuration();
        void  setMotionDuration(uint8_t mot_dur);
        
        // CUSTOMER registers
        uint8_t getCustom1();
        void setCustom1(uint8_t custom1);
        uint8_t getCustom2();
        void setCustom2(uint8_t custom2);
        
        // RANGE / BANDWIDTH registers
        uint8_t getRange();
        void setRange(uint8_t range);
        uint8_t getBandwidth();
        void setBandwidth(uint8_t bandwidth);
        
        // OFFS_GAIN registers
        
        // OFFSET registers
        
        private:
        uint8_t devAddr;
        uint8_t buffer[6];
        uint8_t mode;
};

#endif /* _BMA150_H_ */