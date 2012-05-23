// I2Cdev library collection - HMC5843 I2C device class header file
// Based on Honeywell HMC5843 datasheet, 6/2010 (Form #900367)
// 8/22/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-08-22 - initial release

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

#ifndef _HMC5843_H_
#define _HMC5843_H_

#include "I2Cdev.h"

#define HMC5843_ADDRESS            0x1E // this device only has one address
#define HMC5843_DEFAULT_ADDRESS    0x1E

#define HMC5843_RA_CONFIG_A        0x00
#define HMC5843_RA_CONFIG_B        0x01
#define HMC5843_RA_MODE            0x02
#define HMC5843_RA_DATAX_H         0x03
#define HMC5843_RA_DATAX_L         0x04
#define HMC5843_RA_DATAY_H         0x05
#define HMC5843_RA_DATAY_L         0x06
#define HMC5843_RA_DATAZ_H         0x07
#define HMC5843_RA_DATAZ_L         0x08
#define HMC5843_RA_STATUS          0x09
#define HMC5843_RA_ID_A            0x0A
#define HMC5843_RA_ID_B            0x0B
#define HMC5843_RA_ID_C            0x0C

#define HMC5843_CRA_RATE_BIT       4
#define HMC5843_CRA_RATE_LENGTH    3
#define HMC5843_CRA_BIAS_BIT       1
#define HMC5843_CRA_BIAS_LENGTH    2

#define HMC5843_RATE_0P5           0x00
#define HMC5843_RATE_1             0x01
#define HMC5843_RATE_2             0x02
#define HMC5843_RATE_5             0x03
#define HMC5843_RATE_10            0x04 // default
#define HMC5843_RATE_20            0x05
#define HMC5843_RATE_50            0x06

#define HMC5843_BIAS_NORMAL        0x00 // default
#define HMC5843_BIAS_POSITIVE      0x01
#define HMC5843_BIAS_NEGATIVE      0x02

#define HMC5843_CRB_GAIN_BIT       7
#define HMC5843_CRB_GAIN_LENGTH    3

#define HMC5843_GAIN_1620          0x00
#define HMC5843_GAIN_1300          0x01 // default
#define HMC5843_GAIN_970           0x02
#define HMC5843_GAIN_780           0x03
#define HMC5843_GAIN_530           0x04
#define HMC5843_GAIN_460           0x05
#define HMC5843_GAIN_390           0x06
#define HMC5843_GAIN_280           0x07 // not recommended

#define HMC5843_MODEREG_BIT        1
#define HMC5843_MODEREG_LENGTH     2

/**
 * During continuous-measurement mode, the device continuously makes
 * measurements and places measured data in data output registers. Settings in
 * the configuration register affect the data output rate (bits DO[n]), the
 * measurement configuration (bits MS[n]), and the gain (bits GN[n]) when in
 * continuous-measurement mode. To conserve current between measurements, the
 * device is placed in a state similar to idle mode, but the mode is not changed
 * to idle mode. That is, MD[n] bits are unchanged. Data can be re-read from the
 * data output registers if necessary; however, if the master does not ensure
 * that the data register is accessed before the completion of the next
 * measurement, the new measurement may be lost. All registers maintain values
 * while in continuous-measurement mode. The I2C bus is enabled for use by other
 * devices on the network in while continuous-measurement mode.
 */
#define HMC5843_MODE_CONTINUOUS    0x00
/**
 * This is the default single supply power-up mode. In dual supply configuration
 * this is the default mode when AVDD goes high. During single-measurement mode,
 * the device makes a single measurement and places the measured data in data
 * output registers. Settings in the configuration register affect the
 * measurement configuration (bits MS[n]), and the gain (bits GN[n]) when in
 * single-measurement mode. After the measurement is complete and output data
 * registers are updated, the device is placed sleep mode, and the mode register
 * is changed to sleep mode by setting MD[n] bits. All registers maintain values
 * while in single-measurement mode. The I2C bus is enabled for use by other
 * devices on the network while in single-measurement mode.
 */
#define HMC5843_MODE_SINGLE        0x01
/**
 * During this mode the device is accessible through the I2C bus, but major
 * sources of power consumption are disabled, such as, but not limited to, the
 * ADC, the amplifier, the SVDD pin, and the sensor bias current. All registers
 * maintain values while in idle mode. The I2C bus is enabled for use by other
 * devices on the network while in idle mode.
 */
#define HMC5843_MODE_IDLE          0x02
/**
 * This is the default dual supply power-up mode when only DVDD goes high and
 * AVDD remains low. During sleep mode the device functionality is limited to
 * listening to the I2C bus. The internal clock is not running and register
 * values are not maintained while in sleep mode. The only functionality that
 * exists during this mode is the device is able to recognize and execute any
 * instructions specific to this device but does not change from sleep mode due
 * to other traffic on the I2C bus. The I2C bus is enabled for use by other
 * devices on the network while in sleep mode. This mode has two practical
 * differences from idle mode. First this state will create less noise on system
 * since the clock is disabled, and secondly this state is a lower current
 * consuming state since the clock is disabled.
 */
#define HMC5843_MODE_SLEEP         0x03

#define HMC5843_STATUS_REN_BIT     2
#define HMC5843_STATUS_LOCK_BIT    1
#define HMC5843_STATUS_READY_BIT   0

class HMC5843 {
    public:
        HMC5843();
        HMC5843(uint8_t address);
        
        void initialize();
        bool testConnection();

        // CONFIG_A register
        uint8_t getDataRate();
        void setDataRate(uint8_t rate);
        uint8_t getMeasurementBias();
        void setMeasurementBias(uint8_t bias);

        // CONFIG_B register
        uint8_t getGain();
        void setGain(uint8_t gain);

        // MODE register
        uint8_t getMode();
        void setMode(uint8_t mode);

        // DATA* registers
        void getHeading(int16_t *x, int16_t *y, int16_t *z);
        int16_t getHeadingX();
        int16_t getHeadingY();
        int16_t getHeadingZ();

        // STATUS register
        bool getRegulatorEnabledStatus();
        bool getLockStatus();
        bool getReadyStatus();

        // ID_* registers
        uint8_t getIDA();
        uint8_t getIDB();
        uint8_t getIDC();

    private:
        uint8_t devAddr;
        uint8_t buffer[6];
        uint8_t mode;
};

#endif /* _HMC5843_H_ */
