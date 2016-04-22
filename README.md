# I<sup>2</sup>C Device Library Plus

This is a fork of the I2C Device Library by Jeff Rowberg written in pure C.

The I2C Device Library plus is a uniform collection of functions and structures meant to provide hardware and software abstraction of the I2C to be used across multiple devices (Arduino, Raspberry Pi, etc.). It is meant to be fully compliant with the I2C standard and its derivatives (Fast Mode, Fast Mode Plus, High Speed and Ultra Fast Mode) and extensible enough to be used by all I2C devices on the market.

This library is written only with C code and functions. There are no C++ constructs in this library, which means it can be used with your device even if it does not support C++.

Code in this library is to be documented with Doxygen and available on the project's website.

This library is looking to improve upon i2cdevlib by using only pure C code instead of mixing C-style and C++ code. Also, it is looking to improve upon i2cdevlib by supporting the whole I2C standard and the whole range of existing I2C-related standards supported by NXP.

This library is licensed under the FSF's Lesser GNU Public License v3. View the LICENSE file for more information.
