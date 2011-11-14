// I2Cdev library collection - MYDEVSTUB I2C device class header file
// Based on [Manufacturer Name] MYDEVSTUB datasheet, [datasheet date]
// [current release date] by [Author Name] <[Author Email]>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     [YYYY-mm-dd] - updated some broken thing
//     [YYYY-mm-dd] - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 [Author Name], Jeff Rowberg

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

#ifndef _MYDEVSTUB_H_
#define _MYDEVSTUB_H_

#include "I2Cdev.h"

// ============================================================================
// DEVICE LIBRARY CONVENTIONS:
//
// 1. The class name should be the same as the device model if at all possible.
//    No spaces or hyphens, and ideally no underlines unless they're absolutely
//    necessary for clarity. ALL CAPS for model numbers, or FirstInitial caps
//    for full names. For example:
//      - ADXL345
//      - MPU6050
//      - TCA6424A
//      - PanelPilot
//
// 2. All #defines should use a device-specific prefix that is the same as the
//    all-caps version of the class name ("MYDEVSTUB_" in this example).
//
// 3. All #defines should be ALL CAPS, no matter what. This makes them clearly
//    distinctive from variables, classes, and functions.
//
// 4. Class methods and member variables should be named using camelCaps.
//
// 5. Every device class should contain an "initialize()" method which takes
//    no parameters and resets any important settings back to a known state,
//    ideally the defaults outlined in the datasheet. Some devices have a
//    RESET command available, which may be suitable. Some devices may not
//    require any specific initialization, but an empty method should be
//    created for consistency anyway.
//
// 6. Every device class should contain a "testConnection()" method which
//    returns TRUE if the device appears to be connected, or FALSE otherwise.
//    If a known ID register or device code is available, check for that. Since
//    such an ID register is not always available, at least check to make sure
//    that an I2C read may be performed on a specific register and that data
//    does actually come back (the I2Cdev class returns a "bytes read" value
//    for all read operations).
//
// 7. All class methods should be documented with useful information in Doxygen
//    format. You can take the info right out of the datasheet if you want to,
//    since that's likely to be helpful. Writing your own info is fine as well.
//    The goal is to have enough clear documentation right in the code that
//    someone can determine how the device works by studying the code alone.
//
// ============================================================================

// ----------------------------------------------------------------------------
// STUB TODO:
// List all possible device I2C addresses here, if they are predefined. Some
// devices only have one possible address, in which case that one should be
// defined as both [MYDEVSTUB]_ADDRESS and [MYDEVSTUB]_DEFAULT_ADDRESS for
// consistency. The *_DEFAULT address will be used if a device object is
// created without an address provided in the constructor. Note that I2C
// devices conventionally use 7-bit addresses, so these will generally be
// between 0x00 and 0x7F.
// ----------------------------------------------------------------------------
#define MYDEVSTUB_ADDRESS_AD0_LOW   0x20
#define MYDEVSTUB_ADDRESS_AD0_HIGH  0x21
#define MYDEVSTUB_DEFAULT_ADDRESS   0x20

// ----------------------------------------------------------------------------
// STUB TODO:
// List all registers that this device has. The goal for all device libraries
// is to have complete coverage of all possible functions, so be sure to add
// everything in the datasheet. Register address constants should use the extra
// prefix "RA_", followed by the datasheet's given name for each register.
// ----------------------------------------------------------------------------
#define MYDEVSTUB_RA_MEASURE1       0x00
#define MYDEVSTUB_RA_MEASURE2       0x01
#define MYDEVSTUB_RA_MEASURE3       0x02
#define MYDEVSTUB_RA_CONFIG1        0x03
#define MYDEVSTUB_RA_CONFIG2        0x04
#define MYDEVSTUB_RA_DATA_H         0x05
#define MYDEVSTUB_RA_DATA_L         0x06
#define MYDEVSTUB_RA_WHO_AM_I       0x07

// ----------------------------------------------------------------------------
// STUB TODO:
// List register structures wherever necessary. Bit position constants should
// end in "_BIT", and are defined from 7 to 0, with 7 being the left/MSB and
// 0 being the right/LSB. If the device uses 16-bit registers instead of the
// more common 8-bit registers, the range is 15 to 0. Field length constants
// should end in "_LENGTH", but otherwise match the name of bit position
// constant that it complements. Fields that are a single bit in length don't
// need a separate field length constant.
// ----------------------------------------------------------------------------
#define MYDEVSTUB_MEASURE1_RATE_BIT     4
#define MYDEVSTUB_MEASURE1_RATE_LENGTH  3

#define MYDEVSTUB_CONFIG1_FIFO_BIT      1
#define MYDEVSTUB_CONFIG1_RESET_BIT     0

// ----------------------------------------------------------------------------
// STUB TODO:
// List any special predefined values for each register according to the
// datasheet. For example, MEMS devices often provide different options for
// measurement rates, say 25Hz, 50Hz, 100Hz, and 200Hz. These are typically
// represented by arbitrary bit values, say 0b00, 0b01, 0b10, and 0b11 (or 0x0,
// 0x1, 0x2, and 0x3). Defining them here makes it easy to know which options
// are available.
// ----------------------------------------------------------------------------
#define MYDEVSTUB_RATE_10           0x00
#define MYDEVSTUB_RATE_20           0x01
#define MYDEVSTUB_RATE_40           0x02
#define MYDEVSTUB_RATE_80           0x03
#define MYDEVSTUB_RATE_160          0x04

class MYDEVSTUB {
    public:
        MYDEVSTUB();
        MYDEVSTUB(uint8_t address);

        void initialize();
        bool testConnection();

// ----------------------------------------------------------------------------
// STUB TODO:
// Declare methods to fully cover all available functionality provided by the
// device, according to the datasheet and/or register map. Unless there is very
// clear reason not to, try to follow the get/set naming convention for all
// values, for instance:
//   - uint8_t getThreshold()
//   - void setThreshold(uint8_t threshold)
//   - uint8_t getRate()
//   - void setRate(uint8_t rate)
//
// Some methods may be named differently if it makes sense. As long as all
// functionality is covered, that's the important part. The methods here are
// only examples and should not be kept for your real device.
// ----------------------------------------------------------------------------

        // MEASURE1 register, read-only
        uint8_t getMeasurement1();

        // MEASURE2 register, read-only
        uint8_t getMeasurement2();

        // MEASURE3 register, read-only
        uint8_t getMeasurement3();

        // CONFIG1 register, r/w
        void reset();               // <-- special method that resets entire device
        bool getFIFOEnabled();
        void setFIFOEnabled(bool enabled);

        // CONFIG2 register, r/w
        uint8_t getInterruptMode();
        void setInterruptMode(uint8_t mode);
        uint8_t getRate();
        void setRate(uint8_t rate);

        // DATA_* registers, r/w
        uint16_t getData();
        void setData(uint16_t value);

        // WHO_AM_I register, read-only
        uint8_t getDeviceID();

// ----------------------------------------------------------------------------
// STUB TODO:
// Declare private object helper variables or local storage for particular
// device settings, if required. All devices need a "devAddr" variable to store
// the I2C slave address for easy access. Most devices also need a buffer for
// reads (the I2Cdev class' read methods use a pointer for the storage location
// of any read data). The buffer should be of type "uint8_t" if the device uses
// 8-bit registers, or type "uint16_t" if the device uses 16-bit registers.
// Many devices will not require more member variables than this.
// ----------------------------------------------------------------------------
    private:
        uint8_t devAddr;
        uint8_t buffer[6];
};

#endif /* _MYDEVSTUB_H_ */
