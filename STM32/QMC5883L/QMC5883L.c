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
#include <QMC5883L.h>
#include <I2Cdev.h>

static const uint8_t chip_addr = QMC5883L_DEFAULT_ADDR;

bool QMC5883L_soft_reset() {
  if (I2Cdev_writeByte(chip_addr, QMC5883L_REG_CTRL_2, 0x80) == 0) return false;
  return true;
}

bool QMC5883L_fbr_set(uint8_t fbr) {
  if (I2Cdev_writeByte(chip_addr, QMC5883L_REG_SR_PERIOD, fbr) == 0) return false;
  return true;
}

bool QMC5883L_control_1_set(uint8_t value) {
  if (I2Cdev_writeByte(chip_addr, QMC5883L_REG_CTRL_1, value) == 0) return false;
  return true;
}

bool QMC5883L_statusGet(uint8_t *data) {
  if (I2Cdev_readByte(chip_addr, QMC5883L_REG_STATUS, data) == 0) return false;
  return true;
}

bool QMC5883L_magGet(int16_t *v_mag) {
  uint8_t bytes[6];
  if (I2Cdev_readBytes(chip_addr, QMC5883L_REG_DATA_X_LSB, 6, bytes) == 0) return false;

  v_mag[0] = (((uint16_t)bytes[1]) << 8) | bytes[0];
  v_mag[1] = (((uint16_t)bytes[3]) << 8) | bytes[2];
  v_mag[2] = (((uint16_t)bytes[5]) << 8) | bytes[4];

  return true;
}

bool QMC5883L_tempGet(int16_t *temp) {
  uint8_t bytes[2];
  if (I2Cdev_readBytes(chip_addr, QMC5883L_REG_TEMP_LSB, 2, bytes) == 0) return false;

  *temp  = (((uint16_t)bytes[1]) << 8) | bytes[0];

  return true;
}
