// I2Cdev library collection - MPU6050 I2C device class, 6-axis MotionApps 2.0 implementation
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 5/20/2013 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     ... - ongoing debug release

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

#ifndef _MPU6050_6AXIS_MOTIONAPPS20_H_
#define _MPU6050_6AXIS_MOTIONAPPS20_H_

#include "I2Cdev.h"
#include "helper_3dmath.h"

// MotionApps 2.0 DMP implementation, built using the MPU-6050EVB evaluation board
#define MPU6050_INCLUDE_DMP_MOTIONAPPS20

#include "MPU6050.h"

// Tom Carpenter's conditional PROGMEM code
// http://forum.arduino.cc/index.php?topic=129407.0
#ifndef __arm__
    #include <avr/pgmspace.h>
#else
    // Teensy 3.0 library conditional PROGMEM code from Paul Stoffregen
    #ifndef __PGMSPACE_H_
        #define __PGMSPACE_H_ 1
        #include <inttypes.h>

        #define PROGMEM
        #define PGM_P  const char *
        #define PSTR(str) (str)
        #define F(x) x

        typedef void prog_void;
        typedef char prog_char;
        typedef unsigned char prog_uchar;
        typedef int8_t prog_int8_t;
        typedef uint8_t prog_uint8_t;
        typedef int16_t prog_int16_t;
        typedef uint16_t prog_uint16_t;
        typedef int32_t prog_int32_t;
        typedef uint32_t prog_uint32_t;
        
        #define strcpy_P(dest, src) strcpy((dest), (src))
        #define strcat_P(dest, src) strcat((dest), (src))
        #define strcmp_P(a, b) strcmp((a), (b))
        
        #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
        #define pgm_read_word(addr) (*(const unsigned short *)(addr))
        #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
        #define pgm_read_float(addr) (*(const float *)(addr))
        
        #define pgm_read_byte_near(addr) pgm_read_byte(addr)
        #define pgm_read_word_near(addr) pgm_read_word(addr)
        #define pgm_read_dword_near(addr) pgm_read_dword(addr)
        #define pgm_read_float_near(addr) pgm_read_float(addr)
        #define pgm_read_byte_far(addr) pgm_read_byte(addr)
        #define pgm_read_word_far(addr) pgm_read_word(addr)
        #define pgm_read_dword_far(addr) pgm_read_dword(addr)
        #define pgm_read_float_far(addr) pgm_read_float(addr)
    #endif
#endif

/* Source is from the InvenSense MotionApps v2 demo code. Original source is
 * unavailable, unless you happen to be amazing as decompiling binary by
 * hand (in which case, please contact me, and I'm totally serious).
 *
 * Also, I'd like to offer many, many thanks to Noah Zerkin for all of the
 * DMP reverse-engineering he did to help make this bit of wizardry
 * possible.
 */

// NOTE! Enabling DEBUG adds about 3.3kB to the flash program size.
// Debug output is now working even on ATMega328P MCUs (e.g. Arduino Uno)
// after moving string constants to flash memory storage using the F()
// compiler macro (Arduino IDE 1.0+ required).

//#define DEBUG
#ifdef DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTF(x, y) Serial.print(x, y)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTLNF(x, y) Serial.println(x, y)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTF(x, y)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTLNF(x, y)
#endif

#define MPU6050_DMP_CODE_SIZE       1929    // dmpMemory[]
#define MPU6050_DMP_CONFIG_SIZE     192     // dmpConfig[]
#define MPU6050_DMP_UPDATES_SIZE    47      // dmpUpdates[]

class MPU6050_6Axis_MotionApps20 : public MPU6050_Base {
public:
    uint8_t dmpInitialize();
    bool dmpPacketAvailable();
    uint8_t dmpGetAccel(int32_t *data, const uint8_t* packet);
    uint8_t dmpGetAccel(int16_t *data, const uint8_t* packet);
    uint8_t dmpGetAccel(VectorInt16 *v, const uint8_t* packet);
    uint8_t dmpGetQuaternion(int32_t *data, const uint8_t* packet);
    uint8_t dmpGetQuaternion(int16_t *data, const uint8_t* packet);
    uint8_t dmpGetQuaternion(Quaternion *q, const uint8_t* packet);
    uint8_t dmpGetGyro(int32_t *data, const uint8_t* packet);
    uint8_t dmpGetGyro(int16_t *data, const uint8_t* packet);
    uint8_t dmpGetGyro(VectorInt16 *v, const uint8_t* packet);
    uint8_t dmpGetLinearAccel(VectorInt16 *v, VectorInt16 *vRaw, VectorFloat *gravity);
    uint8_t dmpGetLinearAccelInWorld(VectorInt16 *v, VectorInt16 *vReal, Quaternion *q);
    uint8_t dmpGetGravity(VectorFloat *v, Quaternion *q);
    uint8_t dmpGetEuler(float *data, Quaternion *q);
    uint8_t dmpGetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity);
    uint8_t dmpProcessFIFOPacket(const unsigned char *dmpData);
    uint8_t dmpReadAndProcessFIFOPacket(uint8_t numPackets, uint8_t *processed);
    uint16_t dmpGetFIFOPacketSize();
};

typedef MPU6050_6Axis_MotionApps20 MPU6050_6Axis;

#ifndef HAVE_MPU6050_TYPEDEF
#define HAVE_MPU6050_TYPEDEF
typedef MPU6050_6Axis_MotionApps20 MPU6050;
#endif

#endif /* _MPU6050_6AXIS_MOTIONAPPS20_H_ */
