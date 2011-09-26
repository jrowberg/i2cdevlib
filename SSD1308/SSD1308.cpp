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

#include "SSD1308.h"
#include "I2Cdev.h"

//#ifdef SSD1308_USE_FONT
#include "fixedWidthFont.h"
//#endif

SSD1308::SSD1308(uint8_t address) :
  m_devAddr(address)
{
}

void SSD1308::initialize() 
{
  setHorizontalAddressingMode();
  clearDisplay();
}

void SSD1308::clearDisplay()
{
  setDisplayOff();
  setPageAddress(0, 7);     // all pages
  setColumnAddress(0, 127); // all columns
  for (uint8_t page = 0; page < 8; page++)
  {
    for (uint8_t col = 0; col < 128; col++)
    {
      sendData(0x0);
    }
  }
  setDisplayOn();
}

void SSD1308::fillDisplay()
{
  setPageAddress(0, MAX_PAGE);      // all pages
  setColumnAddress(0, MAX_COL); // all columns

  uint8_t b = 0;
  for (uint8_t page = 0; page < PAGES; page++)
  {
    for (uint8_t col = 0; col < COLUMNS; col++)
    {
      sendData(b++);
    }
  }
}

void SSD1308::writeChar(char chr)
{
//#ifdef SSD1308_USE_FONT
  const uint8_t char_index = chr - 0x20;
  for (uint8_t i = 0; i < 8; i++) {
     const uint8_t b = pgm_read_byte( &fontData[char_index][i] );
     sendData( b ); 
  }
//#endif
}

void SSD1308::writeString(uint8_t row, uint8_t col, uint16_t len, const char * text)
{
  uint16_t index = 0;
  setPageAddress(row, MAX_PAGE);
  const uint8_t col_addr = FONT_WIDTH*col;
  setColumnAddress(col_addr, MAX_COL);

  while ((col+index) < CHARS && (index < len)) {
     // write first line, starting at given position
     writeChar(text[index++]);
  }

  // write remaining lines
  // write until the end of memory
  // then wrap around again from the top.
  if (index + 1 < len) {
    setPageAddress(row + 1, MAX_PAGE);
    setColumnAddress(0, MAX_COL);
    bool wrapEntireScreen = false;
    while (index + 1 < len) {
       writeChar(text[index++]);
       // if we've written the last character space on the screen, 
       // reset the page and column address so that it wraps around from the top again
       if (!wrapEntireScreen && (row*CHARS + col + index) > 127) {
         setPageAddress(0, MAX_PAGE);
         setColumnAddress(0, MAX_COL);
         wrapEntireScreen = true;
       }
    }
  }
}

void SSD1308::sendCommand(uint8_t command)
{
  I2Cdev::writeByte(m_devAddr, COMMAND_MODE, command);
}

void SSD1308::sendCommands(uint8_t len, uint8_t* commands)
{
  I2Cdev::writeBytes(m_devAddr, COMMAND_MODE, len, commands);
}

void SSD1308::sendData(uint8_t data)
{
  I2Cdev::writeByte(m_devAddr, DATA_MODE, data);
}

void SSD1308::sendData(uint8_t len, uint8_t* data)
{
  I2Cdev::writeBytes(m_devAddr, DATA_MODE, len, data);
}

void SSD1308::setHorizontalAddressingMode()
{
  setMemoryAddressingMode(HORIZONTAL_ADDRESSING_MODE); 
}
void SSD1308::setVerticalAddressingMode()
{
  setMemoryAddressingMode(VERTICAL_ADDRESSING_MODE); 
}
void SSD1308::setPageAddressingMode()
{
  setMemoryAddressingMode(PAGE_ADDRESSING_MODE); 
}
    
void SSD1308::setMemoryAddressingMode(uint8_t mode)
{
  uint8_t cmds[2] = { SET_MEMORY_ADDRESSING_MODE, mode };
  sendCommands(2, cmds); 
}

void SSD1308::setDisplayOn()
{
  sendCommand(SET_DISPLAY_POWER_ON);
}

void SSD1308::setDisplayOff()
{
  sendCommand(SET_DISPLAY_POWER_OFF);
}

void SSD1308::setDisplayPower(bool on)
{
  if (on) {
    setDisplayOn();
  } else {
    setDisplayOff();
  }
}

void SSD1308::setPageAddress(uint8_t start, uint8_t end) 
{
  uint8_t data[3] = { SET_PAGE_ADDRESS, start, end };
  sendCommands(3, data);  
}

void SSD1308::setColumnAddress(uint8_t start, uint8_t end) 
{
  uint8_t data[3] = { SET_COLUMN_ADDRESS, start, end };
  sendCommands(3, data);  
}

