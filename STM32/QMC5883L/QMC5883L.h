// I2Cdev library collection - QMC5883L I2C device class header file
// Based on QST QMC5883L datasheet 1.0, 02/2016
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

#ifndef _QMC5883L_H_
#define _QMC5883L_H_

#include <stdint.h>
#include <stdbool.h>

#define QMC5883L_DEFAULT_ADDR 0x0D

/* register addresses */
#define QMC5883L_REG_DATA_X_LSB 0x00
#define QMC5883L_REG_DATA_X_MSB 0x01
#define QMC5883L_REG_DATA_Y_LSB 0x02
#define QMC5883L_REG_DATA_Y_MSB 0x03
#define QMC5883L_REG_DATA_Z_LSB 0x04
#define QMC5883L_REG_DATA_Z_MSB 0x05
#define QMC5883L_REG_STATUS     0x06
#define QMC5883L_REG_TEMP_LSB   0x07
#define QMC5883L_REG_TEMP_MSB   0x08
#define QMC5883L_REG_CTRL_1     0x09
#define QMC5883L_REG_CTRL_2     0x0A
#define QMC5883L_REG_SR_PERIOD  0x0B

/* values for control_1 register */
#define QMC5883L_OVERSAMPLE_512 0b00
#define QMC5883L_OVERSAMPLE_256 0b01
#define QMC5883L_OVERSAMPLE_128 0b10
#define QMC5883L_OVERSAMPLE_64  0b11

#define QMC5883L_SCALE_2G  0b00
#define QMC5883L_SCALE_8G  0b01
 
#define QMC5883L_OUTPUT_RATE_10HZ  0b00
#define QMC5883L_OUTPUT_RATE_50HZ  0b01
#define QMC5883L_OUTPUT_RATE_100HZ 0b10
#define QMC5883L_OUTPUT_RATE_200HZ 0b11
 
#define QMC5883L_MODE_STBY 0b00
#define QMC5883L_MODE_CONT 0b01

#define QMC5883L_CTRL1_VALUE(_mode_, _output_rate_, _scale_, _oversample_) \
  ((_mode_) | ((_output_rate_) << 2) | ((_scale_) << 4) | ((_oversample_) << 6))

/* QMC5883L_soft_reset: does a soft reset */
bool QMC5883L_soft_reset();

/* QMC5883L_fbr_set: SET/RESET Period FBR; recommended value: 1 */
bool QMC5883L_fbr_set(uint8_t fbr);

/* QMC5883L_control_1_set: set value for control register (mode, output rate, scale, oversampling) */
bool QMC5883L_control_1_set(uint8_t value);

bool QMC5883L_statusGet(uint8_t *data);

/* QMC5883L_magGet:
 * parameters:
 *   v_mag: int16_t array of length 3
 * returns:
 *   success: true
 *   failure: false
 */
bool QMC5883L_magGet(int16_t *v_mag);

bool QMC5883L_tempGet(int16_t *temp);

#endif
