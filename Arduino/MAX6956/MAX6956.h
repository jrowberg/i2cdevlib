// I2Cdev library collection - MAX6956 I2C device class header file
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


/*! \mainpage Maxim Integrated Products

The MAX6956 compact, serial-interfaced LED display driver/I/O expander provide microprocessors with up to 28 ports. Each port is individually user configurable to either a logic input, logic output, or common-anode (CA) LED constant-current segment driver. Each port configured as an LED segment driver behaves as a digitally controlled constant-current sink, with 16 equal current steps from 1.5mA to 24mA. The LED drivers are suitable for both discrete LEDs and CA numeric and alphanumeric LED digits.

Each port configured as a general-purpose I/O (GPIO) can be either a push-pull logic output capable of sink- ing 10mA and sourcing 4.5mA, or a Schmitt logic input with optional internal pullup. Seven ports feature config- urable transition detection logic, which generates an interrupt upon change of port logic level. The MAX6956 is controlled through an I2C-compatible 2-wire serial interface, and uses four-level logic to allow 16 I 2C addresses from only 2 select pins.

The MAX6956AAX and MAX6956ATL have 28 ports and are available in 36-pin SSOP and 40-pin thin QFN packages, respectively. The MAX6956AAI and MAX6956ANI have 20 ports and are available in 28-pin SSOP and 28-pin DIP packages, respectively.

For an SPI-interfaced version, refer to the MAX6957 data sheet. For a lower cost pin-compatible port expander without the constant-current LED drive capa- bility, refer to the MAX7300 data sheet.

- 400kbps I2C-Compatible Serial Interface
- 2.5V to 5.5V Operation
- -40°C to +125°C Temperature Range
- 20 or 28 I/O Ports, Each Configurable as
 - Constant-Current LED Driver
 - Push-Pull Logic Output
 - Schmitt Logic Input
 - Schmitt Logic Input with Internal Pullup
- 11μA (max) Shutdown Current
- 16-Step Individually Programmable Current
- Control for Each LED
- Logic Transition Detection for Seven I/O Ports

*/

#ifndef _MAX6956_H_
#define _MAX6956_H_

#include "I2Cdev.h"

// -----------------------------------------------------------------------------
// Arduino-style "Serial.print" debug constant (uncomment to enable)
// -----------------------------------------------------------------------------
// #define MAX6956_SERIAL_DEBUG

/**
Table 3. I2C Address Map
========================

Pin|Pin|DEVICE ADDRESS                 |
:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:--:   
AD1|AD0|A7 |A6 |A5 |A4 |A3 |A2 |A1 |A0 |HEX
GND|GND|1  |0  |0  |0  |0  |0  |0  |RW |0x40
GND|V+ |1  |0  |0  |0  |0  |0  |1  |RW |0x41
GND|SDA|1  |0  |0  |0  |0  |1  |0  |RW |0x42
GND|SCL|1  |0  |0  |0  |0  |1  |1  |RW |0x43
V+ |GND|1  |0  |0  |0  |1  |0  |0  |RW |0x44
V+ |V+ |1  |0  |0  |0  |1  |0  |1  |RW |0x45
V+ |SDA|1  |0  |0  |0  |1  |1  |0  |RW |0x46
V+ |SCL|1  |0  |0  |0  |1  |1  |1  |RW |0x47
SDA|GND|1  |0  |0  |1  |0  |0  |0  |RW |0x48
SDA|V+ |1  |0  |0  |1  |0  |0  |1  |RW |0x49
SDA|SDA|1  |0  |0  |1  |0  |1  |0  |RW |0x50
SDA|SCL|1  |0  |0  |1  |0  |1  |1  |RW |0x51
SCL|GND|1  |0  |0  |1  |1  |0  |0  |RW |0x52
SCL|V+ |1  |0  |0  |1  |1  |0  |1  |RW |0x53
SCL|SDA|1  |0  |0  |1  |1  |1  |0  |RW |0x54
SCL|SCL|1  |0  |0  |1  |1  |1  |1  |RW |0x55
                           
RW bit 0 = Write 1 = Read
*/
#define MAX6956_ADDRESS					0x40 // See table 3 
#define MAX6956_DEFAULT_ADDRESS			0x40 // AD0 and AD1 at ground.

//==========Registers==========

#define MAX6956_RA_NO_OP				0x00 ///< No-op

/**
Table 11. Global Segment Current Register Format
================================================

LED DRIVE |TYPICAL SEGMENT | ADDRESS    |
FRACTION  |  CURRENT(mA)   |CODE (HEX)  |D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0 |HEX CODE
:--------:|:--------------:|:----------:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:------:
1/16      |     1.5        |   0x02     |x  |x  |x  |x  |0  |0  |0  |0  |0xX0
2/16      |      3         |   0x02     |x  |x  |x  |x  |0  |0  |0  |1  |0xX1
3/16      |     4.5        |   0x02     |x  |x  |x  |x  |0  |0  |1  |0  |0xX2
4/16      |      6         |   0x02     |x  |x  |x  |x  |0  |0  |1  |1  |0xX3
5/16      |     7.5        |   0x02     |x  |x  |x  |x  |0  |1  |0  |0  |0xX4
6/16      |      9         |   0x02     |x  |x  |x  |x  |0  |1  |0  |1  |0xX5
7/16      |    10.5        |   0x02     |x  |x  |x  |x  |0  |1  |1  |0  |0xX6
8/16      |     12         |   0x02     |x  |x  |x  |x  |0  |1  |1  |1  |0xX7
9/16      |    13.5        |   0x02     |x  |x  |x  |x  |1  |0  |0  |0  |0xX8
10/16     |     15         |   0x02     |x  |x  |x  |x  |1  |0  |0  |1  |0xX9
11/16     |    16.5        |   0x02     |x  |x  |x  |x  |1  |0  |1  |0  |0xXA
12/16     |     18         |   0x02     |x  |x  |x  |x  |1  |0  |1  |1  |0xXB
13/16     |    19.5        |   0x02     |x  |x  |x  |x  |1  |1  |0  |0  |0xXC
14/16     |     21         |   0x02     |x  |x  |x  |x  |1  |1  |0  |1  |0xXD
15/16     |    22.5        |   0x02     |x  |x  |x  |x  |1  |1  |1  |0  |0xXE
16/16     |     24         |   0x02     |x  |x  |x  |x  |1  |1  |1  |1  |0xXF
*/                                                      
#define MAX6956_RA_GLOBAL_CURRENT		0x02

/**
Table 7. Configuration Register Format
======================================

ADDRESS |D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0
:------:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
0x04    |M  |I  |x  |x  |x  |x  |x  |S

Table 8. Shutdown Control (S Data Bit D0) Format
================================================

FUNCTION        |ADDRESS|D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0
:--------------:|:-----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
Shutdown        |0x04   |M  |I  |x  |x  |x  |x  |x  |0
Normal Operation|0x04   |M  |I  |x  |x  |x  |x  |x  |1

Table 9. Global Current Control (I Data Bit D6) Format
======================================================

FUNCTION          |ADDRESS|D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0
:----------------:|:-----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
Global            |0x04   |M  |0  |x  |x  |x  |x  |x  |S
Individual Segment|0x04   |M  |1  |x  |x  |x  |x  |x  |S 

Table 10. Transition Detection Control (M-Data Bit D7) Format
=============================================================

FUNCTION  |ADDRESS|D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0
:--------:|:-----:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
Disabled  | 0x04  |0  |I  |x  |x  |x  |x  |x  |S
Enabled   | 0x04  |1  |I  |x  |x  |x  |x  |x  |S

*/
#define MAX6956_RA_CONFIGURATION		0x04

/**
Table 15. Transition Detection Mask Register
============================================

|FUNCTION       |REGISTER ADDRESS |READ/WRITE | D7         |D6  |D5  |D4  |D3  |D2  |D1  |D0  |
|---------------|-----------------|-----------|------------|----|----|----|----|----|----|----|
|Mask Register  | 0x06            | Read      | INT Status*|Port|Port|Port|Port|Port|Port|Port|
|               |                 |           |            |30  |29  |28  |27  |26  |25  |24  |
|               |                 |Write      | Unchanged  |mask|mask|mask|mask|mask|mask|mask|

*INT is automatically cleared after it is read.

*/
#define MAX6956_RA_TRANSITION_DETECT	0x06

/**
Table 16. Display Test Register
===============================

                 |ADDR |
MODE             |CODE |D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0
:-------------  :|:---:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:
Normal Operation |0x07 |x  |x  |x  |x  |x  |x  |x  |0
Display Test Mode|0x07 |x  |x  |x  |x  |x  |x  |x  |1

*/
#define MAX6956_RA_DISPLAY_TEST			0x07

/**
Table 1. Port Configuration Map
===============================

REGISTER                                 |ADDRESS|D7 D6|D5 D4|D3 D2|D1 D0
-----------------------------------------|-------|-----|-----|-----|-----
Port Configuration for P7, P6, P5, P4    |  0x09 |  P7 |  P6 |  P5 |  P4
Port Configuration for P11, P10, P9, P8  |  0x0A | P11 | P10 |  P9 |  P8
Port Configuration for P15, P14, P13, P12|  0x0B | P15 | P14 | P13 | P12
Port Configuration for P19, P18, P17, P16|  0x0C | P19 | P18 | P17 | P16
Port Configuration for P23, P22, P21, P20|  0x0D | P23 | P22 | P21 | P20
Port Configuration for P27, P26, P25, P24|  0x0E | P27 | P26 | P25 | P24
Port Configuration for P31, P30, P29, P28|  0x0F | P31 | P30 | P29 | P28

Table 2. P4-P31 configuration bit pairs
=======================================

<pre>
    00 - Output, LED Segment Driver
             Controlled  using port registers 0x20-0x5F. 
             0 = High impedance
             1 = Open-drain current sink, with sink current (up to 24mA) 
                 determined by the appropriate current register
    01 - Output, GPIO Output
             Controlled  using port registers 0x20-0x5F. 
             0 = Active-low logic output
             1 = Active-high logic output
    10 - Input, GPIO Input Without Pullup
             Read port registers 0x20-0x5F.
             Schmitt logic input 
    11 - Input, GPIO Input with Pullup
             Read port registers 0x20-0x5F.
             Schmitt logic input with pullup
</pre>

*/
#define MAX6956_RA_CONFIG_P7P6P5P4				0x09
#define MAX6956_RA_CONFIG_P11P10P9P8			0x0A
#define MAX6956_RA_CONFIG_P15P14P13P12			0x0B
#define MAX6956_RA_CONFIG_P19P18P17P16			0x0C
#define MAX6956_RA_CONFIG_P23P22P21P20			0x0D
#define MAX6956_RA_CONFIG_P27P26P25P24			0x0E
#define MAX6956_RA_CONFIG_P31P30P29P28			0x0F

/**
Constant-current limit for each digit is 
individually controlled by the settings in the 
Current054 through Current1FE registers

Table 12. Individual Port Current Registers
===========================================

                    | ADDRESS |            |
FUNCTION            |CODE(HEX)|D7 D6 D5 D4 | D3 D2 D1 D0
--------------------|---------|------------|--------------
Current054 register |  0x12   | Segment 5  |  Segment 4
Current076 register |  0x13   | Segment 7  |  Segment 6
Current098 register |  0x14   | Segment 9  |  Segment 8
Current0BA register |  0x15   | Segment 11 |  Segment 10
Current0DC register |  0x16   | Segment 13 |  Segment 12
Current0FE register |  0x17   | Segment 15 |  Segment 14
Current110 register |  0x18   | Segment 17 |  Segment 16
Current132 register |  0x19   | Segment 19 |  Segment 18
Current154 register |  0x1A   | Segment 21 |  Segment 20
Current176 register |  0x1B   | Segment 23 |  Segment 22
Current198 register |  0x1C   | Segment 25 |  Segment 24
Current1BA register |  0x1D   | Segment 27 |  Segment 26
Current1DC register |  0x1E   | Segment 29 |  Segment 28
Current1FE register |  0x1F   | Segment 31 |  Segment 30

Register data is 0-Fx0-F for 1-16(0-F) brightness levels.
To completely blank an output turn it off (0) using port
configuration registers. 
*/
#define MAX6956_RA_CURRENT_0xP5P4		0x12
#define MAX6956_RA_CURRENT_0xP7P6		0x13
#define MAX6956_RA_CURRENT_0xP9P8		0x14
#define MAX6956_RA_CURRENT_0xP11P10		0x15
#define MAX6956_RA_CURRENT_0xP13P12		0x16
#define MAX6956_RA_CURRENT_0xP15P14		0x17
#define MAX6956_RA_CURRENT_0xP17P16		0x18
#define MAX6956_RA_CURRENT_0xP19P18		0x19
#define MAX6956_RA_CURRENT_0xP21P20		0x1A
#define MAX6956_RA_CURRENT_0xP23P22		0x1B
#define MAX6956_RA_CURRENT_0xP25P24		0x1C
#define MAX6956_RA_CURRENT_0xP27P26		0x1D
#define MAX6956_RA_CURRENT_0xP29P28		0x1E
#define MAX6956_RA_CURRENT_0xP31P30		0x1F

/**
Individiual Port Registers 
==========================

    data bit D0; D7-D1 read as 0
*/
#define MAX6956_RA_PORT0				0x20  ///< (virtual port, no action)
#define MAX6956_RA_PORT1				0x21  ///< (virtual port, no action)
#define MAX6956_RA_PORT2				0x22  ///< (virtual port, no action)
#define MAX6956_RA_PORT3				0x23  ///< (virtual port, no action)

#define MAX6956_RA_PORT4				0x24
#define MAX6956_RA_PORT5				0x25
#define MAX6956_RA_PORT6				0x26
#define MAX6956_RA_PORT7				0x27
#define MAX6956_RA_PORT8				0x28
#define MAX6956_RA_PORT9				0x29
#define MAX6956_RA_PORT10				0x2A
#define MAX6956_RA_PORT11				0x2B

#define MAX6956_RA_PORT12				0x2C
#define MAX6956_RA_PORT13				0x2D
#define MAX6956_RA_PORT14				0x2E
#define MAX6956_RA_PORT15				0x2F
#define MAX6956_RA_PORT16				0x30
#define MAX6956_RA_PORT17				0x31
#define MAX6956_RA_PORT18				0x32
#define MAX6956_RA_PORT19				0x33

#define MAX6956_RA_PORT20				0x34
#define MAX6956_RA_PORT21				0x35
#define MAX6956_RA_PORT22				0x36
#define MAX6956_RA_PORT23				0x37
#define MAX6956_RA_PORT24				0x38
#define MAX6956_RA_PORT25				0x39
#define MAX6956_RA_PORT26				0x3A
#define MAX6956_RA_PORT27				0x3B

#define MAX6956_RA_PORT28				0x3C
#define MAX6956_RA_PORT29				0x3D
#define MAX6956_RA_PORT30				0x3E
#define MAX6956_RA_PORT31				0x3F
/*
     Lower port groups
*/
#define MAX6956_RA_PORTS4_7				0x40 ///< data bits D0-D3; D4-D7 read as 0
#define MAX6956_RA_PORTS4_8				0x41 ///< data bits D0-D4; D5-D7 read as 0
#define MAX6956_RA_PORTS4_9				0x42 ///< data bits D0-D5; D6-D7 read as 0
#define MAX6956_RA_PORTS4_10			0x43 ///< data bits D0-D6; D7 reads as 0

/*
     8 port registers. Data bits D0-D7
*/
#define MAX6956_RA_PORTS4_11			0x44
#define MAX6956_RA_PORTS5_12			0x45
#define MAX6956_RA_PORTS6_13			0x46
#define MAX6956_RA_PORTS7_14			0x47
#define MAX6956_RA_PORTS8_15			0x48
#define MAX6956_RA_PORTS9_16			0x49
#define MAX6956_RA_PORTS10_17			0x4A
#define MAX6956_RA_PORTS11_18			0x4B
#define MAX6956_RA_PORTS12_19			0x4C
#define MAX6956_RA_PORTS13_20			0x4D
#define MAX6956_RA_PORTS14_21			0x4E
#define MAX6956_RA_PORTS15_22			0x4F
#define MAX6956_RA_PORTS16_23			0x50
#define MAX6956_RA_PORTS17_24			0x51
#define MAX6956_RA_PORTS18_25			0x52
#define MAX6956_RA_PORTS19_26			0x53
#define MAX6956_RA_PORTS20_27			0x54
#define MAX6956_RA_PORTS21_28			0x55
#define MAX6956_RA_PORTS22_29			0x56
#define MAX6956_RA_PORTS23_30			0x57
#define MAX6956_RA_PORTS24_31			0x58
/*
	 Upper port  groups
*/
#define MAX6956_RA_PORTS25_31			0x59 ///< data bits D0-D6; D7 reads as 0
#define MAX6956_RA_PORTS26_31			0x5A ///< data bits D0-D5; D6-D7 read as 0
#define MAX6956_RA_PORTS27_31			0x5B ///< data bits D0-D4; D5-D7 read as 0
#define MAX6956_RA_PORTS28_31			0x5C ///< data bits D0-D3; D4-D7 read as 0
#define MAX6956_RA_PORTS29_31			0x5D ///< data bits D0-D2; D3-D7 read as 0
#define MAX6956_RA_PORTS30_31			0x5E ///< data bits D0-D1; D2-D7 read as 0
#define MAX6956_RA_PORTS31_31			0x5F ///< data bit D0; D1-D7 read as 0. Port 31 only.

//=========Config bits=========

#define MAX6956_CONFIG_POWER_BIT			0  ///< 0 = Shutdown, 1 = Normal operation
#define MAX6956_CONFIG_GLOBAL_CURRENT_BIT	6  ///< 0 = Global control, 1 = Individual control 
#define MAX6956_CONFIG_TRANSITION_BIT		7  ///< 0 = Disabled, 1 = Enabled

#define MAX6956_GLOBAL_CURRENT_BIT	        0  ///< Global current start bit
#define MAX6956_GLOBAL_CURRENT_LENGTH	    4  ///< nybble long

#define MAX6956_PORT12_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT12_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT13_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT13_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT14_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT14_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT15_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT15_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT16_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT16_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT17_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT17_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT18_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT18_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT19_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT19_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT23_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT23_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT21_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT21_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT22_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT22_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT23_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT23_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT24_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT24_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT25_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT25_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT26_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT26_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT27_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT27_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT28_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT28_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT29_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT29_CURRENT_LENGTH	    4  ///< nybble long 
#define MAX6956_PORT33_CURRENT_BIT	        3  ///< LSNybble
#define MAX6956_PORT33_CURRENT_LENGTH	    4  ///< nybble long
#define MAX6956_PORT31_CURRENT_BIT	        7  ///< MSNybble
#define MAX6956_PORT31_CURRENT_LENGTH	    4  ///< nybble long 

#define MAX6956_DISPLAY_TEST_BIT			0  ///< 0 = Normal operation, 1 = Display test

#define MAX6956_TRANSITION_STATUS_BIT		7  ///< INT Status, INT is automatically cleared after it is read.
#define MAX6956_TRANSITION_MASK_BIT			0  ///< Ports 24-30
#define MAX6956_TRANSITION_MASK_LENGTH		7
#define MAX6956_TRANSITION_MASK_PORT30_BIT	6
#define MAX6956_TRANSITION_MASK_PORT29_BIT	5
#define MAX6956_TRANSITION_MASK_PORT28_BIT	4
#define MAX6956_TRANSITION_MASK_PORT27_BIT	3
#define MAX6956_TRANSITION_MASK_PORT26_BIT	2
#define MAX6956_TRANSITION_MASK_PORT25_BIT	1
#define MAX6956_TRANSITION_MASK_PORT24_BIT	0

//=========Values=========

#define MAX6956_OUTPUT_LED			    0x00
#define MAX6956_OUTPUT_GPIO			    0x01
#define MAX6956_INPUT_WO_PULL		    0x02
#define MAX6956_INPUT_W_PULL   		    0x03

#define MAX6956_OFF             	    0x00
#define MAX6956_ON              	    0x01

#define MAX6956_CURRENT_0               0x00
#define MAX6956_CURRENT_1               0x01
#define MAX6956_CURRENT_2               0x02
#define MAX6956_CURRENT_3               0x03
#define MAX6956_CURRENT_4               0x04
#define MAX6956_CURRENT_5               0x05
#define MAX6956_CURRENT_6               0x06
#define MAX6956_CURRENT_7               0x07
#define MAX6956_CURRENT_8               0x08
#define MAX6956_CURRENT_9               0x09
#define MAX6956_CURRENT_10              0x0A
#define MAX6956_CURRENT_11              0x0B
#define MAX6956_CURRENT_12              0x0C
#define MAX6956_CURRENT_13              0x0D
#define MAX6956_CURRENT_14              0x0E
#define MAX6956_CURRENT_15              0x0F

/*!
A library for controlling the MAX6956 using i2C. 
*/
class MAX6956 {
    public:
        MAX6956();
        MAX6956(uint8_t address);

        void initialize();
        bool testConnection();

        void reset();

        // Config register
        uint8_t getConfigReg();
        void setConfigReg(uint8_t config);

        bool getPower();
        void togglePower();
        void setPower(bool power);

        bool getEnableIndividualCurrent();
        void setEnableIndividualCurrent(bool global);

        bool getEnableTransitionDetection();
        void setEnableTransitionDetection(bool transition);

        // Global current
        uint8_t getGlobalCurrent();
        void setGlobalCurrent(uint8_t current);

        // Test mode
        bool getTestMode();
        void setTestMode(bool test);

        // Input mask 
        uint8_t getInputMask();
        void setInputMask(uint8_t mask);

        // Port config
        void configPort(uint8_t port, uint8_t portConfig);
        void configPorts(uint8_t lower, uint8_t upper, uint8_t portConfig);
        void configAllPorts(uint8_t portConfig);
        
        void enableAllPorts();
        void disableAllPorts();
        void toggleLEDs();
        void setPort(uint8_t port, bool enabled); ///< enabled = true, disabled = false
        uint8_t getPort(uint8_t port); ///< Value of port, 0 or 1
        uint8_t getTransitionPorts(); ///< Reads the value of the transition detection ports (24-30) all at once.
        
        void setPortLevel(uint8_t port, uint8_t power); ///< 0 = off, 1-15 brightness levels.
        void setPortCurrent(uint8_t port, uint8_t power); ///< 0-15 brightness levels. 
        void setAllPortsCurrent(uint8_t power); ///< 0-15, 0 = min brightness (not off) 15 = max
        
        /**
            Array that mirrors the configuration of all the ports.
    
            P12 = portsConfig[0][1-0]
            P13 = portsConfig[0][3-2]
            P14 = portsConfig[0][5-4]
            P15 = portsConfig[0][7-6]
            P16 = portsConfig[1][1-0]
            P17 = portsConfig[1][3-2]
            P18 = portsConfig[1][5-4]
            P19 = portsConfig[1][7-6]
            P20 = portsConfig[2][1-0]
            P21 = portsConfig[2][3-2]
            P22 = portsConfig[2][5-4]
            P23 = portsConfig[2][7-6]
            P24 = portsConfig[3][1-0]
            P25 = portsConfig[3][3-2]
            P26 = portsConfig[3][5-4]
            P27 = portsConfig[3][7-6]
            P28 = portsConfig[4][1-0]
            P29 = portsConfig[4][3-2]
            P30 = portsConfig[4][5-4]
            P31 = portsConfig[4][7-6]
        */
        uint8_t portsConfig[5];
        /**
            Array that mirrors the on/off status of all the ports.
            
            P12 = portsStatus[0][0]
            P13 = portsStatus[0][1]
            P14 = portsStatus[0][2]
            P15 = portsStatus[0][3]
            P16 = portsStatus[0][4]
            P17 = portsStatus[0][5]
            P18 = portsStatus[0][6]
            P19 = portsStatus[0][7]
            P20 = portsStatus[1][0]
            P21 = portsStatus[1][1]
            P22 = portsStatus[1][2]
            P23 = portsStatus[1][3]
            P24 = portsStatus[1][4]
            P25 = portsStatus[1][5]
            P26 = portsStatus[1][6]
            P27 = portsStatus[1][7]
            P28 = portsStatus[2][0]
            P29 = portsStatus[2][1]
            P30 = portsStatus[2][2]
            P31 = portsStatus[2][3]
        */
        uint8_t portsStatus[3];

    private:
        uint8_t devAddr; ///< Holds the current device address
        uint8_t buffer[1]; ///< Buffer for reading data from the device
        uint8_t portConfig; ///< Holder for port config bit pair
        uint8_t portCurrentRA; ///< Holder for port current register
        uint8_t portCurrentBit; ///< Holder for the current bit offset
        uint8_t psArrayIndex; ///< array index
        uint8_t psBitPosition; ///< bit position
};

#endif /* _MAX6956_H_ */
