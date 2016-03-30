// I2Cdev library collection - BMP085 I2C device class
// Based on register information stored in the I2Cdevlib internal database
// 2012-06-28 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-06-28 - initial release, dynamically built

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

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

#ifndef _BMP085_H_
#define _BMP085_H_

#include "I2Cdev.h"
#include <math.h>

#define BMP085_ADDRESS              0x77
#define BMP085_DEFAULT_ADDRESS      BMP085_ADDRESS

#define BMP085_RA_AC1_H     0xAA    /* AC1_H */
#define BMP085_RA_AC1_L     0xAB    /* AC1_L */
#define BMP085_RA_AC2_H     0xAC    /* AC2_H */
#define BMP085_RA_AC2_L     0xAD    /* AC2_L */
#define BMP085_RA_AC3_H     0xAE    /* AC3_H */
#define BMP085_RA_AC3_L     0xAF    /* AC3_L */
#define BMP085_RA_AC4_H     0xB0    /* AC4_H */
#define BMP085_RA_AC4_L     0xB1    /* AC4_L */
#define BMP085_RA_AC5_H     0xB2    /* AC5_H */
#define BMP085_RA_AC5_L     0xB3    /* AC5_L */
#define BMP085_RA_AC6_H     0xB4    /* AC6_H */
#define BMP085_RA_AC6_L     0xB5    /* AC6_L */
#define BMP085_RA_B1_H      0xB6    /* B1_H */
#define BMP085_RA_B1_L      0xB7    /* B1_L */
#define BMP085_RA_B2_H      0xB8    /* B2_H */
#define BMP085_RA_B2_L      0xB9    /* B2_L */
#define BMP085_RA_MB_H      0xBA    /* MB_H */
#define BMP085_RA_MB_L      0xBB    /* MB_L */
#define BMP085_RA_MC_H      0xBC    /* MC_H */
#define BMP085_RA_MC_L      0xBD    /* MC_L */
#define BMP085_RA_MD_H      0xBE    /* MD_H */
#define BMP085_RA_MD_L      0xBF    /* MD_L */
#define BMP085_RA_CONTROL   0xF4    /* CONTROL */
#define BMP085_RA_MSB       0xF6    /* MSB */
#define BMP085_RA_LSB       0xF7    /* LSB */
#define BMP085_RA_XLSB      0xF8    /* XLSB */

#define BMP085_MODE_TEMPERATURE     0x2E
#define BMP085_MODE_PRESSURE_0      0x34
#define BMP085_MODE_PRESSURE_1      0x74
#define BMP085_MODE_PRESSURE_2      0xB4
#define BMP085_MODE_PRESSURE_3      0xF4

class BMP085 {
    public:
        BMP085();
        BMP085(uint8_t address);

        void initialize();
        bool testConnection();

#ifdef BMP085_INCLUDE_INDIVIDUAL_CALIBRATION_ACCESS
        /* calibration register methods */
        int16_t     getAC1();
        int16_t     getAC2();
        int16_t     getAC3();
        uint16_t    getAC4();
        uint16_t    getAC5();
        uint16_t    getAC6();
        int16_t     getB1();
        int16_t     getB2();
        int16_t     getMB();
        int16_t     getMC();
        int16_t     getMD();
#endif

        /* CONTROL register methods */
        uint8_t     getControl();
        void        setControl(uint8_t value);

        /* MEASURE register methods */
        uint16_t    getMeasurement2(); // 16-bit data
        uint32_t    getMeasurement3(); // 24-bit data
        uint8_t     getMeasureDelayMilliseconds(uint8_t mode=0);
        uint16_t    getMeasureDelayMicroseconds(uint8_t mode=0);

        // convenience methods
        void        loadCalibration();
        uint16_t    getRawTemperature();
        float       getTemperatureC();
        float       getTemperatureF();
        uint32_t    getRawPressure();
        float       getPressure();
        float       getAltitude(float pressure, float seaLevelPressure=101325);

   private:
        uint8_t devAddr;
        uint8_t buffer[3];

        bool calibrationLoaded;
        int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
        uint16_t ac4, ac5, ac6;
        int32_t b5;
        uint8_t measureMode;
};

#endif /* _BMP085_H_ */
