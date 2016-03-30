// I2Cdev library collection - iAQ-2000 I2C device class header file
// Based on AppliedSensor iAQ-2000 Interface Description, Version PA1, 2009
// 2012-04-01 by Peteris Skorovs <pskorovs@gmail.com>
//
// This I2C device library is using (and submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-04-01 - initial release
//     2015-11-08 - added TVoc and Status

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Peteris Skorovs, Jeff Rowberg

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

#ifndef _IAQ2000_H_
#define _IAQ2000_H_

#include "I2Cdev.h"

#define IAQ2000_ADDRESS             0x5A
#define IAQ2000_DEFAULT_ADDRESS     IAQ2000_ADDRESS

#define IAQ2000_RA_DATA1 0x00
#define IAQ2000_RA_DATA2 0x01

class IAQ2000 {
    public:
        IAQ2000();
        IAQ2000(uint8_t address);
        void initialize();
	bool testConnection();
	uint16_t getIaqtvoc();
        uint16_t getIaqpred();
	uint8_t getIaqstatus();

    private:
        uint8_t devAddr;
        uint8_t buffer[8];
    
        int8_t readAllBytes(uint8_t devAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout);
};

#endif /* _IAQ200_H_ */
