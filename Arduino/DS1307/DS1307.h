// I2Cdev library collection - DS1307 I2C device class header file
// Based on Maxim DS1307 datasheet, 2008
// 11/13/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2011-11-13 - initial release

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

#ifndef _DS1307_H_
#define _DS1307_H_

#include "I2Cdev.h"

// comment this out if you already have a JeeLabs' DateTime class in your code
// or if you don't need DateTime functionality
#define DS1307_INCLUDE_DATETIME_CLASS

// comment this out if you don't need DateTime functionality
#define DS1307_INCLUDE_DATETIME_METHODS

#define DS1307_ADDRESS              0x68 // this device only has one address
#define DS1307_DEFAULT_ADDRESS      0x68

#define DS1307_RA_SECONDS           0x00
#define DS1307_RA_MINUTES           0x01
#define DS1307_RA_HOURS             0x02
#define DS1307_RA_DAY               0x03
#define DS1307_RA_DATE              0x04
#define DS1307_RA_MONTH             0x05
#define DS1307_RA_YEAR              0x06
#define DS1307_RA_CONTROL           0x07
#define DS1307_RA_RAM               0x08

#define DS1307_SECONDS_CH_BIT       7
#define DS1307_SECONDS_10_BIT       6
#define DS1307_SECONDS_10_LENGTH    3
#define DS1307_SECONDS_1_BIT        3
#define DS1307_SECONDS_1_LENGTH     4

#define DS1307_MINUTES_10_BIT       6
#define DS1307_MINUTES_10_LENGTH    3
#define DS1307_MINUTES_1_BIT        3
#define DS1307_MINUTES_1_LENGTH     4

#define DS1307_HOURS_MODE_BIT       6 // 0 = 24-hour mode, 1 = 12-hour mode
#define DS1307_HOURS_AMPM_BIT       5 // 2nd HOURS_10 bit if in 24-hour mode
#define DS1307_HOURS_10_BIT         4
#define DS1307_HOURS_1_BIT          3
#define DS1307_HOURS_1_LENGTH       4

#define DS1307_DAY_BIT              2
#define DS1307_DAY_LENGTH           3

#define DS1307_DATE_10_BIT          5
#define DS1307_DATE_10_LENGTH       2
#define DS1307_DATE_1_BIT           3
#define DS1307_DATE_1_LENGTH        4

#define DS1307_MONTH_10_BIT         4
#define DS1307_MONTH_1_BIT          3
#define DS1307_MONTH_1_LENGTH       4

#define DS1307_YEAR_10H_BIT         7
#define DS1307_YEAR_10H_LENGTH      4
#define DS1307_YEAR_1H_BIT          3
#define DS1307_YEAR_1H_LENGTH       4

#define DS1307_CONTROL_OUT_BIT      7
#define DS1307_CONTROL_SQWE_BIT     4
#define DS1307_CONTROL_RS_BIT       1
#define DS1307_CONTROL_RS_LENGTH    2

#define DS1307_SQW_RATE_1           0x0
#define DS1307_SQW_RATE_4096        0x1
#define DS1307_SQW_RATE_8192        0x2
#define DS1307_SQW_RATE_32768       0x3

#ifdef DS1307_INCLUDE_DATETIME_CLASS
    // DateTime class courtesy of public domain JeeLabs code
    // simple general-purpose date/time class (no TZ / DST / leap second handling!)
    class DateTime {
        public:
            DateTime (uint32_t t=0);
            DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t min=0, uint8_t sec=0);
            DateTime (const char* date, const char* time);
            uint16_t year() const       { return 2000 + yOff; }
            uint8_t month() const       { return m; }
            uint8_t day() const         { return d; }
            uint8_t hour() const        { return hh; }
            uint8_t minute() const      { return mm; }
            uint8_t second() const      { return ss; }
            uint8_t dayOfWeek() const;
    
            // 32-bit times as seconds since 1/1/2000
            long secondstime() const;
            // 32-bit times as seconds since 1/1/1970
            uint32_t unixtime(void) const;
        
        protected:
            uint8_t yOff, m, d, hh, mm, ss;
    };
#endif

class DS1307 {
    public:
        DS1307();
        DS1307(uint8_t address);

        void initialize();
        bool testConnection();

        // SECONDS register
        bool getClockRunning();
        void setClockRunning(bool running);
        uint8_t getSeconds(); // 0-59
        void setSeconds(uint8_t seconds);

        // MINUTES register
        uint8_t getMinutes(); // 0-59
        void setMinutes(uint8_t minutes);

        // HOURS register
        uint8_t getMode(); // 0-1
        void setMode(uint8_t mode);
        uint8_t getAMPM(); // 0-1
        void setAMPM(uint8_t ampm);
        uint8_t getHours12(); // 1-12
        void setHours12(uint8_t hours, uint8_t ampm);
        uint8_t getHours24(); // 0-23
        void setHours24(uint8_t hours);

        // DAY register
        uint8_t getDayOfWeek(); // 1-7
        void setDayOfWeek(uint8_t dow);

        // DATE register
        uint8_t getDay(); // 1-31
        void setDay(uint8_t day);
        
        // MONTH register
        uint8_t getMonth(); // 1-12
        void setMonth(uint8_t month);

        // YEAR register
        uint16_t getYear(); // 1970, 2000, 2011, etc
        void setYear(uint16_t year);
        
        // CONTROL register
        bool getFixedOutputLevel();
        void setFixedOutputLevel(bool level);
        bool getSquareWaveEnabled();
        void setSquareWaveEnabled(bool enabled);
        uint8_t getSquareWaveRate();
        void setSquareWaveRate(uint8_t rate);
        
        // RAM registers
        uint8_t getMemoryByte(uint8_t offset);
        void setMemoryByte(uint8_t offset, uint8_t value);

        // convenience methods

        void getDate(uint16_t *year, uint8_t *month, uint8_t *day);
        void setDate(uint16_t year, uint8_t month, uint8_t day);

        void getTime12(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm);
        void setTime12(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm);
        
        void getTime24(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
        void setTime24(uint8_t hours, uint8_t minutes, uint8_t seconds);
        
        void getDateTime12(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm);
        void setDateTime12(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm);
        
        void getDateTime24(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
        void setDateTime24(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds);
        
        #ifdef DS1307_INCLUDE_DATETIME_METHODS
            DateTime getDateTime();
            void setDateTime(DateTime dt);
        #endif

    private:
        uint8_t devAddr;
        uint8_t buffer[1];
        bool mode12;
        bool clockHalt;
};

#endif /* _DS1307_H_ */
