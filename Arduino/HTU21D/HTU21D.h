// I2Cdev library collection - HTU21D I2C device class header file
// Based on MEAS HTU21D HPC199_2 HTU321(F) datasheet, October 2013
// 2016-03-24 by https://github.com/eadf
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2016-03-24 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2016 Eadf, Jeff Rowberg

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

#ifndef _HTU21D_H_
#define _HTU21D_H_

#include "I2Cdev.h"

#define HTU21D_DEFAULT_ADDRESS     0x40

#define HTU21D_RA_TEMPERATURE      0xE3
#define HTU21D_RA_HUMIDITY         0xE5
#define HTU21D_RESET               0xFE
#define HTU21D_WRITE_USER_REGISTER 0xE6
#define HTU21D_READ_USER_REGISTER  0xE7

class HTU21D {
    public:
        HTU21D();

        void initialize();
        bool testConnection();

        float getTemperature();
        float getHumidity();

        void reset();

    private:
        uint8_t devAddr;
        uint8_t buffer[2];
};

#endif /* _HTU21D_H_ */
