// I2Cdev library collection - ADXL345 I2C device class header file
// Based on Analog Devices ADXL345 datasheet rev. C, 5/2011
// 7/31/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-07-31 - initial release

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

#ifndef _ADXL345_H_
#define _ADXL345_H_

#include "I2Cdev.h"

#define ADXL345_ADDRESS_ALT_LOW     0x53 // alt address pin low (GND)
#define ADXL345_ADDRESS_ALT_HIGH    0x1D // alt address pin high (VCC)
#define ADXL345_DEFAULT_ADDRESS     ADXL345_ADDRESS_ALT_LOW

#define ADXL345_RA_DEVID            0x00
#define ADXL345_RA_RESERVED1        0x01
#define ADXL345_RA_THRESH_TAP       0x1D
#define ADXL345_RA_OFSX             0x1E
#define ADXL345_RA_OFSY             0x1F
#define ADXL345_RA_OFSZ             0x20
#define ADXL345_RA_DUR              0x21
#define ADXL345_RA_LATENT           0x22
#define ADXL345_RA_WINDOW           0x23
#define ADXL345_RA_THRESH_ACT       0x24
#define ADXL345_RA_THRESH_INACT     0x25
#define ADXL345_RA_TIME_INACT       0x26
#define ADXL345_RA_ACT_INACT_CTL    0x27
#define ADXL345_RA_THRESH_FF        0x28
#define ADXL345_RA_TIME_FF          0x29
#define ADXL345_RA_TAP_AXES         0x2A
#define ADXL345_RA_ACT_TAP_STATUS   0x2B
#define ADXL345_RA_BW_RATE          0x2C
#define ADXL345_RA_POWER_CTL        0x2D
#define ADXL345_RA_INT_ENABLE       0x2E
#define ADXL345_RA_INT_MAP          0x2F
#define ADXL345_RA_INT_SOURCE       0x30
#define ADXL345_RA_DATA_FORMAT      0x31
#define ADXL345_RA_DATAX0           0x32
#define ADXL345_RA_DATAX1           0x33
#define ADXL345_RA_DATAY0           0x34
#define ADXL345_RA_DATAY1           0x35
#define ADXL345_RA_DATAZ0           0x36
#define ADXL345_RA_DATAZ1           0x37
#define ADXL345_RA_FIFO_CTL         0x38
#define ADXL345_RA_FIFO_STATUS      0x39

#define ADXL345_AIC_ACT_AC_BIT      7
#define ADXL345_AIC_ACT_X_BIT       6
#define ADXL345_AIC_ACT_Y_BIT       5
#define ADXL345_AIC_ACT_Z_BIT       4
#define ADXL345_AIC_INACT_AC_BIT    3
#define ADXL345_AIC_INACT_X_BIT     2
#define ADXL345_AIC_INACT_Y_BIT     1
#define ADXL345_AIC_INACT_Z_BIT     0

#define ADXL345_TAPAXIS_SUP_BIT     3
#define ADXL345_TAPAXIS_X_BIT       2
#define ADXL345_TAPAXIS_Y_BIT       1
#define ADXL345_TAPAXIS_Z_BIT       0

#define ADXL345_TAPSTAT_ACTX_BIT    6
#define ADXL345_TAPSTAT_ACTY_BIT    5
#define ADXL345_TAPSTAT_ACTZ_BIT    4
#define ADXL345_TAPSTAT_ASLEEP_BIT  3
#define ADXL345_TAPSTAT_TAPX_BIT    2
#define ADXL345_TAPSTAT_TAPY_BIT    1
#define ADXL345_TAPSTAT_TAPZ_BIT    0

#define ADXL345_BW_LOWPOWER_BIT     4
#define ADXL345_BW_RATE_BIT         3
#define ADXL345_BW_RATE_LENGTH      4

#define ADXL345_RATE_3200           0b1111
#define ADXL345_RATE_1600           0b1110
#define ADXL345_RATE_800            0b1101
#define ADXL345_RATE_400            0b1100
#define ADXL345_RATE_200            0b1011
#define ADXL345_RATE_100            0b1010
#define ADXL345_RATE_50             0b1001
#define ADXL345_RATE_25             0b1000
#define ADXL345_RATE_12P5           0b0111
#define ADXL345_RATE_6P25           0b0110
#define ADXL345_RATE_3P13           0b0101
#define ADXL345_RATE_1P56           0b0100
#define ADXL345_RATE_0P78           0b0011
#define ADXL345_RATE_0P39           0b0010
#define ADXL345_RATE_0P20           0b0001
#define ADXL345_RATE_0P10           0b0000

#define ADXL345_PCTL_LINK_BIT       5
#define ADXL345_PCTL_AUTOSLEEP_BIT  4
#define ADXL345_PCTL_MEASURE_BIT    3
#define ADXL345_PCTL_SLEEP_BIT      2
#define ADXL345_PCTL_WAKEUP_BIT     1
#define ADXL345_PCTL_WAKEUP_LENGTH  2

#define ADXL345_WAKEUP_8HZ          0b00
#define ADXL345_WAKEUP_4HZ          0b01
#define ADXL345_WAKEUP_2HZ          0b10
#define ADXL345_WAKEUP_1HZ          0b11

#define ADXL345_INT_DATA_READY_BIT  7
#define ADXL345_INT_SINGLE_TAP_BIT  6
#define ADXL345_INT_DOUBLE_TAP_BIT  5
#define ADXL345_INT_ACTIVITY_BIT    4
#define ADXL345_INT_INACTIVITY_BIT  3
#define ADXL345_INT_FREE_FALL_BIT   2
#define ADXL345_INT_WATERMARK_BIT   1
#define ADXL345_INT_OVERRUN_BIT     0

#define ADXL345_FORMAT_SELFTEST_BIT 7
#define ADXL345_FORMAT_SPIMODE_BIT  6
#define ADXL345_FORMAT_INTMODE_BIT  5
#define ADXL345_FORMAT_FULL_RES_BIT 3
#define ADXL345_FORMAT_JUSTIFY_BIT  2
#define ADXL345_FORMAT_RANGE_BIT    1
#define ADXL345_FORMAT_RANGE_LENGTH 2

#define ADXL345_RANGE_2G            0b00
#define ADXL345_RANGE_4G            0b01
#define ADXL345_RANGE_8G            0b10
#define ADXL345_RANGE_16G           0b11

#define ADXL345_FIFO_MODE_BIT       7
#define ADXL345_FIFO_MODE_LENGTH    2
#define ADXL345_FIFO_TRIGGER_BIT    5
#define ADXL345_FIFO_SAMPLES_BIT    4
#define ADXL345_FIFO_SAMPLES_LENGTH 5

#define ADXL345_FIFO_MODE_BYPASS    0b00
#define ADXL345_FIFO_MODE_FIFO      0b01
#define ADXL345_FIFO_MODE_STREAM    0b10
#define ADXL345_FIFO_MODE_TRIGGER   0b11

#define ADXL345_FIFOSTAT_TRIGGER_BIT        7
#define ADXL345_FIFOSTAT_LENGTH_BIT         5
#define ADXL345_FIFOSTAT_LENGTH_LENGTH      6

class ADXL345 {
    public:
        ADXL345();
        ADXL345(uint8_t address);

        void initialize();
        bool testConnection();

        // DEVID register
        uint8_t getDeviceID();
        
        // THRESH_TAP register
        uint8_t getTapThreshold();
        void setTapThreshold(uint8_t threshold);

        // OFS* registers
        void getOffset(int8_t* x, int8_t* y, int8_t* z);
        void setOffset(int8_t x, int8_t y, int8_t z);
        int8_t getOffsetX();
        void setOffsetX(int8_t x);
        int8_t getOffsetY();
        void setOffsetY(int8_t y);
        int8_t getOffsetZ();
        void setOffsetZ(int8_t z);
        
        // DUR register
        uint8_t getTapDuration();
        void setTapDuration(uint8_t duration);
        
        // LATENT register
        uint8_t getDoubleTapLatency();
        void setDoubleTapLatency(uint8_t latency);
        
        // WINDOW register
        uint8_t getDoubleTapWindow();
        void setDoubleTapWindow(uint8_t window);
        
        // THRESH_ACT register
        uint8_t getActivityThreshold();
        void setActivityThreshold(uint8_t threshold);
        
        // THRESH_INACT register
        uint8_t getInactivityThreshold();
        void setInactivityThreshold(uint8_t threshold);

        // TIME_INACT register
        uint8_t getInactivityTime();
        void setInactivityTime(uint8_t time);
        
        // ACT_INACT_CTL register
        bool getActivityAC();
        void setActivityAC(bool enabled);
        bool getActivityXEnabled();
        void setActivityXEnabled(bool enabled);
        bool getActivityYEnabled();
        void setActivityYEnabled(bool enabled);
        bool getActivityZEnabled();
        void setActivityZEnabled(bool enabled);
        bool getInactivityAC();
        void setInactivityAC(bool enabled);
        bool getInactivityXEnabled();
        void setInactivityXEnabled(bool enabled);
        bool getInactivityYEnabled();
        void setInactivityYEnabled(bool enabled);
        bool getInactivityZEnabled();
        void setInactivityZEnabled(bool enabled);
        
        // THRESH_FF register
        uint8_t getFreefallThreshold();
        void setFreefallThreshold(uint8_t threshold);
        
        // TIME_FF register
        uint8_t getFreefallTime();
        void setFreefallTime(uint8_t time);
        
        // TAP_AXES register
        bool getTapAxisSuppress();
        void setTapAxisSuppress(bool enabled);
        bool getTapAxisXEnabled();
        void setTapAxisXEnabled(bool enabled);
        bool getTapAxisYEnabled();
        void setTapAxisYEnabled(bool enabled);
        bool getTapAxisZEnabled();
        void setTapAxisZEnabled(bool enabled);
        
        // ACT_TAP_STATUS register
        bool getActivitySourceX();
        bool getActivitySourceY();
        bool getActivitySourceZ();
        bool getAsleep();
        bool getTapSourceX();
        bool getTapSourceY();
        bool getTapSourceZ();
        
        // BW_RATE register
        bool getLowPowerEnabled();
        void setLowPowerEnabled(bool enabled);
        uint8_t getRate();
        void setRate(uint8_t rate);

        // POWER_CTL register
        bool getLinkEnabled();
        void setLinkEnabled(bool enabled);
        bool getAutoSleepEnabled();
        void setAutoSleepEnabled(bool enabled);
        bool getMeasureEnabled();
        void setMeasureEnabled(bool enabled);
        bool getSleepEnabled();
        void setSleepEnabled(bool enabled);
        uint8_t getWakeupFrequency();
        void setWakeupFrequency(uint8_t frequency);
        
        // INT_ENABLE register
        bool getIntDataReadyEnabled();
        void setIntDataReadyEnabled(bool enabled);
        bool getIntSingleTapEnabled();
        void setIntSingleTapEnabled(bool enabled);
        bool getIntDoubleTapEnabled();
        void setIntDoubleTapEnabled(bool enabled);
        bool getIntActivityEnabled();
        void setIntActivityEnabled(bool enabled);
        bool getIntInactivityEnabled();
        void setIntInactivityEnabled(bool enabled);
        bool getIntFreefallEnabled();
        void setIntFreefallEnabled(bool enabled);
        bool getIntWatermarkEnabled();
        void setIntWatermarkEnabled(bool enabled);
        bool getIntOverrunEnabled();
        void setIntOverrunEnabled(bool enabled);
        
        // INT_MAP register
        uint8_t getIntDataReadyPin();
        void setIntDataReadyPin(uint8_t pin);
        uint8_t getIntSingleTapPin();
        void setIntSingleTapPin(uint8_t pin);
        uint8_t getIntDoubleTapPin();
        void setIntDoubleTapPin(uint8_t pin);
        uint8_t getIntActivityPin();
        void setIntActivityPin(uint8_t pin);
        uint8_t getIntInactivityPin();
        void setIntInactivityPin(uint8_t pin);
        uint8_t getIntFreefallPin();
        void setIntFreefallPin(uint8_t pin);
        uint8_t getIntWatermarkPin();
        void setIntWatermarkPin(uint8_t pin);
        uint8_t getIntOverrunPin();
        void setIntOverrunPin(uint8_t pin);

        // INT_SOURCE register
        uint8_t getIntDataReadySource();
        uint8_t getIntSingleTapSource();
        uint8_t getIntDoubleTapSource();
        uint8_t getIntActivitySource();
        uint8_t getIntInactivitySource();
        uint8_t getIntFreefallSource();
        uint8_t getIntWatermarkSource();
        uint8_t getIntOverrunSource();
        
        // DATA_FORMAT register
        uint8_t getSelfTestEnabled();
        void setSelfTestEnabled(uint8_t enabled);
        uint8_t getSPIMode();
        void setSPIMode(uint8_t mode);
        uint8_t getInterruptMode();
        void setInterruptMode(uint8_t mode);
        uint8_t getFullResolution();
        void setFullResolution(uint8_t resolution);
        uint8_t getDataJustification();
        void setDataJustification(uint8_t justification);
        uint8_t getRange();
        void setRange(uint8_t range);

        // DATA* registers
        void getAcceleration(int16_t* x, int16_t* y, int16_t* z);
        int16_t getAccelerationX();
        int16_t getAccelerationY();
        int16_t getAccelerationZ();

        // FIFO_CTL register
        uint8_t getFIFOMode();
        void setFIFOMode(uint8_t mode);
        uint8_t getFIFOTriggerInterruptPin();
        void setFIFOTriggerInterruptPin(uint8_t interrupt);
        uint8_t getFIFOSamples();
        void setFIFOSamples(uint8_t size);
        
        // FIFO_STATUS register
        bool getFIFOTriggerOccurred();
        uint8_t getFIFOLength();

    private:
        uint8_t devAddr;
        uint8_t buffer[6];
};

#endif /* _ADXL345_H_ */
