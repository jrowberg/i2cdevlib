// I2Cdev library collection - DS1307 I2C device class
// Based on Maxim DS1307 datasheet, 2008
// 11/13/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2011-11-13 - initial release
//     2012-03-29 - alain.spineux@gmail.com: bug in getHours24() 
//                  am/pm is bit 0x20 instead of 0x80
//

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

#include "DS1307.h"

/** Default constructor, uses default I2C address.
 * @see DS1307_DEFAULT_ADDRESS
 */
DS1307::DS1307() {
    devAddr = DS1307_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see DS1307_DEFAULT_ADDRESS
 * @see DS1307_ADDRESS
 */
DS1307::DS1307(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This method reads the current 12/24-hour mode setting into the locally stored
 * variable so that it isn't accidentally changed by the setHour*() methods.
 */
void DS1307::initialize() {
    getMode(); // automatically sets internal "mode12" member variable
    getClockRunning(); // automatically sets internal "clockHalt" member variable
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool DS1307::testConnection() {
    if (I2Cdev::readByte(devAddr, DS1307_RA_SECONDS, buffer) == 1) {
        return true;
    }
    return false;
}

// SECONDS register
bool DS1307::getClockRunning() {
    I2Cdev::readBit(devAddr, DS1307_RA_SECONDS, DS1307_SECONDS_CH_BIT, buffer);
    clockHalt = buffer[0];
    return !buffer[0];
}
void DS1307::setClockRunning(bool running) {
    I2Cdev::writeBit(devAddr, DS1307_RA_SECONDS, DS1307_SECONDS_CH_BIT, !running);
}
uint8_t DS1307::getSeconds() {
    // Byte: [7 = CH] [6:4 = 10SEC] [3:0 = 1SEC]
    I2Cdev::readByte(devAddr, DS1307_RA_SECONDS, buffer);
    clockHalt = buffer[0] & 0x80;
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;
}
void DS1307::setSeconds(uint8_t seconds) {
    if (seconds > 59) return;
    uint8_t value = (clockHalt ? 0x80 : 0x00) + ((seconds / 10) << 4) + (seconds % 10);
    I2Cdev::writeByte(devAddr, DS1307_RA_SECONDS, value);
}

// MINUTES register
uint8_t DS1307::getMinutes() {
    // Byte: [7 = 0] [6:4 = 10MIN] [3:0 = 1MIN]
    I2Cdev::readByte(devAddr, DS1307_RA_MINUTES, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;
}
void DS1307::setMinutes(uint8_t minutes) {
    if (minutes > 59) return;
    uint8_t value = ((minutes / 10) << 4) + (minutes % 10);
    I2Cdev::writeByte(devAddr, DS1307_RA_MINUTES, value);
}

// HOURS register
uint8_t DS1307::getMode() {
    I2Cdev::readBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_MODE_BIT, buffer);
    mode12 = buffer[0];
    return buffer[0];
}
void DS1307::setMode(uint8_t mode) {
    I2Cdev::writeBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_MODE_BIT, mode);
}
uint8_t DS1307::getAMPM() {
    I2Cdev::readBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_AMPM_BIT, buffer);
    return buffer[0];
}
void DS1307::setAMPM(uint8_t ampm) {
    I2Cdev::writeBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_AMPM_BIT, ampm);
}
uint8_t DS1307::getHours12() {
    I2Cdev::readByte(devAddr, DS1307_RA_HOURS, buffer);
    mode12 = buffer[0] & 0x40;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        uint8_t hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;

        // convert 24-hour to 12-hour format, since that's what's requested
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        return hours;
    }
}
void DS1307::setHours12(uint8_t hours, uint8_t ampm) {
    if (hours > 12 || hours < 1) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) ampm = 0x20;
        uint8_t value = 0x40 + ampm + ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, DS1307_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) hours += 12;
        if (hours == 0) hours = 12; // 12 AM
        else if (hours == 24) hours = 12; // 12 PM, after +12 adjustment
        uint8_t value = ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, DS1307_RA_HOURS, value);
    }
}
uint8_t DS1307::getHours24() {
    I2Cdev::readByte(devAddr, DS1307_RA_HOURS, buffer);
    mode12 = buffer[0] & 0x40;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        uint8_t hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;

        // convert 12-hour to 24-hour format, since that's what's requested
        if (buffer[0] & 0x20) {
            // currently PM
            if (hours < 12) hours += 12;
        } else {
            // currently AM
            if (hours == 12) hours = 0;
        }
        return hours;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
    }
}
void DS1307::setHours24(uint8_t hours) {
    if (hours > 23) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        uint8_t ampm = 0;
        if (hours > 11) ampm = 0x20;
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        uint8_t value = 0x40 + ampm + ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, DS1307_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        uint8_t value = ((hours / 10) << 4) + (hours % 10);
        I2Cdev::writeByte(devAddr, DS1307_RA_HOURS, value);
    }
}

// DAY register
uint8_t DS1307::getDayOfWeek() {
    I2Cdev::readBits(devAddr, DS1307_RA_DAY, DS1307_DAY_BIT, DS1307_DAY_LENGTH, buffer);
    return buffer[0];
}
void DS1307::setDayOfWeek(uint8_t dow) {
    if (dow < 1 || dow > 7) return;
    I2Cdev::writeBits(devAddr, DS1307_RA_DAY, DS1307_DAY_BIT, DS1307_DAY_LENGTH, dow);
}

// DATE register
uint8_t DS1307::getDay() {
    // Byte: [7:6 = 0] [5:4 = 10DAY] [3:0 = 1DAY]
    I2Cdev::readByte(devAddr, DS1307_RA_DATE, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
}
void DS1307::setDay(uint8_t day) {
    uint8_t value = ((day / 10) << 4) + (day % 10);
    I2Cdev::writeByte(devAddr, DS1307_RA_DATE, value);
}

// MONTH register
uint8_t DS1307::getMonth() {
    // Byte: [7:5 = 0] [4 = 10MONTH] [3:0 = 1MONTH]
    I2Cdev::readByte(devAddr, DS1307_RA_MONTH, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
}
void DS1307::setMonth(uint8_t month) {
    if (month < 1 || month > 12) return;
    uint8_t value = ((month / 10) << 4) + (month % 10);
    I2Cdev::writeByte(devAddr, DS1307_RA_MONTH, value);
}

// YEAR register
uint16_t DS1307::getYear() {
    I2Cdev::readByte(devAddr, DS1307_RA_YEAR, buffer);
    return 2000 + (buffer[0] & 0x0F) + ((buffer[0] & 0xF0) >> 4) * 10;
}
void DS1307::setYear(uint16_t year) {
    if (year < 2000) return;
    year -= 2000;
    uint8_t value = ((year / 10) << 4) + (year % 10);
    I2Cdev::writeByte(devAddr, DS1307_RA_YEAR, value);
}

// CONTROL register
bool DS1307::getFixedOutputLevel() {
    I2Cdev::readBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_OUT_BIT, buffer);
    return buffer[0];
}
void DS1307::setFixedOutputLevel(bool level) {
    I2Cdev::writeBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_OUT_BIT, level);
}
bool DS1307::getSquareWaveEnabled() {
    I2Cdev::readBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_SQWE_BIT, buffer);
    return buffer[0];
}
void DS1307::setSquareWaveEnabled(bool enabled) {
    I2Cdev::writeBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_SQWE_BIT, enabled);
}
uint8_t DS1307::getSquareWaveRate() {
    I2Cdev::readBits(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_RS_BIT, DS1307_CONTROL_RS_LENGTH, buffer);
    return buffer[0];
}
void DS1307::setSquareWaveRate(uint8_t rate) {
    I2Cdev::writeBits(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_RS_BIT, DS1307_CONTROL_RS_LENGTH, rate);
}

// RAM registers
uint8_t DS1307::getMemoryByte(uint8_t offset) {
    if (offset > 55) return 0;
    I2Cdev::readByte(devAddr, DS1307_RA_RAM + offset, buffer);
    return buffer[0];
}
void DS1307::setMemoryByte(uint8_t offset, uint8_t value) {
    if (offset > 55) return;
    I2Cdev::writeByte(devAddr, DS1307_RA_RAM + offset, value);
}

// convenience methods

void DS1307::getDate(uint16_t *year, uint8_t *month, uint8_t *day) {
    *year = getYear();
    *month = getMonth();
    *day = getDay();
}
void DS1307::setDate(uint16_t year, uint8_t month, uint8_t day) {
    setYear(year);
    setMonth(month);
    setDay(day);
}

void DS1307::getTime12(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm) {
    *hours = getHours12();
    *minutes = getMinutes();
    *seconds = getSeconds();
    *ampm = getAMPM();
}
void DS1307::setTime12(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm) {
    // write seconds first to reset divider chain and give
    // us 1 second to write remaining time info
    setSeconds(seconds);
    setMinutes(minutes);
    setHours12(hours, ampm);
}

void DS1307::getTime24(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    *hours = getHours24();
    *minutes = getMinutes();
    *seconds = getSeconds();
}
void DS1307::setTime24(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    // write seconds first to reset divider chain and give
    // us 1 second to write remaining time info
    setSeconds(seconds);
    setMinutes(minutes);
    setHours24(hours);
}

void DS1307::getDateTime12(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm) {
    getTime12(hours, minutes, seconds, ampm);
    getDate(year, month, day);
}
void DS1307::setDateTime12(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm) {
    setTime12(hours, minutes, seconds, ampm);
    setDate(year, month, day);
}

void DS1307::getDateTime24(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    getTime24(hours, minutes, seconds);
    getDate(year, month, day);
}
void DS1307::setDateTime24(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    setTime24(hours, minutes, seconds);
    setDate(year, month, day);
}

#ifdef DS1307_INCLUDE_DATETIME_METHODS
    DateTime DS1307::getDateTime() {
        DateTime dt = DateTime(getYear(), getMonth(), getDay(), getHours24(), getMinutes(), getSeconds());
        return dt;
    }
    void DS1307::setDateTime(DateTime dt) {
        setSeconds(dt.second());
        setMinutes(dt.minute());
        setHours24(dt.hour());
        setDay(dt.day());
        setMonth(dt.month());
        setYear(dt.year());
    }
#endif

#ifdef DS1307_INCLUDE_DATETIME_CLASS
    // DateTime class courtesy of public domain JeeLabs code
    #include <avr/pgmspace.h>
    #define SECONDS_PER_DAY 86400L
    #define SECONDS_FROM_1970_TO_2000 946684800
    
    ////////////////////////////////////////////////////////////////////////////////
    // utility code, some of this could be exposed in the DateTime API if needed
    
    static uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    
    // number of days since 2000/01/01, valid for 2001..2099
    static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
        if (y >= 2000)
            y -= 2000;
        uint16_t days = d;
        for (uint8_t i = 1; i < m; ++i)
            days += pgm_read_byte(daysInMonth + i - 1);
        if (m > 2 && y % 4 == 0)
            ++days;
        return days + 365 * y + (y + 3) / 4 - 1;
    }
    
    static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
        return ((days * 24L + h) * 60 + m) * 60 + s;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // DateTime implementation - ignores time zones and DST changes
    // NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second
    
    DateTime::DateTime (uint32_t t) {
      t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970
    
        ss = t % 60;
        t /= 60;
        mm = t % 60;
        t /= 60;
        hh = t % 24;
        uint16_t days = t / 24;
        uint8_t leap;
        for (yOff = 0; ; ++yOff) {
            leap = yOff % 4 == 0;
            if (days < 365 + leap)
                break;
            days -= 365 + leap;
        }
        for (m = 1; ; ++m) {
            uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
            if (leap && m == 2)
                ++daysPerMonth;
            if (days < daysPerMonth)
                break;
            days -= daysPerMonth;
        }
        d = days + 1;
    }
    
    DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
        if (year >= 2000)
            year -= 2000;
        yOff = year;
        m = month;
        d = day;
        hh = hour;
        mm = min;
        ss = sec;
    }
    
    static uint8_t conv2d(const char* p) {
        uint8_t v = 0;
        if ('0' <= *p && *p <= '9')
            v = *p - '0';
        return 10 * v + *++p - '0';
    }
    
    // A convenient constructor for using "the compiler's time":
    //   DateTime now (__DATE__, __TIME__);
    // NOTE: using PSTR would further reduce the RAM footprint
    DateTime::DateTime (const char* date, const char* time) {
        // sample input: date = "Dec 26 2009", time = "12:34:56"
        yOff = conv2d(date + 9);
        // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
        switch (date[0]) {
            case 'J': m = (date[1] == 'a' ? 1 : (date[2] == 'n' ? 6 : 7)); break;
            case 'F': m = 2; break;
            case 'A': m = date[2] == 'r' ? 4 : 8; break;
            case 'M': m = date[2] == 'r' ? 3 : 5; break;
            case 'S': m = 9; break;
            case 'O': m = 10; break;
            case 'N': m = 11; break;
            case 'D': m = 12; break;
        }
        d = conv2d(date + 4);
        hh = conv2d(time);
        mm = conv2d(time + 3);
        ss = conv2d(time + 6);
    }
    
    uint8_t DateTime::dayOfWeek() const {    
        uint16_t day = secondstime() / SECONDS_PER_DAY;
        return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
    }
    
    uint32_t DateTime::unixtime(void) const {
        return secondstime() + SECONDS_FROM_1970_TO_2000;
    }
    
    long DateTime::secondstime(void) const {
        uint32_t t;
        uint16_t days = date2days(yOff, m, d);
        t = time2long(days, hh, mm, ss);    
        return t;
    }
#endif
