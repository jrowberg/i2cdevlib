// I2Cdev library collection - AT30TSE75x I2C device class
// Based on ATMEL AT30TSE75x datasheet, 2013 Rev. Atmel-8751F-DTS-AT30TSE752-754-758-Datasheet_092013
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

#include "AT30TSE75x.h"

/** Default constructor, uses default I2C address. Assumes the device is 
 *  AT30TSE752 and A2-A0 pins are connected to ground.
 */
AT30TSE75x::AT30TSE75x() {
    devAddr = 0x00;
    devType = AT30TSE75x_752;
}

/** Specific address constructor.
 * @param address 3 least significat bits of the address should be provided here
 * depending on the A2-A0 connection
 * @deviceType The type of device (AT30TSE75x_752, AT30TSE75x_754, AT30TSE75x_758)
 */
AT30TSE75x::AT30TSE75x(uint8_t address, uint8_t deviceType) {
    // Here we only need the least significant 3 bits
    devAddr = 0x07 & address;
    devType = deviceType;
}

/** Power on and prepare for general usage.
 */
void AT30TSE75x::initialize() {
  // Nothing to do here
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool AT30TSE75x::testConnection() {
  
    if (I2Cdev::readWord(devAddr, AT30TSE75x_RA_TEMPERATURE, buffer) == 1) {
        return true;
    }
    return false;

}

/** Get the current reading of temperature in Celcius degrees
 * @return Current temperature measurement in Celcius degrees
 */
float AT30TSE75x::getTemperatureCelcius() {

    uint16_t rawTemperature = getTemperatureRaw();

    float celciusTemperature = 0.0;

    if((rawTemperature & 0x8000) == 0) {
      celciusTemperature = (rawTemperature>>8) + ((rawTemperature&0x00F0)>>4)*AT30TSE75x_STEP_12BIT;
    }
    else {
      uint16_t twosComplement = (~rawTemperature) + 1;
      celciusTemperature = - (twosComplement>>8) - ((twosComplement & 0x00F0)>>4)*AT30TSE75x_STEP_12BIT;
    }

    return celciusTemperature;

}

/** Get the current reading of temperature in Fahrenheit degrees
 * @return Current temperature measurement in Fahrenheit degrees
 */
float AT30TSE75x::getTemperatureFahrenheit() {

  return (getTemperatureCelcius()*9.0)/5.0 + 32.0;
  
}

/** Get the current contents of devices temperature register
 * @return Current temperature register content
 */
uint16_t AT30TSE75x::getTemperatureRaw() {
    
    uint16_t temperatureValue = 0x0000;
    I2Cdev::readWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                                    AT30TSE75x_RA_TEMPERATURE, 
                                    &temperatureValue);
    return temperatureValue;

}

/** Get the current configuration register content
 * @return Current configuration register content
 */
uint16_t AT30TSE75x::getConfiguration() {
  uint16_t config = 0x0000;
  I2Cdev::readWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, AT30TSE75x_RA_CONFIGURATION, &config);
  return config;
}

/** Get the status of Non-volatile Busy flag (Read-only)
 * @return Status of Non-volatile Busy flag
 */
bool AT30TSE75x::getNVRBusy() {
    uint16_t config = getConfiguration();
    return (config>>AT30TSE75x_CONFIG_NVRBSY_BIT)&(0x0001);
}

/** Get the status of shutdown mode flag
 * @return Status of shutdown mode
 */
bool AT30TSE75x::getShutdownMode() {
    uint16_t config = getConfiguration();
    return (config>>AT30TSE75x_CONFIG_SD_BIT)&(0x0001);
}

/** Set the device shutdown mode
 * @param true - shutdown active, false - normal operation
 */
void AT30TSE75x::setShutdownMode(bool shutdownMode) {
  uint16_t config = getConfiguration()&(~(0x01<<AT30TSE75x_CONFIG_SD_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_CONFIGURATION, 
                    config|(shutdownMode<<AT30TSE75x_CONFIG_SD_BIT));
}

/** Get the status of comparator/interrupt mode flag
 * @return Status of comparator/interrupt flag
 */
bool AT30TSE75x::getComparatorInterruptMode() {
    uint16_t config = getConfiguration();
    return (config>>AT30TSE75x_CONFIG_CMPINT_BIT)&(0x0001);
}

/** Set the device shutdown mode
 * @param false - comparator mode, true - interrupt mode
 */
void AT30TSE75x::setComparatorInterruptMode(bool comparatorOrInterrup) {
  uint16_t config = getConfiguration()&(~(0x01<<AT30TSE75x_CONFIG_CMPINT_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_CONFIGURATION, 
                    config|(comparatorOrInterrup<<AT30TSE75x_CONFIG_CMPINT_BIT));
}

/** Get the status of alert polarity flag
 * @return Status of alert polarity
 */
bool AT30TSE75x::getAlertPolarity() {
    uint16_t config = getConfiguration();
    return (config>>AT30TSE75x_CONFIG_POL_BIT)&(0x0001);
}

/** Set the alert polarity
 * @param false - alert polarity low, true - alert polarity high
 */
void AT30TSE75x::setAlertPolarity(bool alertPolarity) {
  uint16_t config = getConfiguration()&(~(0x01<<AT30TSE75x_CONFIG_POL_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_CONFIGURATION, 
                    config|(alertPolarity<<AT30TSE75x_CONFIG_POL_BIT));
}

/** Get the value of fault tolerance queue setting
 * @return Fault tolerance queue setting 
 * @see AT30TSE75x_FAULT_COUNT_1
 * @see AT30TSE75x_FAULT_COUNT_2
 * @see AT30TSE75x_FAULT_COUNT_4
 * @see AT30TSE75x_FAULT_COUNT_6
 */
uint8_t AT30TSE75x::getFaultToleranceQueue() {
    uint16_t config = getConfiguration();
    return (config>>AT30TSE75x_CONFIG_FT_BIT)&(0x0003);
}

/** Set the value of alert fault count
 * @return Value of alert fault count
 * @see AT30TSE75x_FAULT_COUNT_1
 * @see AT30TSE75x_FAULT_COUNT_2
 * @see AT30TSE75x_FAULT_COUNT_4
 * @see AT30TSE75x_FAULT_COUNT_6
 */
void AT30TSE75x::setFaultToleranceQueue(uint8_t faultCount) {
  uint16_t config = getConfiguration()&(~(0x03<<AT30TSE75x_CONFIG_FT_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_CONFIGURATION, 
                    config|(faultCount<<AT30TSE75x_CONFIG_FT_BIT));
}

/** Get the conversion resolution 
 * @return Conversion resolution setting
 * @see AT30TSE75x_RES_9BIT
 * @see AT30TSE75x_RES_10BIT
 * @see AT30TSE75x_RES_11BIT
 * @see AT30TSE75x_RES_12BIT
 */
uint8_t AT30TSE75x::getConversionResolution() {
    uint16_t config = getConfiguration();
    return (config>>AT30TSE75x_CONFIG_RES_BIT)&(0x0003);
}

/** Set the conversion resolution
 * @param conversionResolution Value of the resolution
 * @see AT30TSE75x_RES_9BIT
 * @see AT30TSE75x_RES_10BIT
 * @see AT30TSE75x_RES_11BIT
 * @see AT30TSE75x_RES_12BIT
 */
void AT30TSE75x::setConversionResolution(uint8_t conversionResolution) {
  uint16_t config = getConfiguration()&(~(0x03<<AT30TSE75x_CONFIG_RES_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_CONFIGURATION, 
                    config|(conversionResolution<<AT30TSE75x_CONFIG_RES_BIT));
}

/** Get the status of device one shot mode
 * @return false - one shot mode disabled, true - one shot mode enabled
 */
bool AT30TSE75x::getOneShotMode() {
    uint16_t config = getConfiguration();
    return (config>>AT30TSE75x_CONFIG_FT_BIT)&(0x0001);
}

/** Set the status of device one shot mode
 * @param oneShotMode false - one shot mode disabled, true - one shot mode enabled
 */
void AT30TSE75x::setOneShotMode(bool oneShotMode) {
  uint16_t config = getConfiguration()&(~(0x01<<AT30TSE75x_CONFIG_FT_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_CONFIGURATION, 
                    config|(oneShotMode<<AT30TSE75x_CONFIG_FT_BIT));
}


/** Set the value of alert low temperature limit
 * @param lowLimit Low temperature limit
 */
void AT30TSE75x::setTempLowLimit(uint16_t lowLimit) {
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_TLOW_LIMIT, 
                    lowLimit);
}

/** Get the value of alert low temperature limit
 * @return Low temperature limit
 */
uint16_t AT30TSE75x::getTempLowLimit() {
  uint16_t lowLimit = 0x0000;
  I2Cdev::readWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                                AT30TSE75x_RA_TLOW_LIMIT, 
                                &lowLimit);
  return lowLimit;
}
        
        
/** Set the value of alert high temperature limit
 * @param High temperature limit
 */
void AT30TSE75x::setTempHighLimit(uint16_t highLimit) {
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                AT30TSE75x_RA_THIGH_LIMIT, 
                highLimit);
}

/** Get the value of alert high temperature limit
 * @return High temperature limit
 */
uint16_t AT30TSE75x::getTempHighLimit() {
  uint16_t highLimit = 0x0000;
  I2Cdev::readWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                                AT30TSE75x_RA_THIGH_LIMIT, 
                                &highLimit);
  return highLimit;
}



/** Get the current non-volatile configuration register content
 * @return Current non-volatile configuration register content
 */
uint16_t AT30TSE75x::getNVConfiguration() {
  uint16_t config = 0x0000;
  I2Cdev::readWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                   AT30TSE75x_RA_NV_CONFIGURATION, 
                   &config);
  return config;
}

/** Get the status of comparator/interrupt mode from non-volatile configuration register
 * @return false - comparator mode, true - interrupt mode
 */
bool AT30TSE75x::getNVComparatorInterruptMode() {
  uint16_t config = getNVConfiguration();
  return (config>>AT30TSE75x_NVCONFIG_NVCMPINT_BIT)&(0x0001);
}

/** Set the status of comparator/interrupt mode in non-volatile configuration register
 * @param comparatorOrInterrupt false - comparator mode, true - interrupt mode
 */
void AT30TSE75x::setNVComparatorInterruptMode(bool comparatorOrInterrupt) {
  uint16_t config = getNVConfiguration()&(~(0x01<<AT30TSE75x_NVCONFIG_NVCMPINT_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_NV_CONFIGURATION, 
                    config|(comparatorOrInterrupt<<AT30TSE75x_NVCONFIG_NVCMPINT_BIT));  
}

/** Get the status of register lock from non-volatile configuration register
 * @return false - register locked, true - register unlocked
 */
bool AT30TSE75x::getNVRegisterLock() {
  uint16_t config = getNVConfiguration();
  return (config>>AT30TSE75x_NVCONFIG_RLCK_BIT)&(0x0001);
}

/** Set the status of comparator/interrupt mode in non-volatile configuration register
 * @param comparatorOrInterrupt false - comparator mode, true - interrupt mode
 */
void AT30TSE75x::setNVRegisterLock(bool registerLock) {
  uint16_t config = getNVConfiguration()&(~(0x01<<AT30TSE75x_NVCONFIG_RLCK_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_NV_CONFIGURATION, 
                    config|(registerLock<<AT30TSE75x_NVCONFIG_RLCK_BIT));    
}

/** Get the status of register lockdown from non-volatile configuration register.
 * @return false - register locked, true - register unlocked
 */
bool AT30TSE75x::getNVRegisterLockdown() {
  uint16_t config = getNVConfiguration();
  return (config>>AT30TSE75x_NVCONFIG_RLCKDWN_BIT)&(0x0001);
}

/** Set the status of comparator/interrupt mode in non-volatile configuration register
 * Warning! Calling this method with true param disables any further modifications of any registers.
 * @param registerLockdown false - registers lockdown reset, true - registers lockdown set
 */
void AT30TSE75x::setNVRegisterLockdown(bool registerLockdown) {
  uint16_t config = getNVConfiguration()&(~(0x01<<AT30TSE75x_NVCONFIG_RLCKDWN_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_NV_CONFIGURATION, 
                    config|(registerLockdown<<AT30TSE75x_NVCONFIG_RLCKDWN_BIT));
}

/** Get the status of device shutdown mode from non-volatile configuration register.
 * @return false - device shutdown unset, true - device shutdown set
 */
bool AT30TSE75x::getNVShutdownMode() {
  uint16_t config = getNVConfiguration();
  return (config>>AT30TSE75x_NVCONFIG_NVSD_BIT)&(0x0001);
}

/** Set the status of device shutdown mode from non-volatile configuration register.
 * @param shutdownMode false - device shutdown unset, true - device shutdown set
 */
void AT30TSE75x::setNVShutdownMode(bool shutdownMode) {  
  uint16_t config = getNVConfiguration()&(~(0x01<<AT30TSE75x_NVCONFIG_NVSD_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_NV_CONFIGURATION, 
                    config|(shutdownMode<<AT30TSE75x_NVCONFIG_NVSD_BIT));
}

/** Get the alert polarity from non-volatile configuration register.
 * @return false - alert polarity low, true - alert polarity high
 */
bool AT30TSE75x::getNVAlertPolarity() {
    uint16_t config = getNVConfiguration();
    return (config>>AT30TSE75x_NVCONFIG_NVPOL_BIT)&(0x0001);
}

/** Set the alert polarity from non-volatile configuration register.
 * @param alertPolarity false - alert polarity low, true - alert polarity high
 */
void AT30TSE75x::setNVAlertPolarity(bool alertPolarity) {
  uint16_t config = getNVConfiguration()&(~(0x01<<AT30TSE75x_NVCONFIG_NVPOL_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_NV_CONFIGURATION, 
                    config|(alertPolarity<<AT30TSE75x_NVCONFIG_NVPOL_BIT));  
}
 
/** Get the value of fault tolerance from non-volatile configuration register.
 * @see AT30TSE75x_FAULT_COUNT_1
 * @see AT30TSE75x_FAULT_COUNT_2
 * @see AT30TSE75x_FAULT_COUNT_4
 * @see AT30TSE75x_FAULT_COUNT_6
 * @return false - alert polarity low, true - alert polarity high
 */ 
uint8_t AT30TSE75x::getNVFaultToleranceQueue() {
    uint16_t config = getNVConfiguration();
    return (config>>AT30TSE75x_NVCONFIG_NVFT_BIT)&(0x0003);
}


/** Set the value of fault tolerance from non-volatile configuration register.
 * @param comparatorOrInterrupt false - comparator mode, true - interrupt mode
 * @see AT30TSE75x_FAULT_COUNT_1
 * @see AT30TSE75x_FAULT_COUNT_2
 * @see AT30TSE75x_FAULT_COUNT_4
 * @see AT30TSE75x_FAULT_COUNT_6
 */
void AT30TSE75x::setNVFaultToleranceQueue(uint8_t faultCount) {
  uint16_t config = getNVConfiguration()&(~(0x03<<AT30TSE75x_NVCONFIG_NVFT_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_NV_CONFIGURATION, 
                    config|(faultCount<<AT30TSE75x_NVCONFIG_NVFT_BIT));      
}

/** Get the conversion resolution from non-volatile configuration register
 * @return Conversion resolution setting
 * @see AT30TSE75x_RES_9BIT
 * @see AT30TSE75x_RES_10BIT
 * @see AT30TSE75x_RES_11BIT
 * @see AT30TSE75x_RES_12BIT
 */
uint8_t AT30TSE75x::geNVConversionResolution() {
    uint16_t config = getNVConfiguration();
    return (config>>AT30TSE75x_NVCONFIG_NVR_BIT)&(0x0003);
}

/** Set the conversion resolution from non-volatile configuration register
 * @param conversionResolution Conversion resolution setting
 * @see AT30TSE75x_RES_9BIT
 * @see AT30TSE75x_RES_10BIT
 * @see AT30TSE75x_RES_11BIT
 * @see AT30TSE75x_RES_12BIT
 */
void AT30TSE75x::setNVConversionResolution(uint8_t conversionResolution) {
  uint16_t config = getNVConfiguration()&(~(0x03<<AT30TSE75x_NVCONFIG_NVR_BIT));
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                    AT30TSE75x_RA_NV_CONFIGURATION, 
                    config|(conversionResolution<<AT30TSE75x_NVCONFIG_NVR_BIT));    
}
 
 
// Temperature alert low limit non-volatile register
void AT30TSE75x::setNVTempLowLimit(uint16_t lowLimit) {
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                AT30TSE75x_RA_NV_TLOW_LIMIT, 
                lowLimit);
}

uint16_t AT30TSE75x::getNVTempLowLimit() {
  uint16_t lowLimit = 0x0000;
  I2Cdev::readWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                                AT30TSE75x_RA_NV_TLOW_LIMIT, 
                                &lowLimit);
  return lowLimit;
}



/** Set the value of alert high temperature limit in non-volatile configuration register
 * @param highLimit High temperature limit
 */
void AT30TSE75x::setNVTempHighLimit(uint16_t highLimit) {
  I2Cdev::writeWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                AT30TSE75x_RA_NV_THIGH_LIMIT, 
                highLimit);
}

/** Get the value of alert high temperature limit in non-volatile configuration register
 * @return High temperature limit
 */
uint16_t AT30TSE75x::getNVTempHighLimit() {
  uint16_t highLimit = 0x0000;
  I2Cdev::readWord(AT30TSE75x_ADDRESS_TEMP_SENSOR | devAddr, 
                                AT30TSE75x_RA_NV_THIGH_LIMIT, 
                                &highLimit);
  return highLimit;
}


/** Reads a single EEPROM byte at given address.
 * @param address The address in the form (pageNumber*16) + byteInPage. Depending on the device the number of pages can be 16, 32, 64.
 * @return The value of the byte read from the EEPROM
 */
uint8_t AT30TSE75x::readEEPROMByte(uint16_t address) {
  
  uint8_t eepromDeviceAddress = 0x00;
  uint8_t byteAddress = 0x00;
  
  if(devType == AT30TSE75x_752) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | devAddr;
  }
  else if(devType == AT30TSE75x_754) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ((devAddr&(~0x01))|((address>>8)&0x01));
  }
  else {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ((devAddr&(~0x03))|((address>>8)&0x03));
  }
  
  byteAddress = 0x00FF&address;
  
  uint8_t result = 0x00;
  
  I2Cdev::readByte(eepromDeviceAddress, byteAddress, &result);
  
  return result;
  
}

/** Write single byte to EEPROM
 * @param address Address of byte in EEPROM
 * @param value Value to be stored at address
 */
void AT30TSE75x::writeEEPROMByte(uint16_t address, uint8_t value) {
  
  uint8_t eepromDeviceAddress = 0x00;
  uint8_t byteAddress = 0x00;
  
  if(devType == AT30TSE75x_752) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | devAddr;
  }
  else if(devType == AT30TSE75x_754) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ( (devAddr&(~0x01))|((address>>8)&0x01) );
  }
  else {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ( (devAddr&(~0x03))|((address>>8)&0x03) );
  }
  
  byteAddress = 0x00FF&address;
  
  I2Cdev::writeByte(eepromDeviceAddress, byteAddress, value);
  
}

/** Read entire page from EEPROM
 * @param address Address of page in EEPROM (must be aligned to 0 byte in the requested page)
 * @param value Pointer to where the data should be stored (must be at least 16 byte array)
 */
void AT30TSE75x::readEEPROMPage(uint16_t address, uint8_t* page) {
  
  uint8_t eepromDeviceAddress = 0x00;
  uint8_t byteAddress = 0x00;
  
  if(devType == AT30TSE75x_752) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | devAddr;
  }
  else if(devType == AT30TSE75x_754) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ((devAddr&(~0x01))|((address>>8)&0x01));
  }
  else {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ((devAddr&(~0x03))|((address>>8)&0x03));
  }
  
  byteAddress = 0x00FF&address;
  
  bool status = I2Cdev::readBytes(eepromDeviceAddress, byteAddress, 16, page);

}

/** Write entire page (16 bytes) to EEPROM
 * After writing it is good to sleep for 5-10 ms as the bytes are buffered
 * by the device before writing them to EEPROM.
 * @param address Address of page in EEPROM (must be aligned to required pages' 0 byte)
 * @param page Pointer to array of 16 bytes of data
 */
void AT30TSE75x::writeEEPROMPage(uint16_t address, uint8_t* page) {
  
  uint8_t eepromDeviceAddress = 0x00;
  uint8_t byteAddress = 0x00;
  
  if(devType == AT30TSE75x_752) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | devAddr;
  }
  else if(devType == AT30TSE75x_754) {
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ((devAddr&(~0x01))|((address>>8)&0x01));
  }
  else {
    Serial.println("Writing EEPROM page");
    eepromDeviceAddress = AT30TSE75x_ADDRESS_SERIAL_EEPROM | ((devAddr&(~0x03))|((address>>8)&0x03));
  }
  
  byteAddress = 0x00FF&address;
  
  I2Cdev::writeBytes(eepromDeviceAddress, byteAddress, 16, page);

}


/** Sets the reversible software write protection
 *  Please note that the A2-A0 pins must be set to A2=GND, A1=GND, A0=HV(7V~10V)
 */
void AT30TSE75x::setReversibleSoftwareWriteProtect() {
  I2Cdev::writeByte(AT30TSE75x_ADDRESS_WRITE_PROTECT|0x1, 0x00, 0x00);
}

/** Clears the reversible software write protection
 *  Please note that the A2-A0 pins must be set to A2=GND, A1=VCC, A0=HV(7V~10V)
 */
void AT30TSE75x::clearReversibleSoftwareWriteProtect() {
  I2Cdev::writeByte(AT30TSE75x_ADDRESS_WRITE_PROTECT|0x3, 0x00, 0x00);  
}

/** Reset the configuration register and temperature alert limits to default values. 
 * It won't change the non-volatile register.
 */
void AT30TSE75x::reset() {
   setShutdownMode(false);
   setComparatorInterruptMode(false);
   setAlertPolarity(false);
   setFaultToleranceQueue(AT30TSE75x_FAULT_COUNT_1);
   setConversionResolution(AT30TSE75x_RES_9BIT);
   setOneShotMode(false);
   setTempLowLimit(0x4B00);
   setTempHighLimit(0x5000);
}

