// I2Cdev library collection - MPR121 I2C device class header file
// Based on Freescale MPR121 datasheet rev. 2, 04/2010 and Freescale App Note 3944, rev 1 3/26/2010
// 9/3/2011 by Andrew Schamp <schamp@gmail.com>
//
// This I2C device library is using (and submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-09-03 - add callback support
//     2011-08-20 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Andrew Schamp

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

#include "MPR121.h"
#include "I2Cdev.h"

MPR121::MPR121(uint8_t address) :
  m_devAddr(address)
{
  for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    m_callbackMap[ch][TOUCHED] = 0;
    m_callbackMap[ch][RELEASED] = 0;
  }
}

void MPR121::initialize()
{
  // These are the configuration values recommended by app note AN3944
  // along with the description in the app note.

  // Section A
  // Description:
  //   This group of settings controls the filtering of the system
  //   when the data is greater than the baseline.  The setting used 
  //   allows the filter to act quickly and adjust for environmental changes. 
  //   Additionally, if calibration happens to take place while a touch occurs,
  //   the value will self adjust very quickly.  This auto-recovery or snap back
  //   provides repeated false negative for a touch detection. 
  // Variation:
  //   As the filter is sensitive to setting changes, it is recommended
  //   that users read AN3891 before changing the values. 
  //   In most cases these default values will work.
  I2Cdev::writeByte(m_devAddr, MHD_RISING,        0x01);
  I2Cdev::writeByte(m_devAddr, NHD_AMOUNT_RISING, 0x01);
  I2Cdev::writeByte(m_devAddr, NCL_RISING,        0x00);
  I2Cdev::writeByte(m_devAddr, FDL_RISING,        0x00);

  // Section B
  // Description:
  //   This group of settings controls the filtering of the system 
  //   when the data is less than the baseline.  The settings slow down 
  //   the filter as the negative charge is in the same direction 
  //   as a touch.  By slowing down the filter, touch signals 
  //   are "rejected" by the baseline filter.  While at the same time
  //   long term environmental changes that occur slower than
  //   at a touch are accepted.  This low pass filter both allows 
  //   for touches to be detected properly while preventing false 
  //   positives by passing environmental changes through the filter.
  // Variation:
  //   As the filter is sensitive to setting changes, it is recommended 
  //   that users read AN3891 before changing the values.  
  //   In most cases these default values will work.
  I2Cdev::writeByte(m_devAddr, MHD_FALLING,        0x01);
  I2Cdev::writeByte(m_devAddr, NHD_AMOUNT_FALLING, 0x01);
  I2Cdev::writeByte(m_devAddr, NCL_FALLING,        0xFF);
  I2Cdev::writeByte(m_devAddr, FDL_FALLING,        0x02);

  // Section C
  // Description:
  //   The touch threshold registers set the minimum delta from the baseline
  //   when a touch is detected.  The value 0x0F (or 15 in decimal) is
  //   an estimate of the minimum value for touch.  Most electrodes will 
  //   work with this value even if they vary greatly in size and shape.  
  //   The value of 0x0A or 10 in the release threshold register allowed 
  //   for hysteresis in the touch detection.
  // Variation: 
  //   For very small electrodes, smaller values can be used and for 
  //   very large electrodes the reverse is true.  One easy method is 
  //   to view the deltas actually seen in a system and set the touch 
  //   at 80% and release at 70% of delta for good performance.
  I2Cdev::writeByte(m_devAddr, ELE0_TOUCH_THRESHOLD,   TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE0_RELEASE_THRESHOLD, RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE1_TOUCH_THRESHOLD,   TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE1_RELEASE_THRESHOLD, RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE2_TOUCH_THRESHOLD,   TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE2_RELEASE_THRESHOLD, RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE3_TOUCH_THRESHOLD,   TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE3_RELEASE_THRESHOLD, RELEASE_THRESHOLD);

  // TODO: enable setting these channels to capsense or GPIO
  // for now they are all capsense
  I2Cdev::writeByte(m_devAddr, ELE4_TOUCH_THRESHOLD,    TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE4_RELEASE_THRESHOLD,  RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE5_TOUCH_THRESHOLD,    TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE5_RELEASE_THRESHOLD,  RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE6_TOUCH_THRESHOLD,    TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE6_RELEASE_THRESHOLD,  RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE7_TOUCH_THRESHOLD,    TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE7_RELEASE_THRESHOLD,  RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE8_TOUCH_THRESHOLD,    TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE8_RELEASE_THRESHOLD,  RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE9_TOUCH_THRESHOLD,    TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE9_RELEASE_THRESHOLD,  RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE10_TOUCH_THRESHOLD,   TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE10_RELEASE_THRESHOLD, RELEASE_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE11_TOUCH_THRESHOLD,   TOUCH_THRESHOLD);
  I2Cdev::writeByte(m_devAddr, ELE11_RELEASE_THRESHOLD, RELEASE_THRESHOLD);

  // Section D
  // Description:
  //   There are three settings embedded in this register so it is 
  //   only necessary to pay attention to one.  The ESI controls 
  //   the sample rate of the device.  In the default, the setting
  //   used is 0x00 for 1ms sample rate.  Since the SFI is set to 00,
  //   resulting in 4 samples averaged, the response time will be 4 ms.
  // Variation:
  //   To save power, the 1 ms can be increased to 128 ms by increasing
  //   the setting to 0x07.  The values are base 2 exponential, thus 
  //   0x01 = 2ms, 0x02 = 4 ms; and so on to 0x07 = 128 ms.  Most of 
  //   the time, 0x04 results in the best compromise between power 
  //   consumption and response time.
  I2Cdev::writeByte(m_devAddr, FILTER_CONFIG, 0x04);

  // Section E
  // Description:
  //   This register controls the number of electrodes being enabled
  //   and the mode the device is in.  There are only two modes, 
  //   Standby (when the value is 0x00) and Run (when the value of 
  //   the lower bit is non-zero).  The default value shown enables 
  //   all 12 electrodes by writing decimal 12 or hex 0x0C to the register. 
  //   Typically other registers cannot be changed while the part is running, 
  //   so this register should always be written last.
  // Variation:
  //   During debug of a system, this register will change between 
  //   the number of electrodes and 0x00 every time a register needs 
  //   to change.  In a production system, this register will only need 
  //   to be written when the mode is changed from Standby to Run or vice versa.
  I2Cdev::writeByte(m_devAddr, ELECTRODE_CONFIG, 0x0C); 

  // Section F
  // Description:
  //   These are the settings used for the Auto Configuration.  They enable 
  //   AUTO-CONFIG and AUTO_RECONFIG.  In addition, they set the target
  //   rate for the baseline.  The upper limit is set to 190, the target 
  //   is set to 180 and the lower limit is set to 140.
  // Variation:
  //   In most cases these values will never need to be changed, but if
  //   a case arises, a full description is found in application note AN3889.
  //I2Cdev::writeByte(m_devAddr, AUTO_CONFIG_CONTROL_0,    0x0B);
  //I2Cdev::writeByte(m_devAddr, AUTO_CONFIG_USL,          0x9C);
  //I2Cdev::writeByte(m_devAddr, AUTO_CONFIG_LSL,          0x65);
  //I2Cdev::writeByte(m_devAddr, AUTO_CONFIG_TARGET_LEVEL, 0x8C);
          
}

// check to see if the filter configuration register contains 0x04,
// which is the default powerup value, and which we don't change in this library
bool MPR121::testConnection() {
  uint8_t buf = 0;
  I2Cdev::readByte(m_devAddr, FILTER_CONFIG, &buf);
  return buf == 0x04; // default value, we don't change
}

bool MPR121::getTouchStatus(uint8_t channel) {
  const uint8_t statusReg = (channel < 8) ? ELE0_ELE7_TOUCH_STATUS : ELE8_ELE11_ELEPROX_TOUCH_STATUS;
  uint8_t buf = 0;
  I2Cdev::readByte(m_devAddr, statusReg, &buf);
  const bool touchActive = (channel < 8) ? (buf & (1 << channel)) > 0 : (buf & (1 << (channel - 8))) > 0;
  return touchActive;
}

uint16_t MPR121::getTouchStatus() {
  uint16_t statusBuf;
  uint8_t buf;
  I2Cdev::readByte(m_devAddr, ELE0_ELE7_TOUCH_STATUS, &buf);
  statusBuf = buf;
  I2Cdev::readByte(m_devAddr, ELE8_ELE11_ELEPROX_TOUCH_STATUS, &buf);
  statusBuf += (buf << 8);
  return statusBuf;
}

void MPR121::setCallback(uint8_t channel, EventType event, CallbackPtrType callbackPtr) {
  m_callbackMap[channel][event] = callbackPtr;
}
    
void MPR121::serviceCallbacks() {
  for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) { 
    bool touchStatus = getTouchStatus(channel);
    if (touchStatus != m_prevTouchStatus[channel]) {
      for (uint8_t type = 0; type < NUM_EVENTS; type++) { 
        const CallbackPtrType cb = touchStatus ? m_callbackMap[channel][TOUCHED] : m_callbackMap[channel][RELEASED]; 
        if (cb != 0) {
          cb();           
        }
      }
      m_prevTouchStatus[channel] = touchStatus; 
    }
  }
}
