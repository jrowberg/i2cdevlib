// I2Cdev library collection - MAX6956 I2C device class
// Based on Maxim MAX6956 datasheet 19-2414; Rev 4; 6/10
//
// 2013-12-15 by Tom Beighley <tomthegeek@gmail.com>
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2013-12-15 - initial release
//

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2013 Tom Beighley

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

#include "MAX6956.h"

/** Default constructor, uses default I2C address.
 * @see MAX6956_DEFAULT_ADDRESS
 */
MAX6956::MAX6956() {
    devAddr = MAX6956_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see MAX6956_DEFAULT_ADDRESS
 * @see MAX6956_ADDRESS
 */
MAX6956::MAX6956(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage. */
void MAX6956::initialize() {
    // Need this in setup() or here.
    // Fastwire::setup(400, false);
    reset();
    
    /** The 28-pin MAX6956ANI and MAX6956AAI make only 20 ports available, P12 to P31.
    The eight unused ports should be configured as outputs on power-up by writing 0x55 to registers 0x09 and 0x0A. 
    If this is not done,the eight unused ports remain as unconnected inputs and quiescent supply current rises, 
    although there is no damage to the part.
    */
    I2Cdev::writeByte(devAddr, MAX6956_RA_CONFIG_P7P6P5P4, 0x55);
    I2Cdev::writeByte(devAddr, MAX6956_RA_CONFIG_P11P10P9P8, 0x55);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool MAX6956::testConnection() {
    if (I2Cdev::readByte(devAddr, MAX6956_RA_CONFIGURATION, buffer) == 1) {
        return true;
    }
    return false;
}

/** Set registers back to Power-up Configuration */
void MAX6956::reset() {
    disableAllPorts(); // set Port RA 0x2C-0x3F to 0
    setGlobalCurrent(0x00);  // Set global current to minimum on
    setConfigReg(0x00); // Shutdown Enabled,Current Control = Global, Transition Detection Disabled 
    setInputMask(0x00); // Set Input register mask to 0
    setTestMode(false); // disable test mode
    configAllPorts(MAX6956_INPUT_WO_PULL); // Configure all inputs as MAX6956_INPUT_WO_PULL
    setAllPortsCurrent(0x00); // Set current RA 0x16-01F to 0
}

/** Config register
@return uint8_t value of register
@see MAX6956_RA_CONFIGURATION
*/
uint8_t MAX6956::getConfigReg() {
    I2Cdev::readByte(devAddr, MAX6956_RA_CONFIGURATION, buffer);
    return buffer[0];
}
/**	Set config register
@param config uint8_t value to set register
@see MAX6956_RA_CONFIGURATION
*/
void MAX6956::setConfigReg(uint8_t config) {
    I2Cdev::writeByte(devAddr, MAX6956_RA_CONFIGURATION, config);
}

/**	Get power configuration bit 
@return Boolean value if power is enabled or not
@see MAX6956_RA_CONFIGURATION
*/
bool MAX6956::getPower() {
    I2Cdev::readBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_POWER_BIT, buffer);
    return buffer[0];
}
/**	Enable or disable power
@param power Boolean value, true enables, false disables power
@see MAX6956_RA_CONFIGURATION
*/
void MAX6956::setPower(bool power) {
    I2Cdev::writeBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_POWER_BIT, power);
}
/**	Toggle power
@see MAX6956_RA_CONFIGURATION
*/
void MAX6956::togglePower() {
    I2Cdev::readBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_POWER_BIT, buffer);
    buffer[0] = !(buffer[0]);
    I2Cdev::writeBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_POWER_BIT, buffer[0]);
}

/**	Get transition detection configuration bit 
@return Boolean value if global current is enabled or not
@see MAX6956_RA_CONFIGURATION
*/
bool MAX6956::getEnableIndividualCurrent() {
    I2Cdev::readBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_GLOBAL_CURRENT_BIT, buffer);
    return buffer[0];
}
/**	Enable or disable global current
@param global Boolean value, true enables, false disables individual current
@see MAX6956_RA_CONFIGURATION
@see getGlobalCurrent()
@see setGlobalCurrent()
*/
void MAX6956::setEnableIndividualCurrent(bool global) {
    I2Cdev::writeBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_GLOBAL_CURRENT_BIT, global);
}

/**	Get transition detection configuration bit 
@return Boolean value if transition detection is enabled or not
@see MAX6956_RA_CONFIGURATION
*/
bool MAX6956::getEnableTransitionDetection() {
    I2Cdev::readBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_TRANSITION_BIT, buffer);
    return buffer[0];
}
/**	Enable or disable transition detection. Must be reset after every mask read.
@param transition Boolean value, true enables, false disables transition detection
@see MAX6956_RA_CONFIGURATION
@see getInputMask()
@see setInputMask()
*/
void MAX6956::setEnableTransitionDetection(bool transition) {
    I2Cdev::writeBit(devAddr, MAX6956_RA_CONFIGURATION, MAX6956_CONFIG_TRANSITION_BIT, transition);
}

// Global Current
/**	Get global current 
@return uint8_t value of register
@see MAX6956_RA_GLOBAL_CURRENT
*/
uint8_t MAX6956::getGlobalCurrent() {
    I2Cdev::readBits(devAddr, MAX6956_RA_GLOBAL_CURRENT, MAX6956_GLOBAL_CURRENT_BIT, MAX6956_GLOBAL_CURRENT_LENGTH, buffer);
    return buffer[0];
}
/**	Set current globally
@param current uint8_t 0-15, 0 = min, 15 = max brightness
@see MAX6956_RA_GLOBAL_CURRENT
*/
void MAX6956::setGlobalCurrent(uint8_t current) {
    I2Cdev::writeByte(devAddr, MAX6956_RA_GLOBAL_CURRENT, current);
}

// Test mode
/**	Returns true if test mode is enabled. 
@return uint8_t value of register
@see MAX6956_RA_DISPLAY_TEST
*/
bool MAX6956::getTestMode() {
    I2Cdev::readBit(devAddr, MAX6956_RA_DISPLAY_TEST, MAX6956_DISPLAY_TEST_BIT, buffer);
    return buffer[0];
}
/**	Enable or disable test mode
@param test Boolean, true enables, false disables
@see MAX6956_RA_DISPLAY_TEST
*/
void MAX6956::setTestMode(bool test) {
    I2Cdev::writeByte(devAddr, MAX6956_RA_DISPLAY_TEST, test);
}

// Input mask register
/**	Get the input mask to see which ports have changed. MSB is cleared after every read.
@return uint8_t value of register
@see MAX6956_RA_TRANSITION_DETECT
*/
uint8_t MAX6956::getInputMask() {
    I2Cdev::readByte(devAddr, MAX6956_RA_TRANSITION_DETECT, buffer);
    return buffer[0];
}

/**	Set the input mask for which ports to monitor with transition detection
@param mask 8 bit value. MSB is ignored.
@see MAX6956_RA_TRANSITION_DETECT
*/
void MAX6956::setInputMask(uint8_t mask) {
    I2Cdev::writeByte(devAddr, MAX6956_RA_TRANSITION_DETECT, mask);
}


/**	Takes an INDIVIDUAL port register address (MAX6956_RA_PORT12, ect)
	and configures it as: MAX6956_OUTPUT_LED, MAX6956_OUTPUT_GPIO,
    MAX6956_INPUT_WO_PULL, or MAX6956_INPUT_W_PULL

Port registers
--------------    

Addr. | Name
:----:|:------------------------------:
0x09  | MAX6956_RA_CONFIG_P7P6P5P4				
0x0A  | MAX6956_RA_CONFIG_P11P10P9P8			
0x0B  | MAX6956_RA_CONFIG_P15P14P13P12			
0x0C  | MAX6956_RA_CONFIG_P19P18P17P16			
0x0D  | MAX6956_RA_CONFIG_P23P22P21P20			
0x0E  | MAX6956_RA_CONFIG_P27P26P25P24			
0x0F  | MAX6956_RA_CONFIG_P31P30P29P28			

Num|Dec|Hex |Prt|Port Status Array|Ports Config Array
:-:|:-:|:--:|:-:|:---------------:|:-----------------:
00 |44 |0x2C|P12|portsStatus[0][0]|portsConfig[0][1-0]     
01 |45 |0x2D|P13|portsStatus[0][1]|portsConfig[0][3-2]        
02 |46 |0x2E|P14|portsStatus[0][2]|portsConfig[0][5-4]
03 |47 |0x2F|P15|portsStatus[0][3]|portsConfig[0][7-6]
04 |48 |0x30|P16|portsStatus[0][4]|portsConfig[1][1-0]
05 |49 |0x31|P17|portsStatus[0][5]|portsConfig[1][3-2]
06 |50 |0x32|P18|portsStatus[0][6]|portsConfig[1][5-4]
07 |51 |0x33|P19|portsStatus[0][7]|portsConfig[1][7-6]
08 |52 |0x34|P20|portsStatus[1][0]|portsConfig[2][1-0]                         
09 |53 |0x35|P21|portsStatus[1][1]|portsConfig[2][3-2]
10 |54 |0x36|P22|portsStatus[1][2]|portsConfig[2][5-4]
11 |55 |0x37|P23|portsStatus[1][3]|portsConfig[2][7-6]                
12 |56 |0x38|P24|portsStatus[1][4]|portsConfig[3][1-0]
13 |57 |0x39|P25|portsStatus[1][5]|portsConfig[3][3-2]
14 |58 |0x3A|P26|portsStatus[1][6]|portsConfig[3][5-4]
15 |59 |0x3B|P27|portsStatus[1][7]|portsConfig[3][7-6]
16 |60 |0x3C|P28|portsStatus[2][0]|portsConfig[4][1-0]    
17 |61 |0x3D|P29|portsStatus[2][1]|portsConfig[4][3-2]
18 |62 |0x3E|P30|portsStatus[2][2]|portsConfig[4][5-4]
19 |63 |0x3F|P31|portsStatus[2][3]|portsConfig[4][7-6]
 
 @param port Port register address (MAX6956_RA_PORT12, ect)
 @param portConfig Valid options are: MAX6956_OUTPUT_LED, MAX6956_OUTPUT_GPIO, MAX6956_INPUT_WO_PULL, MAX6956_INPUT_W_PULL
 @see MAX6956_RA_PORT12 MAX6956_RA_PORT13 MAX6956_RA_PORT14 MAX6956_RA_PORT15 MAX6956_RA_PORT16 MAX6956_RA_PORT17 MAX6956_RA_PORT18 MAX6956_RA_PORT19 MAX6956_RA_PORT20 MAX6956_RA_PORT21
      MAX6956_RA_PORT22 MAX6956_RA_PORT23 MAX6956_RA_PORT24 MAX6956_RA_PORT25 MAX6956_RA_PORT26 MAX6956_RA_PORT27 MAX6956_RA_PORT28 MAX6956_RA_PORT29 MAX6956_RA_PORT30 MAX6956_RA_PORT31
 @see MAX6956_OUTPUT_LED MAX6956_OUTPUT_GPIO MAX6956_INPUT_WO_PULL MAX6956_INPUT_W_PULL
*/
void MAX6956::configPort(uint8_t port, uint8_t portConfig) {
    uint8_t bitPosition = (((port - 44) % 4) * 2) + 1; // bit position to start flipping 
    uint8_t pcArrayIndex = (port - 44) / 4; // array index
    uint8_t portConfigRA = pcArrayIndex + 11; // port config register
	I2Cdev::writeBits(devAddr, portConfigRA, bitPosition, 2, portConfig);
    // Update portConfig array
    bitPosition--; // Remove 1 bit offset
    portsConfig[pcArrayIndex] &=  ~(3 << bitPosition);
    portsConfig[pcArrayIndex] |=  portConfig << bitPosition;   
}

/** Configure consecutive range of ports
 * @param lower Lower port register address (MAX6956_RA_PORT12, ect)
 * @param upper Upper port register address (MAX6956_RA_PORT12, ect)
 * @param portConfig Valid options are: MAX6956_OUTPUT_LED, MAX6956_OUTPUT_GPIO, MAX6956_INPUT_WO_PULL, MAX6956_INPUT_W_PULL
 * @see MAX6956_OUTPUT_LED
 * @see MAX6956_OUTPUT_GPIO
 * @see MAX6956_INPUT_WO_PULL
 * @see MAX6956_INPUT_W_PULL
*/
void MAX6956::configPorts(uint8_t lower, uint8_t upper, uint8_t portConfig) {

    #ifdef MAX6956_SERIAL_DEBUG
        Serial.print("lower ");
        Serial.print(lower, HEX);
        Serial.print(" upper ");
        Serial.print(upper, HEX);
        Serial.print(" portConfig ");
        Serial.println(portConfig, BIN);
    #endif
    
    uint8_t bitPosition = 0;
    uint8_t pcArrayIndex = 0;
    uint8_t portConfigRA = 0;
    uint8_t i = lower;
    
    while ( i <= upper ) { 
        bitPosition = (((i - 44) % 4) * 2) + 1; // bit position to start flipping 
        pcArrayIndex = (i - 44) / 4; // array index
        portConfigRA = pcArrayIndex + 11; // port config register
        
        #ifdef MAX6956_SERIAL_DEBUG
            Serial.print("i ");
            Serial.print(i);
            Serial.print(" bitPosition ");
            Serial.print(bitPosition);
            Serial.print(" pcArrayIndex");
            Serial.print(pcArrayIndex);
            Serial.print(" portConfigRA ");
            Serial.println(portConfigRA, HEX);
        #endif
        
        I2Cdev::writeBits(devAddr, portConfigRA, bitPosition, 2, portConfig);
        
        #ifdef MAX6956_SERIAL_DEBUG
            Serial.print("portsConfig ");
            Serial.print(portsConfig[pcArrayIndex], BIN);
        #endif
        
        // Update portConfig array
        bitPosition--; // Remove 1 bit offset
        portsConfig[pcArrayIndex] &=  ~(3 << bitPosition); //3 == B00000011 Shift over correct number of bits then invert to create the mask
        portsConfig[pcArrayIndex] |=  portConfig << bitPosition; 
        
        #ifdef MAX6956_SERIAL_DEBUG
            Serial.print(" >> ");
            Serial.println(portsConfig[pcArrayIndex], BIN);
        #endif
        
        i++;
    }
  
}

/**	Configure all ports the same
 @param portConfig Valid options are: MAX6956_OUTPUT_LED, MAX6956_OUTPUT_GPIO, MAX6956_INPUT_WO_PULL, MAX6956_INPUT_W_PULL
 @see MAX6956_OUTPUT_LED
 @see MAX6956_OUTPUT_GPIO
 @see MAX6956_INPUT_WO_PULL
 @see MAX6956_INPUT_W_PULL
*/
void MAX6956::configAllPorts(uint8_t portConfig) {
    // build byte with config bits shifted to all 4 slots
    portConfig = portConfig << 6 + portConfig << 4 + portConfig << 2 + portConfig;
    // copy byte to portsConfig array slots
    memset (portsConfig, portConfig, 5);
    // write bytes all at once to device
    I2Cdev::writeBytes(devAddr, MAX6956_RA_CONFIG_P15P14P13P12, 5, portsConfig);
}

/** Write 1's to all port registers. This enables ports set as outputs and 
*   they will always have some brightness, port must be disabled to 
*   turn off completely. */
void MAX6956::enableAllPorts() {
    // set portStatus array to all 1's
    memset (portsStatus, 0xFF, 3);
    // write bytes
    I2Cdev::writeByte(devAddr, MAX6956_RA_PORTS12_19, portsStatus[0]);
    I2Cdev::writeByte(devAddr, MAX6956_RA_PORTS20_27, portsStatus[1]);
    I2Cdev::writeByte(devAddr, MAX6956_RA_PORTS28_31, portsStatus[2]);
}

/** Write 0's to all port registers. */
void MAX6956::disableAllPorts() {
    // set portStatus array to all 0's
    memset (portsStatus, 0x00, 3);
    // write bytes
    I2Cdev::writeByte(devAddr, MAX6956_RA_PORTS12_19, portsStatus[0]);
    I2Cdev::writeByte(devAddr, MAX6956_RA_PORTS20_27, portsStatus[1]);
    I2Cdev::writeByte(devAddr, MAX6956_RA_PORTS28_31, portsStatus[2]);
}

      
/** Enables/Disables any port.set as a LED driver 
*/
void MAX6956::toggleLEDs() {
  // Toggle power on all pins set as LED drivers
  for (int i = 0; i < sizeof(portsConfig); i++){
    uint8_t portRA = 0;
    uint8_t bitPosition = 0;
    uint8_t psArrayIndex = 0;
    for (int j = 3; j >= 0; j--){
      if (((portsConfig[i] >> (j * 2)) & B00000011) == 0) {
        portRA = j+(i*4);
        bitPosition = portRA % 8; // bit position to flip 
        psArrayIndex = portRA / 8; // array index
        portRA += 44; // Finally adjust portRA for actual offset.
        if (portsStatus[psArrayIndex] & (1 << bitPosition )) {
        setPort(portRA, false);
        } else {
        setPort(portRA, true);
        }
      }
    }
  }
}      
      
/** Enables/Disables indiviual port. 
 @param port Port register address (MAX6956_RA_PORT12, ect)
 @param enabled true or false
 @see MAX6956_RA_PORT12 MAX6956_RA_PORT13 MAX6956_RA_PORT14 MAX6956_RA_PORT15 MAX6956_RA_PORT16 MAX6956_RA_PORT17 MAX6956_RA_PORT18 MAX6956_RA_PORT19 MAX6956_RA_PORT20 MAX6956_RA_PORT21
      MAX6956_RA_PORT22 MAX6956_RA_PORT23 MAX6956_RA_PORT24 MAX6956_RA_PORT25 MAX6956_RA_PORT26 MAX6956_RA_PORT27 MAX6956_RA_PORT28 MAX6956_RA_PORT29 MAX6956_RA_PORT30 MAX6956_RA_PORT31
*/
void MAX6956::setPort(uint8_t port, bool enabled) {
    if ( port >= 44 && port <= 63) { 
        I2Cdev::writeByte(devAddr, port, enabled);
        
        #ifdef MAX6956_SERIAL_DEBUG
            Serial.print("port ");
            Serial.print(port, HEX);
            Serial.print(" is ");
            Serial.println(enabled);
        #endif
        
        // Update portConfig array
        uint8_t bitPosition = (port - 44) % 8; // bit position to flip 
        uint8_t psArrayIndex = (port - 44) / 8; // array index
        portsStatus[psArrayIndex] &=  ~(1 << bitPosition);
        portsStatus[psArrayIndex] |=  enabled << bitPosition;
    }
}

/**	Takes an individual port register address (MAX6956_RA_PORT12, ect)
	and returns the value of the port (data bit D0; D7–D1 read as 0) .
	Returns 129 for out of bounds requests.
 * @return data bit D0; D7–D1 read as 0. D7 set to 1 on out of bounds.
 * @param port Port register address (MAX6956_RA_PORT12, ect)
*/
uint8_t MAX6956::getPort(uint8_t port) {
    if ( port >= 44 && port <= 63) { 
        I2Cdev::readByte(devAddr, port, buffer); // read port
        
        #ifdef MAX6956_SERIAL_DEBUG
            Serial.print("port ");
            Serial.print(port);
            Serial.print(" is ");
            Serial.println(buffer[0]);
        #endif
        
        return buffer[0];
	} else 
	    return 129;
}

/**	Reads the value of the transition detection ports (24-30) all at once.
 * @return uint8_t value of ports
*/
uint8_t MAX6956::getTransitionPorts() {
    I2Cdev::readByte(devAddr, 0x58, buffer); // read ports

    #ifdef MAX6956_SERIAL_DEBUG
        Serial.print("Transition ports: ");
        Serial.println(buffer[0], BIN);
    #endif
    
    return buffer[0];
}

/**	Takes an individual port register address (MAX6956_RA_PORT12, ect)
	and sets it to a power scale where 0 = off. 
 * @param port Port register address (MAX6956_RA_PORT12, ect)
 * @param power 0 is off, 15 is max brightness.
*/
void MAX6956::setPortLevel(uint8_t port, uint8_t power) {
  if ( port >= 44 && port <= 63 && power >= 0 && power <= 15 ) { 
    //power = sqrt(2) * power; // Convert linear to log scale
  
    psArrayIndex = (port - 44) / 8; // array index
    psBitPosition = (port - 44) % 8; // bit position to flip 
  
    if ( power > 0 ) { 
      // When not at 100% power decrement power by one because 0 is 
      // really an "on" power level and the lower levels are more important.
      if ( power < 15 ) power--;
      
      // Calculate current register from port address
      // set current bit offset
      if ( port % 2 ) { //port is odd
        portCurrentRA = (port - 1) / 2;
        portCurrentBit = 7;
      } 
            else {
        portCurrentRA = port / 2;
        portCurrentBit = 3;
      }
      
      // Write changes
      I2Cdev::writeBits(devAddr, portCurrentRA, portCurrentBit, 4, power); // Write changes
      I2Cdev::writeByte(devAddr, port, MAX6956_ON); // turn on port
  
      #ifdef MAX6956_SERIAL_DEBUG
          Serial.print("port ");
          Serial.print(port);
          Serial.print(" portCurrentBit ");
          Serial.print(portCurrentBit);
          Serial.print(" portCurrentRA ");
          Serial.print(portCurrentRA, HEX);
          Serial.print(" power ");
          Serial.println(power	);
      #endif
      
      // Update portStatus array
      portsStatus[psArrayIndex] |=  1 << psBitPosition;   
    
    } else {
      I2Cdev::writeByte(devAddr, port, MAX6956_OFF); // turn off port
      portsStatus[psArrayIndex] &=  ~(1 << psBitPosition);   // Update portStatus array
  
      #ifdef MAX6956_SERIAL_DEBUG
          Serial.print("port ");
          Serial.print(port);
          Serial.println(" is off");
      #endif
    }
	} 
}


/**	Takes an INDIVIDUAL port register address (MAX6956_RA_PORT12, ect)
	and sets it to a power level 0-15. 0 is min, 15 is max brightness. 
 * @param port Port register address (MAX6956_RA_PORT12, ect)
 * @param power 0 is min, 15 is max brightness.
*/
void MAX6956::setPortCurrent(uint8_t port, uint8_t power) {
    if ( port >= 44 && port <= 63 && power >= 0 && power <= 15 ) { 
		//power = sqrt(2) * power; /** TODO Convert linear to log scale */

	    psArrayIndex = (port - 44) / 8; // array index
        psBitPosition = (port - 44) % 8; // bit position to flip 

        // Calculate current register from port address
        // set current bit offset
        if ( port % 2 ) { //port is odd
            portCurrentRA = (port - 1) / 2;
            portCurrentBit = 7;
        } 
        else {
            portCurrentRA = port / 2;
            portCurrentBit = 3;
        }
        
        // Write changes
        I2Cdev::writeBits(devAddr, portCurrentRA, portCurrentBit, 4, power); // Write changes
        I2Cdev::writeByte(devAddr, port, MAX6956_ON); // turn on port

        #ifdef MAX6956_SERIAL_DEBUG
            Serial.print("port ");
            Serial.print(port);
            Serial.print(" portCurrentBit ");
            Serial.print(portCurrentBit);
            Serial.print(" portCurrentRA ");
            Serial.print(portCurrentRA, HEX);
            Serial.print(" power ");
            Serial.println(power	);
        #endif
        // Update portStatus array
        portsStatus[psArrayIndex] |=  1 << psBitPosition;   
	} 
}

/**	Set ALL port current registers (MAX6956_RA_PORT12, ect) to 
    the SAME power level 0-15. 0 min brightness, 15 is max brightness.
    @param power 0 is min, 15 is max brightness.
*/
void MAX6956::setAllPortsCurrent(uint8_t power) {
   for (int portCurrentRA=MAX6956_RA_CURRENT_0xP13P12; portCurrentRA <= MAX6956_RA_CURRENT_0xP31P30; portCurrentRA++){
    I2Cdev::writeByte(devAddr, portCurrentRA, power);
   } 
}
