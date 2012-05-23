// I2Cdev library collection - SSD1308 I2C device class header file
// Based on Solomon Systech SSD1308 datasheet, rev. 1, 10/2008
// 8/25/2011 by Andrew Schamp <schamp@gmail.com>
//
// This I2C device library is using (and submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2011-08-25 - initial release
        
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

#ifndef _SSD1308_h_
#define _SSD1308_h_

#include <inttypes.h>

// this is the 7-bit I2C address
// which wone is used is determined by the D/C# pin.
// with D/C# (pin 13) grounded, address is 0x3C
// tied high, 0x3D
// assume grounded by default.
#define SSD1308_ADDRESS_1 0x3C
#define SSD1308_ADDRESS_2 0x3D
#define SSD1308_DEFAULT_ADDRESS SSD1308_ADDRESS_1

#define ROWS 64
#define COLUMNS 128
#define PAGES 8
#define PAGE_WIDTH (ROWS / 8)
#define FONT_WIDTH 8
#define CHARS (COLUMNS / FONT_WIDTH)
#define MAX_PAGE (PAGES - 1)
#define MAX_COL (COLUMNS - 1)

#define HORIZONTAL_ADDRESSING_MODE 0x00
#define VERTICAL_ADDRESSING_MODE   0x01
#define PAGE_ADDRESSING_MODE       0x02

#define SET_MEMORY_ADDRESSING_MODE 0x20 // takes one byte

#define SET_COLUMN_ADDRESS 0x21 // takes two bytes, start address and end address of display data RAM
#define SET_PAGE_ADDRESS   0x22 // takes two bytes, start address and end address of display data RAM

#define SET_CONTRAST 0x81 // takes one byte, 0x00 - 0xFF

#define SET_SEGMENT_REMAP_0   0xA0 // column address 0 is mapped to SEG0
#define SET_SEGMENT_REMAP_127 0xA1 // column address 127 is mapped to SEG0

#define SET_ENTIRE_DISPLAY_ON 0xA5 // turns all pixels on, does not affect RAM
#define SET_DISPLAY_GDDRAM    0xA4 // restores display to contents of RAM

#define SET_NORMAL_DISPLAY  0xA6 // a data of 1 indicates 'ON'
#define SET_INVERSE_DISPLAY 0xA7 // a data of 0 indicates 'ON'

#define SET_MULTIPLEX_RATIO 0xA8 // takes one byte, from 16 to 63 (0x

#define EXTERNAL_IREF 0x10
#define INTERNAL_IREF 0x00
#define SET_IREF_SELECTION 0xAD // sets internal or external Iref

#define SET_DISPLAY_POWER_OFF 0xAE
#define SET_DISPLAY_POWER_ON  0xAF

#define COMMAND_MODE 0x80
#define DATA_MODE 0x40

#define PAGE0 0x00
#define PAGE1 0x01
#define PAGE2 0x02
#define PAGE3 0x03
#define PAGE4 0x04
#define PAGE5 0x05
#define PAGE6 0x06
#define PAGE7 0x07
#define SET_PAGE_START_ADDRESS 0xB0 // | with a page number to get start address

#define SET_DISPLAY_OFFSET 0xD3

#define SET_DISPLAY_CLOCK 0xD5

#define VCOMH_DESELECT_0_65_CODE 0x00
#define VCOMH_DESELECT_0_77_CODE 0x20
#define VCOMH_DESELECT_0_83_CODE 0x30
#define SET_VCOMH_DESELECT_LEVEL 0xDB

#define NOP 0xE3

#define SET_RIGHT_HORIZONTAL_SCROLL 0x26
#define SET_LEFT_HORIZONTAL_SCROLL  0x27
#define SET_VERTICAL_RIGHT_HORIZONTAL_SCROLL 0x29
#define SET_VERTICAL_LEFT_HORIZONTAL_SCROLL  0x2A

#define SET_DEACTIVATE_SCROLL 0x2E
#define SET_ACTIVATE_SCROLL 0x2F

#define SET_VERTICAL_SCROLL_AREA 0xA3

class SSD1308
{
  public:
    
      // constructor
    // takes a 7-b I2C address to use (0x3C by default, assumes D/C# (pin 13) grounded)
    SSD1308(uint8_t address = SSD1308_DEFAULT_ADDRESS);

    void initialize();
    void clearDisplay();
    void fillDisplay(); // crosshatches    
    
    // x, y is position (x is row (i.e., page), y is character (0-15), starting at top-left)
    // text will wrap around until it is done.
    void writeString(uint8_t row, uint8_t col, uint16_t len, const char* txt);
    
    //void setXY(uint8_t, uint8_t y);

    void setHorizontalAddressingMode();
    void setVerticalAddressingMode();
    void setPageAddressingMode();
    
    void setMemoryAddressingMode(uint8_t mode);
    
    // takes one byte, 0x00-0x0F
    void setLowerColumnStartAddressForPageAddressingMode(uint8_t address);
    
    // takes one byte, 0x10-0x1F
    void setHigherColumnStartAddressForPageAddressingMode(uint8_t address);
    
    // takes two bytes, start address and end address of display data RAM
    void setColumnAddress(uint8_t start, uint8_t end);
    
    // takes two bytes, start address and end address of display data RAM
    void setPageAddress(uint8_t start, uint8_t end);
    
    // takes one byte, PAGE0 - PAGE7
    void setPageStartForPageAddressingMode(uint8_t page);
    
    // takes one byte, 0x40-0x7F
    void setDisplayStartLine(uint8_t line);
    
    // takes one byte, 0x00 (lowest) - 0xFF (highest)
    void setContrastControl(uint8_t contrast);
    
    void setEntireDisplayOn();
    void setEntireDisplayRAM();
    void setEntireDisplay(bool on);
    void setNormalDisplay();
    void setInverseDisplay();
    
    // setMultiplexRatio
    
    void setInternalIref();
    void setExternalIref();
    
    void setDisplayOn();
    void setDisplayOff();
    void setDisplayPower(bool on);
    
    // Set vertical shift by COM from 0 - 63 (0x00 - 0x3F)
    // set to 0x00 after RESET
    void setDisplayOffset(uint8_t offset);
    
    // divide ratio 0x00-0x0F, value +1 (reset 0x00)
    // oscillator freq 0x00-0x0F (reset 0x08)
    void setDisplayClock(uint8_t divideRatio, uint8_t oscFreq);
    
    // phase1 0x01-0x0F period of up to 15 DCLK clocks (reset 0x02, 0 is invalid)
    // phase2 0x01-0x0F period of up to 15 DCLK clocks (reset 0x02, 0 is invalid)
    void setPrechargePeriod(uint8_t phase1, uint8_t phase2);
    
    #define VCOM_DESELECT_0_65 0x00
    #define VCOM_DESELECT_0_77 0x02
    #define VCOM_DESELECT_0_83 0x03
    void setVcomhDeselectLevel(uint8_t level);
    
    // command for no operation
    void nop();
    
    #define SCROLL_INTERVAL_5_FRAMES   0x00
    #define SCROLL_INTERVAL_64_FRAMES  0x01
    #define SCROLL_INTERVAL_128_FRAMES 0x02
    #define SCROLL_INTERVAL_256_FRAMES 0x03
    #define SCROLL_INTERVAL_3_FRAMES   0x04
    #define SCROLL_INTERVAL_4_FRAMES   0x05
    #define SCROLL_INTERVAL_25_FRAMES  0x06
    #define SCROLL_INTERVAL_2_FRAMES   0x07
    // end_page must not be less than start_page
    void setContinuousHorizontalScroll(bool left, uint8_t start_page, uint8_t interval, uint8_t end_page);
    // horizontal scroll by one column per interval
    // offset = 1 (0x01) to 63 (0x3F)
    void setContinuousVerticalAndHorizontalScroll(bool left, uint8_t start_page, uint8_t interval, uint8_t end_page, uint8_t offset);
    
    // note, after deactivating scrolling, the RAM data needs to be rewritten
    void deactivateScroll();
    void activateScroll();
    
    void setVerticalScrollArea(uint8_t topRowsFixed, uint8_t scrollRows);

    void sendData(uint8_t data);
    void sendData(uint8_t len, uint8_t* data);
    // write the configuration registers in accordance with the datasheet and app note 3944
//    void initialize();
    
    // returns true if the device is responding on the I2C bus
//    bool testConnection();

    // getTouchStatus returns the touch status for the given channel (0 - 11)
//    bool getTouchStatus(uint8_t channel);
    // when not given a channel, returns a bitfield of all touch channels.
//    uint16_t getTouchStatus();

  private:
    // sends a single-byte command (given) to device
    void sendCommand(uint8_t command);
    void sendCommands(uint8_t len, uint8_t* buf);

    void writeChar(char chr);
    
    uint8_t m_devAddr; // contains the I2C address of the device
};

#endif

