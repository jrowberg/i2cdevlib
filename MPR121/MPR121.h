// I2Cdev library collection - MPR121 I2C device class header file
// Based on Freescale MPR121 datasheet rev. 2, 04/2010 and Freescale App Note 3944, rev 1 3/26/2010
// 9/3/2011 by Andrew Schamp <schamp@gmail.com>
//
// This I2C device library is using (and submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-09-03 - add callback support
//     2011-08-20 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Andrew Schamp

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

#ifndef _MPR121_h_
#define _MPR121_h_

#include <inttypes.h>

// this is the 7-bit I2C address with the ADDR pin grounded
#define MPR121_DEFAULT_ADDRESS 0x5A

// MPR121 Registers (from data sheet)
#define ELE0_ELE7_TOUCH_STATUS          0x00
#define ELE8_ELE11_ELEPROX_TOUCH_STATUS 0x01

#define ELE0_7_OOR_STATUS          0x02
#define ELE8_11_ELEPROX_OOR_STATUS 0x03

#define ELE0_FILTERED_DATA_LSB    0x04
#define ELE0_FILTERED_DATA_MSB    0x05
#define ELE1_FILTERED_DATA_LSB    0x06
#define ELE1_FILTERED_DATA_MSB    0x07
#define ELE2_FILTERED_DATA_LSB    0x08
#define ELE2_FILTERED_DATA_MSB    0x09
#define ELE3_FILTERED_DATA_LSB    0x0A
#define ELE3_FILTERED_DATA_MSB    0x0B
#define ELE4_FILTERED_DATA_LSB    0x0C
#define ELE4_FILTERED_DATA_MSB    0x0D
#define ELE5_FILTERED_DATA_LSB    0x0E
#define ELE5_FILTERED_DATA_MSB    0x0F
#define ELE6_FILTERED_DATA_LSB    0x10
#define ELE6_FILTERED_DATA_MSB    0x11
#define ELE7_FILTERED_DATA_LSB    0x12
#define ELE7_FILTERED_DATA_MSB    0x13
#define ELE8_FILTERED_DATA_LSB    0x14
#define ELE8_FILTERED_DATA_MSB    0x15
#define ELE9_FILTERED_DATA_LSB    0x16
#define ELE9_FILTERED_DATA_MSB    0x17
#define ELE10_FILTERED_DATA_LSB   0x18
#define ELE10_FILTERED_DATA_MSB   0x19
#define ELE11_FILTERED_DATA_LSB   0x1A
#define ELE11_FILTERED_DATA_MSB   0x1B
#define ELEPROX_FILTERED_DATA_LSB 0x1C
#define ELEPROX_FILTERED_DATA_MSB 0x1D

#define ELE0_BASELINE_VALUE    0x1E
#define ELE1_BASELINE_VALUE    0x1F
#define ELE2_BASELINE_VALUE    0x20
#define ELE3_BASELINE_VALUE    0x21
#define ELE4_BASELINE_VALUE    0x22
#define ELE5_BASELINE_VALUE    0x23
#define ELE6_BASELINE_VALUE    0x24
#define ELE7_BASELINE_VALUE    0x25
#define ELE8_BASELINE_VALUE    0x26
#define ELE9_BASELINE_VALUE    0x27
#define ELE10_BASELINE_VALUE   0x28
#define ELE11_BASELINE_VALUE   0x29
#define ELEPROX_BASELINE_VALUE 0x2A

#define MHD_RISING                 0x2B
#define NHD_AMOUNT_RISING          0x2C
#define NCL_RISING                 0x2D
#define FDL_RISING                 0x2E
#define MHD_FALLING                0x2F
#define NHD_AMOUNT_FALLING         0x30
#define NCL_FALLING                0x31
#define FDL_FALLING                0x32
#define NHD_AMOUNT_TOUCHED         0x33
#define NCL_TOUCHED                0x34
#define FDL_TOUCHED                0x35
#define ELEPROX_MHD_RISING         0x36
#define ELEPROX_NHD_AMOUNT_RISING  0x37
#define ELEPROX_NCL_RISING         0x38
#define ELEPROX_FDL_RISING         0x39
#define ELEPROX_MHD_FALLING        0x3A
#define ELEPROX_NHD_AMOUNT_FALLING 0x3B
#define ELEPROX_FDL_FALLING        0x3C
#define ELEPROX_NHD_AMOUNT_TOUCHED 0x3E
#define ELEPROX_NCL_TOUCHED        0x3F
#define ELEPROX_FDL_TOUCHED        0x40

#define ELE0_TOUCH_THRESHOLD       0x41
#define ELE0_RELEASE_THRESHOLD     0x42
#define ELE1_TOUCH_THRESHOLD       0x43
#define ELE1_RELEASE_THRESHOLD     0x44
#define ELE2_TOUCH_THRESHOLD       0x45
#define ELE2_RELEASE_THRESHOLD     0x46
#define ELE3_TOUCH_THRESHOLD       0x47
#define ELE3_RELEASE_THRESHOLD     0x48
#define ELE4_TOUCH_THRESHOLD       0x49
#define ELE4_RELEASE_THRESHOLD     0x4A
#define ELE5_TOUCH_THRESHOLD       0x4B
#define ELE5_RELEASE_THRESHOLD     0x4C
#define ELE6_TOUCH_THRESHOLD       0x4D
#define ELE6_RELEASE_THRESHOLD     0x4E
#define ELE7_TOUCH_THRESHOLD       0x4F
#define ELE7_RELEASE_THRESHOLD     0x50
#define ELE8_TOUCH_THRESHOLD       0x51
#define ELE8_RELEASE_THRESHOLD     0x52
#define ELE9_TOUCH_THRESHOLD       0x53
#define ELE9_RELEASE_THRESHOLD     0x54
#define ELE10_TOUCH_THRESHOLD      0x55
#define ELE10_RELEASE_THRESHOLD    0x56
#define ELE11_TOUCH_THRESHOLD      0x57
#define ELE11_RELEASE_THRESHOLD    0x58
#define ELEPROX_TOUCH_THRESHOLD    0x59
#define ELEPROX_RELEASE_THRESHOLD  0x5A
#define DEBOUNCE_TOUCH_AND_RELEASE 0x5B
#define AFE_CONFIGURATION          0x5C

#define FILTER_CONFIG    0x5D
#define ELECTRODE_CONFIG 0x5E
#define ELE0_CURRENT     0x5F
#define ELE1_CURRENT     0x60
#define ELE2_CURRENT     0x61
#define ELE3_CURRENT     0x62
#define ELE4_CURRENT     0x63
#define ELE5_CURRENT     0x64
#define ELE6_CURRENT     0x65
#define ELE7_CURRENT     0x66
#define ELE8_CURRENT     0x67
#define ELE9_CURRENT     0x68
#define ELE10_CURRENT    0x69
#define ELE11_CURRENT    0x6A
#define ELEPROX_CURRENT  0x6B

#define ELE0_ELE1_CHARGE_TIME   0x6C
#define ELE2_ELE3_CHARGE_TIME   0x6D
#define ELE4_ELE5_CHARGE_TIME   0x6E
#define ELE6_ELE7_CHARGE_TIME   0x6F
#define ELE8_ELE9_CHARGE_TIME   0x70
#define ELE10_ELE11_CHARGE_TIME 0x71
#define ELEPROX_CHARGE_TIME     0x72

#define GPIO_CONTROL_0           0x73
#define GPIO_CONTROL_1           0x74
#define GPIO_DATA                0x75
#define GPIO_DIRECTION           0x76
#define GPIO_ENABLE              0x77
#define GPIO_SET                 0x78
#define GPIO_CLEAR               0x79
#define GPIO_TOGGLE              0x7A
#define AUTO_CONFIG_CONTROL_0    0x7B
#define AUTO_CONFIG_CONTROL_1    0x7C
#define AUTO_CONFIG_USL          0x7D
#define AUTO_CONFIG_LSL          0x7E
#define AUTO_CONFIG_TARGET_LEVEL 0x7F

// Other Constants
// these are suggested values from app note 3944
#define TOUCH_THRESHOLD   0x0F
#define RELEASE_THRESHOLD 0x0A
#define NUM_CHANNELS      12

class MPR121
{
  public:
    enum EventType {
      TOUCHED    = 0,
      RELEASED   = 1,
      NUM_EVENTS = 2
    };
  
    typedef void (*CallbackPtrType)(void);
    
    // constructor
    // takes a 7-b I2C address to use (0x5A by default, assumes addr pin grounded)
    MPR121(uint8_t address = MPR121_DEFAULT_ADDRESS);

    // write the configuration registers in accordance with the datasheet and app note 3944
    void initialize();
    
    // returns true if the device is responding on the I2C bus
    bool testConnection();

    // getTouchStatus returns the touch status for the given channel (0 - 11)
    bool getTouchStatus(uint8_t channel);
    // when not given a channel, returns a bitfield of all touch channels.
    uint16_t getTouchStatus();

    void setCallback(uint8_t channel, EventType event, CallbackPtrType callbackPtr);
    
    void serviceCallbacks();
    
  private:
    uint8_t m_devAddr; // contains the I2C address of the device
    CallbackPtrType m_callbackMap[NUM_CHANNELS][NUM_EVENTS];
    bool m_prevTouchStatus[NUM_CHANNELS];
    
};

#endif

