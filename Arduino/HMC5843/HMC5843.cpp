// I2Cdev library collection - HMC5843 I2C device class
// Based on Honeywell HMC5843 datasheet, 6/2010 (Form #900367)
// 8/22/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-08-22 - initial release

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

#include "HMC5843.h"

/** Default constructor, uses default I2C address.
 * @see HMC5843_DEFAULT_ADDRESS
 */
HMC5843::HMC5843() {
    devAddr = HMC5843_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see HMC5843_DEFAULT_ADDRESS
 * @see HMC5843_ADDRESS
 */
HMC5843::HMC5843(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This will prepare the magnetometer with default settings, ready for single-
 * use mode (very low power requirements). Default settings include 8-sample
 * averaging, 15 Hz data output rate, normal measurement bias, a,d 1090 gain (in
 * terms of LSB/Gauss). Be sure to adjust any settings you need specifically
 * after initialization, especially the gain settings if you happen to be seeing
 * a lot of -4096 values (see the datasheet for mor information).
 */
void HMC5843::initialize() {
    // write CONFIG_A register
    I2Cdev::writeByte(devAddr, HMC5843_RA_CONFIG_A,
        (HMC5843_RATE_10     << (HMC5843_CRA_RATE_BIT - HMC5843_CRA_RATE_LENGTH + 1)) |
        (HMC5843_BIAS_NORMAL << (HMC5843_CRA_BIAS_BIT - HMC5843_CRA_BIAS_LENGTH + 1)));

    // write CONFIG_B register
    setGain(HMC5843_GAIN_1300);
    
    // write MODE register
    setMode(HMC5843_MODE_SINGLE);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool HMC5843::testConnection() {
    if (I2Cdev::readBytes(devAddr, HMC5843_RA_ID_A, 3, buffer) == 3) {
        return (buffer[0] == 'H' && buffer[1] == '4' && buffer[2] == '3');
    }
    return false;
}

// CONFIG_A register

/** Get data output rate value.
 * The Table below shows all selectable output rates in continuous measurement
 * mode. All three channels shall be measured within a given output rate.
 *
 * Value | Typical Data Output Rate (Hz)
 * ------+------------------------------
 * 0     | 0.5
 * 1     | 1
 * 2     | 2
 * 3     | 5
 * 4     | 10 (Default)
 * 5     | 20
 * 6     | 50
 * 7     | Not used
 *
 * @return Current rate of data output to registers
 * @see HMC5843_RATE_10
 * @see HMC5843_RA_CONFIG_A
 * @see HMC5843_CRA_RATE_BIT
 * @see HMC5843_CRA_RATE_LENGTH
 */
uint8_t HMC5843::getDataRate() {
    I2Cdev::readBits(devAddr, HMC5843_RA_CONFIG_A, HMC5843_CRA_RATE_BIT, HMC5843_CRA_RATE_LENGTH, buffer);
    return buffer[0];
}
/** Set data output rate value.
 * @param rate Rate of data output to registers
 * @see getDataRate()
 * @see HMC5843_RATE_10
 * @see HMC5843_RA_CONFIG_A
 * @see HMC5843_CRA_RATE_BIT
 * @see HMC5843_CRA_RATE_LENGTH
 */
void HMC5843::setDataRate(uint8_t rate) {
    I2Cdev::writeBits(devAddr, HMC5843_RA_CONFIG_A, HMC5843_CRA_RATE_BIT, HMC5843_CRA_RATE_LENGTH, rate);
}
/** Get measurement bias value.
 * @return Current bias value (0-2 for normal/positive/negative respectively)
 * @see HMC5843_BIAS_NORMAL
 * @see HMC5843_RA_CONFIG_A
 * @see HMC5843_CRA_BIAS_BIT
 * @see HMC5843_CRA_BIAS_LENGTH
 */
uint8_t HMC5843::getMeasurementBias() {
    I2Cdev::readBits(devAddr, HMC5843_RA_CONFIG_A, HMC5843_CRA_BIAS_BIT, HMC5843_CRA_BIAS_LENGTH, buffer);
    return buffer[0];
}
/** Set measurement bias value.
 * @param bias New bias value (0-2 for normal/positive/negative respectively)
 * @see HMC5843_BIAS_NORMAL
 * @see HMC5843_RA_CONFIG_A
 * @see HMC5843_CRA_BIAS_BIT
 * @see HMC5843_CRA_BIAS_LENGTH
 */
void HMC5843::setMeasurementBias(uint8_t bias) {
    I2Cdev::writeBits(devAddr, HMC5843_RA_CONFIG_A, HMC5843_CRA_BIAS_BIT, HMC5843_CRA_BIAS_LENGTH, bias);
}

// CONFIG_B register

/** Get magnetic field gain value.
 * The table below shows nominal gain settings. Use the Gain column to convert
 * counts to Gauss. Choose a lower gain value (higher GN#) when total field
 * strength causes overflow in one of the data output registers (saturation).
 * The data output range for all settings is 0xF800-0x07FF (-2048 - 2047).
 *
 * Value | Field Range | Gain (LSB/Gauss)
 * ------+-------------+-----------------
 * 0     | +/- 0.7 Ga  | 1620
 * 1     | +/- 1.0 Ga  | 1300 (Default)
 * 2     | +/- 1.5 Ga  | 970
 * 3     | +/- 2.0 Ga  | 780
 * 4     | +/- 3.2 Ga  | 530
 * 5     | +/- 3.8 Ga  | 460
 * 6     | +/- 4.5 Ga  | 390
 * 7     | +/- 6.5 Ga  | 280 (Not recommended)
 *
 * @return Current magnetic field gain value
 * @see HMC5843_GAIN_1300
 * @see HMC5843_RA_CONFIG_B
 * @see HMC5843_CRB_GAIN_BIT
 * @see HMC5843_CRB_GAIN_LENGTH
 */
uint8_t HMC5843::getGain() {
    I2Cdev::readBits(devAddr, HMC5843_RA_CONFIG_B, HMC5843_CRB_GAIN_BIT, HMC5843_CRB_GAIN_LENGTH, buffer);
    return buffer[0];
}
/** Set magnetic field gain value.
 * @param gain New magnetic field gain value
 * @see getGain()
 * @see HMC5843_RA_CONFIG_B
 * @see HMC5843_CRB_GAIN_BIT
 * @see HMC5843_CRB_GAIN_LENGTH
 */
void HMC5843::setGain(uint8_t gain) {
    // use this method to guarantee that bits 4-0 are set to zero, which is a
    // requirement specified in the datasheet; it's actually more efficient than
    // using the I2Cdev.writeBits method
    I2Cdev::writeByte(devAddr, HMC5843_RA_CONFIG_B, gain << (HMC5843_CRB_GAIN_BIT - HMC5843_CRB_GAIN_LENGTH + 1));
}

// MODE register

/** Get measurement mode.
 * In continuous-measurement mode, the device continuously performs measurements
 * and places the result in the data register. RDY goes high when new data is
 * placed in all three registers. After a power-on or a write to the mode or
 * configuration register, the first measurement set is available from all three
 * data output registers after a period of 2/fDO and subsequent measurements are
 * available at a frequency of fDO, where fDO is the frequency of data output.
 *
 * When single-measurement mode (default) is selected, device performs a single
 * measurement, sets RDY high and returned to idle mode. Mode register returns
 * to idle mode bit values. The measurement remains in the data output register
 * and RDY remains high until the data output register is read or another
 * measurement is performed.
 *
 * @return Current measurement mode
 * @see HMC5843_MODE_CONTINUOUS
 * @see HMC5843_MODE_SINGLE
 * @see HMC5843_MODE_IDLE
 * @see HMC5843_MODE_SLEEP
 * @see HMC5843_RA_MODE
 * @see HMC5843_MODEREG_BIT
 * @see HMC5843_MODEREG_LENGTH
 */
uint8_t HMC5843::getMode() {
    I2Cdev::readBits(devAddr, HMC5843_RA_MODE, HMC5843_MODEREG_BIT, HMC5843_MODEREG_LENGTH, buffer);
    return buffer[0];
}
/** Set measurement mode.
 * @param newMode New measurement mode
 * @see getMode()
 * @see HMC5843_MODE_CONTINUOUS
 * @see HMC5843_MODE_SINGLE
 * @see HMC5843_MODE_IDLE
 * @see HMC5843_MODE_SLEEP
 * @see HMC5843_RA_MODE
 * @see HMC5843_MODEREG_BIT
 * @see HMC5843_MODEREG_LENGTH
 */
void HMC5843::setMode(uint8_t newMode) {
    // use this method to guarantee that bits 7-2 are set to zero, which is a
    // requirement specified in the datasheet; it's actually more efficient than
    // using the I2Cdev.writeBits method
    I2Cdev::writeByte(devAddr, HMC5843_RA_MODE, newMode << (HMC5843_MODEREG_BIT - HMC5843_MODEREG_LENGTH + 1));
    mode = newMode; // track to tell if we have to clear bit 7 after a read
}

// DATA* registers

/** Get 3-axis heading measurements.
 * In the event the ADC reading overflows or underflows for the given channel,
 * or if there is a math overflow during the bias measurement, this data
 * register will contain the value -4096. This register value will clear when
 * after the next valid measurement is made. Note that this method automatically
 * clears the appropriate bit in the MODE register if Single mode is active.
 * @param x 16-bit signed integer container for X-axis heading
 * @param y 16-bit signed integer container for Y-axis heading
 * @param z 16-bit signed integer container for Z-axis heading
 * @see HMC5843_RA_DATAX_H
 */
void HMC5843::getHeading(int16_t *x, int16_t *y, int16_t *z) {
    I2Cdev::readBytes(devAddr, HMC5843_RA_DATAX_H, 6, buffer);
    if (mode == HMC5843_MODE_SINGLE) I2Cdev::writeByte(devAddr, HMC5843_RA_MODE, HMC5843_MODE_SINGLE << (HMC5843_MODEREG_BIT - HMC5843_MODEREG_LENGTH + 1));
    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[2]) << 8) | buffer[3];
    *z = (((int16_t)buffer[4]) << 8) | buffer[5];
}
/** Get X-axis heading measurement.
 * @return 16-bit signed integer with X-axis heading
 * @see HMC5843_RA_DATAX_H
 */
int16_t HMC5843::getHeadingX() {
    // each axis read requires that ALL axis registers be read, even if only
    // one is used; this was not done ineffiently in the code by accident
    I2Cdev::readBytes(devAddr, HMC5843_RA_DATAX_H, 6, buffer);
    if (mode == HMC5843_MODE_SINGLE) I2Cdev::writeByte(devAddr, HMC5843_RA_MODE, HMC5843_MODE_SINGLE << (HMC5843_MODEREG_BIT - HMC5843_MODEREG_LENGTH + 1));
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
/** Get Y-axis heading measurement.
 * @return 16-bit signed integer with Y-axis heading
 * @see HMC5843_RA_DATAY_H
 */
int16_t HMC5843::getHeadingY() {
    // each axis read requires that ALL axis registers be read, even if only
    // one is used; this was not done ineffiently in the code by accident
    I2Cdev::readBytes(devAddr, HMC5843_RA_DATAX_H, 6, buffer);
    if (mode == HMC5843_MODE_SINGLE) I2Cdev::writeByte(devAddr, HMC5843_RA_MODE, HMC5843_MODE_SINGLE << (HMC5843_MODEREG_BIT - HMC5843_MODEREG_LENGTH + 1));
    return (((int16_t)buffer[2]) << 8) | buffer[3];
}
/** Get Z-axis heading measurement.
 * @return 16-bit signed integer with Z-axis heading
 * @see HMC5843_RA_DATAZ_H
 */
int16_t HMC5843::getHeadingZ() {
    // each axis read requires that ALL axis registers be read, even if only
    // one is used; this was not done ineffiently in the code by accident
    I2Cdev::readBytes(devAddr, HMC5843_RA_DATAX_H, 6, buffer);
    if (mode == HMC5843_MODE_SINGLE) I2Cdev::writeByte(devAddr, HMC5843_RA_MODE, HMC5843_MODE_SINGLE << (HMC5843_MODEREG_BIT - HMC5843_MODEREG_LENGTH + 1));
    return (((int16_t)buffer[4]) << 8) | buffer[5];
}

// STATUS register

/** Get regulator enabled status.
 * This bit is set when the internal voltage regulator is enabled. This bit is
 * cleared when the internal regulator is disabled.
 * @return Regulator enabled status
 * @see HMC5843_RA_STATUS
 * @see HMC5843_STATUS_REN_BIT
 */
bool HMC5843::getRegulatorEnabledStatus() {
    I2Cdev::readBit(devAddr, HMC5843_RA_STATUS, HMC5843_STATUS_REN_BIT, buffer);
    return buffer[0];
}
/** Get data output register lock status.
 * This bit is set when this some but not all for of the six data output
 * registers have been read. When this bit is set, the six data output registers
 * are locked and any new data will not be placed in these register until one of
 * three conditions are met: one, all six bytes have been read or the mode
 * changed, two, the mode is changed, or three, the measurement configuration is
 * changed.
 * @return Data output register lock status
 * @see HMC5843_RA_STATUS
 * @see HMC5843_STATUS_LOCK_BIT
 */
bool HMC5843::getLockStatus() {
    I2Cdev::readBit(devAddr, HMC5843_RA_STATUS, HMC5843_STATUS_LOCK_BIT, buffer);
    return buffer[0];
}
/** Get data ready status.
 * This bit is set when data is written to all six data registers, and cleared
 * when the device initiates a write to the data output registers and after one
 * or more of the data output registers are written to. When RDY bit is clear it
 * shall remain cleared for 250 us. DRDY pin can be used as an alternative to
 * the status register for monitoring the device for measurement data.
 * @return Data ready status
 * @see HMC5843_RA_STATUS
 * @see HMC5843_STATUS_READY_BIT
 */
bool HMC5843::getReadyStatus() {
    I2Cdev::readBit(devAddr, HMC5843_RA_STATUS, HMC5843_STATUS_READY_BIT, buffer);
    return buffer[0];
}

// ID_* registers

/** Get identification byte A
 * @return ID_A byte (should be 01001000, ASCII value 'H')
 */
uint8_t HMC5843::getIDA() {
    I2Cdev::readByte(devAddr, HMC5843_RA_ID_A, buffer);
    return buffer[0];
}
/** Get identification byte B
 * @return ID_A byte (should be 00110100, ASCII value '4')
 */
uint8_t HMC5843::getIDB() {
    I2Cdev::readByte(devAddr, HMC5843_RA_ID_B, buffer);
    return buffer[0];
}
/** Get identification byte C
 * @return ID_A byte (should be 00110011, ASCII value '3')
 */
uint8_t HMC5843::getIDC() {
    I2Cdev::readByte(devAddr, HMC5843_RA_ID_C, buffer);
    return buffer[0];
}
