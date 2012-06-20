// I2Cdev library collection - MS5611-01BA01 I2C device class header file
// Based on MEAS MS5611-01BA01 datasheet, 07/2011 (DA5611-01BA01_006)
// 6/12/2012 by Manuel J Guisado <mjguisado@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-06-12 - Initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Manuel J Guisado

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

#ifndef _MS561101BA_H_
#define _MS561101BA_H_

#include "I2Cdev.h"

// The MS5611-01BA address is 111011Cx, where C
// is the complementary value of the pin CSB
#define MS561101BA_ADDRESS_CSB_LOW  B1110111
#define MS561101BA_ADDRESS_CSB_HIGH B1110110
#define MS561101BA_DEFAULT_ADDRESS  B1110110

// Register's address
#define MS561101BA_RA_D1     0x40
#define MS561101BA_RA_D2     0x50
#define MS561101BA_D1D2_SIZE 3 // (bytes)
#define MS561101BA_RA_RESET  0x1E

// PROM
#define MS561101BA_RA_PROM 0xA0
#define MS561101BA_PROM_NUM_REGISTERS 8
#define MS561101BA_PROM_BITS_PER_REGISTER 16

// Oversample rates
#define MS561101BA_OSR_256  0x00
#define MS561101BA_OSR_512  0x02
#define MS561101BA_OSR_1024 0x04
#define MS561101BA_OSR_2048 0x06
#define MS561101BA_OSR_4096 0x08

// ADC max conversion time (µs)
#define MS561101BA_MAX_CONVERSION_TIME_OSR_256   600
#define MS561101BA_MAX_CONVERSION_TIME_OSR_512  1170
#define MS561101BA_MAX_CONVERSION_TIME_OSR_1024 2280
#define MS561101BA_MAX_CONVERSION_TIME_OSR_2048 4540
#define MS561101BA_MAX_CONVERSION_TIME_OSR_4096 9040

class MS561101BA {

	public:

		MS561101BA();
		MS561101BA(uint8_t address);
		MS561101BA(uint8_t address, uint8_t osr);

		void initialize();
		bool testConnection();
		bool reset();
		bool readPROM();
		bool setOverSampleRate(uint8_t osr);

		bool readValues(
				float * pressure,
				float * temperature,
				int8_t osr = -1);

		int32_t readD1(int8_t osr = -1);
		int32_t readD2(int8_t osr = -1);

	private:

		// This constants are used to optimize the division operations.
		// It isn't safe to use the bitshift right operator with signed integers.
		static const uint8_t  POW_2_7  = 1 << 7;
		static const uint16_t POW_2_8  = 1 << 8;
		static const uint16_t POW_2_15 = 1 << 15;
		static const uint32_t POW_2_21 = 1L << 21;
		static const uint32_t POW_2_23 = 1L << 23;
		static const uint32_t POW_2_31 = 1L << 31;

		// Device's address
		uint8_t devAddr;

		// Default oversample rate
		uint8_t defaultOsr;

		int32_t readConversion(uint8_t regAddr, uint8_t osr);

        // Calibration parameters
		uint16_t prom[MS561101BA_PROM_NUM_REGISTERS];
		uint16_t getSENST1();
		uint16_t getOFFT1();
		uint16_t getTCS();
		uint16_t getTCO();
		uint16_t getTREF();
		uint16_t getTEMPSENS();


};

#endif /* _MS561101BA_H_ */
