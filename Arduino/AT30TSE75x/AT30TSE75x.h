// I2Cdev library collection - AT30TSE752/754/758 I2C device class header file
// Based on ATMEL AT30TSE75 datasheet, 2013 Rev. Atmel-8751F-DTS-AT30TSE752-754-758-Datasheet_092013
// 2014-02-16 by Bartosz Kryza <bkryza@gmail.com>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2014-02-16 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2014 Bartosz Kryza, Jeff Rowberg

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

#ifndef _AT30TSE75x_H_
#define _AT30TSE75x_H_

#include "I2Cdev.h"


/** @defgroup devices Defines to distinguish between different AT30TSE752/754/758 variants
 *  @{
 */
#define  AT30TSE75x_752 1    /*!< ID of x752 variant (2Kb EEPROM) */
#define  AT30TSE75x_754 2    /*!< ID of x754 variant (4Kb EEPROM) */
#define  AT30TSE75x_758 3    /*!< ID of x758 variant (8Kb EEPROM) */
/** @} */

/** @defgroup address Device addresses
 * AT30TSE75 has 3 distinct subdevices with different I2C addresses:
 * AT30TSE75x consist of 3 devices:
 * - Temperature sensor
 * - Serial EEPROM
 * - Software Write Protection
 * each with a separate I2C address. The 3 least significant bits
 * of the 7-bit address are specified by wiring pins A2,A1,A0 to GND or VCC.
 * The 4 most significant bits are predefined for each subdevice as below: 
 *  @{
 */
#define AT30TSE75x_ADDRESS_TEMP_SENSOR   (0x9<<3) /*!< The 4 MSB of the 7-bit Temperature Sensor subdevice address */
#define AT30TSE75x_ADDRESS_SERIAL_EEPROM (0xA<<3) /*!< The 4 MSB of the 7-bit Serial EEPROM subdevice address */
#define AT30TSE75x_ADDRESS_WRITE_PROTECT (0x6<<3) /*!< The 4 MSB of the 7-bit Software Write Protect subdevice address */

#define AT30TSE75x_DEFAULT_ADDRESS       0x00 /*!< Assumes that the A2-A0 pins are grounded. */
/** @} */


/** @defgroup register Register addresses
 *  @{
 */
#define AT30TSE75x_RA_TEMPERATURE    0x00     /*!< Address of Temperature register */
#define AT30TSE75x_RA_CONFIGURATION  0x01     /*!< Address of Configuration register */
#define AT30TSE75x_RA_TLOW_LIMIT     0x02     /*!< Address of alert temperature low limit */
#define AT30TSE75x_RA_THIGH_LIMIT    0x03     /*!< Address of alert temperature high limit */
#define AT30TSE75x_RA_NV_CONFIGURATION 0x11   /*!< Address of Non-volatile Configuration register */
#define AT30TSE75x_RA_NV_TLOW_LIMIT  0x12     /*!< Address of Non-volatile alert temperature low limit */
#define AT30TSE75x_RA_NV_THIGH_LIMIT 0x13     /*!< Address of Non-volatile alert temperature high limit */
/** @} */


/** @defgroup cfgstruct Configuration Register Structure
 *  @{
 */
#define AT30TSE75x_CONFIG_NVRBSY_BIT     0   /*!< Configuration register Non-volatile Register Busy (NVRBSY) bit offset */

#define AT30TSE75x_CONFIG_RFU_BIT        1   /*!< Configuration register reserved bits segment offset (7 bits) */
#define AT30TSE75x_CONFIG_RFU_LENGTH     7

#define AT30TSE75x_CONFIG_SD_BIT         8   /*!< Configuration register Shutdown mode (SD) bit offset */

#define AT30TSE75x_CONFIG_CMPINT_BIT     9   /*!< Configuration register ~Comparator/Interrupt mode bit offset */

#define AT30TSE75x_CONFIG_POL_BIT        10  /*!< Configuration register Alert polarity (POL) bit offset */

#define AT30TSE75x_CONFIG_FT_BIT         11  /*!< Configuration register alert fault count (FT) value (2 bits) offset */
#define AT30TSE75x_CONFIG_FT_LENGTH      2

#define AT30TSE75x_CONFIG_RES_BIT        13  /*!< Configuration register conversion resolution (RES) value (2 bits) offset */
#define AT30TSE75x_CONFIG_RES_LENGTH     2

#define AT30TSE75x_CONFIG_OS_BIT         15  /*!< Configuration register one shot mode bit offset */
/** @} */



/** @defgroup nvcfgstruct Non-volatile Configuration Register Structure
 *  @{
 */
#define AT30TSE75x_NVCONFIG_RFU_BIT       0    /*!< Non-volatile configuration register reserved bit offset */

#define AT30TSE75x_NVCONFIG_RLCK_BIT      1    /*!< Non-volatile configuration register register lock (RLCK) it offset */

#define AT30TSE75x_NVCONFIG_RLCKDWN_BIT   2    /*!< Non-volatile configuration register register lockdown (RLCKDWN) bit offset */

#define AT30TSE75x_NVCONFIG_RFU2_BIT      3    /*!< Non-volatile configuration register reserved bit segment offset (4 bits) */
#define AT30TSE75x_NVCONFIG_RFU2_LENGTH   4

#define AT30TSE75x_NVCONFIG_NVSD_BIT      8    /*!< Non-volatile configuration register shutdown (NVSD) bit offset */

#define AT30TSE75x_NVCONFIG_NVCMPINT_BIT  9    /*!< Non-volatile configuration register ~comparator/interrupt (NVCMPINT) bit offset */

#define AT30TSE75x_NVCONFIG_NVPOL_BIT     10   /*!< Non-volatile configuration register alert polarity (NVPOL) bit offset */

#define AT30TSE75x_NVCONFIG_NVFT_BIT      11   /*!< Non-volatile configuration register alert fault count (NVFT) value offset (2 bits) */
#define AT30TSE75x_NVCONFIG_NVFT_LENGTH   2

#define AT30TSE75x_NVCONFIG_NVR_BIT       13   /*!< Non-volatile configuration register resolution (NVR) value offset (2 bits) */
#define AT30TSE75x_NVCONFIG_NVR_LENGTH    2
/** @} */



/** @defgroup resolution Conversion resolution parameters
 *  Conversion resolution (9, 10, 11, or 12-bits) impacts both the resolution 
 *  step (from 0.0625C to 0.5C) and conversion time (25ms, 50ms, 100ms, 200ms).
 *  @{
 */
#define AT30TSE75x_RES_9BIT          0x00      /*!< 9-bit resolution configuration register value */
#define AT30TSE75x_RES_10BIT         0x01      /*!< 10-bit resolution configuration register value */
#define AT30TSE75x_RES_11BIT         0x02      /*!< 11-bit resolution configuration register value */
#define AT30TSE75x_RES_12BIT         0x03      /*!< 12-bit resolution configuration register value */

#define AT30TSE75x_STEP_9BIT         0.5000    /*!< 9-bit resolution step */
#define AT30TSE75x_STEP_10BIT        0.2500    /*!< 10-bit resolution step */ 
#define AT30TSE75x_STEP_11BIT        0.1250    /*!< 11-bit resolution step */
#define AT30TSE75x_STEP_12BIT        0.0625    /*!< 12-bit resolution step */

#define AT30TSE75x_CONVTIME_MS_9BIT     25     /*!< 9-bit resolution conversion time (ms) */
#define AT30TSE75x_CONVTIME_MS_10BIT    50     /*!< 10-bit resolution conversion time (ms) */
#define AT30TSE75x_CONVTIME_MS_11BIT    100    /*!< 11-bit resolution conversion time (ms) */
#define AT30TSE75x_CONVTIME_MS_12BIT    200    /*!< 12-bit resolution conversion time (ms) */
/** @} */

/** @defgroup resolution Conversion resolution parameters
 *  Conversion resolution (9, 10, 11, or 12-bits) impacts both the resolution 
 *  step (from 0.0625C to 0.5C) and conversion time (25ms, 50ms, 100ms, 200ms).
 *  @{
 */
#define AT30TSE75x_FAULT_COUNT_1    0x00       /*!< Alert fault count value - 1 fault */
#define AT30TSE75x_FAULT_COUNT_2    0x01       /*!< Alert fault count value - 2 consecutive faults */
#define AT30TSE75x_FAULT_COUNT_4    0x02       /*!< Alert fault count value - 4 consecutive faults */
#define AT30TSE75x_FAULT_COUNT_6    0x03       /*!< Alert fault count value - 6 consecutive faults */
/** @} */



class AT30TSE75x {
    public:

        AT30TSE75x();

        AT30TSE75x(uint8_t address, uint8_t deviceType);

        void initialize();
        bool testConnection();


        float getTemperatureCelcius();
        float getTemperatureFahrenheit();
        uint16_t getTemperatureRaw();

        
        // CONFIGURATION register 
        uint16_t getConfiguration();

        bool getNVRBusy();

        bool getShutdownMode();
        void setShutdownMode(bool shutdownMode);

        bool getComparatorInterruptMode();
        void setComparatorInterruptMode(bool comparatorOrInterrup);

        bool getAlertPolarity();
        void setAlertPolarity(bool alertPolarity);

        uint8_t getFaultToleranceQueue();
        void setFaultToleranceQueue(uint8_t faultCount);

        uint8_t getConversionResolution();
        void setConversionResolution(uint8_t conversionResolution);

        bool getOneShotMode();
        void setOneShotMode(bool oneShotMode);
        
        
        // Temperature alert low limit register
        void setTempLowLimit(uint16_t lowLimit);
        uint16_t getTempLowLimit();
                
                
        // Temperature alert high limit register
        void setTempHighLimit(uint16_t highLimit);
        uint16_t getTempHighLimit();
        
        
        // Non-volatile Configuration register
        uint16_t getNVConfiguration();
                
        bool getNVComparatorInterruptMode();
        void setNVComparatorInterruptMode(bool comparatorOrInterrup);

        bool getNVRegisterLock();
        void setNVRegisterLock(bool registerLock);
        
        bool getNVRegisterLockdown();
        void setNVRegisterLockdown(bool registerLockdown);
        
        bool getNVShutdownMode();
        void setNVShutdownMode(bool shutdownMode);
        
        bool getNVAlertPolarity();
        void setNVAlertPolarity(bool alertPolarity);
         
        uint8_t getNVFaultToleranceQueue();
        void setNVFaultToleranceQueue(uint8_t faultCount);
        
        uint8_t geNVConversionResolution();
        void setNVConversionResolution(uint8_t conversionResolution);
 
 
        // Temperature alert low limit non-volatile register
        void setNVTempLowLimit(uint16_t lowLimit);
        uint16_t getNVTempLowLimit();
        
        
        // Temperature alert high limit non-volatile register
        void setNVTempHighLimit(uint16_t highLimit);
        uint16_t getNVTempHighLimit();


        // EEPROM
        uint8_t readEEPROMByte(uint16_t address);
        void writeEEPROMByte(uint16_t address, uint8_t value);
        
        void readEEPROMPage(uint16_t address, uint8_t* page);
        void writeEEPROMPage(uint16_t address, uint8_t* page);
  
        
        // Software write protect
        void setReversibleSoftwareWriteProtect();
        void clearReversibleSoftwareWriteProtect();

        // Reset Non-volatile config to default values
        void reset();


    private:
        uint8_t devAddr;
        uint8_t devType;
        uint16_t buffer[6];
};

#endif /* _AT30TSE75x_H_ */
