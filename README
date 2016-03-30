Jennic platform added!

============================================================================
Note: for details about this project, please visit: http://www.i2cdevlib.com
============================================================================

The I2C Device Library (i2cdevlib) is a collection of uniform and well-documented classes to provide simple and intuitive interfaces to I2C devices. Each device is built to make use of the generic "I2Cdev" class, which abstracts the I2C bit- and byte-level communication away from each specific device class, making it easy to keep the device class clean while providing a simple way to modify just one class to port the I2C communication code onto different platforms (Arduino, PIC, MSP430, Jennic, simple bit-banging, etc.). Device classes are designed to provide complete coverage of all functionality described by each device's documentation, plus any generic convenience functions that are helpful.

The code is written primarily to support the Arduino/Wiring implementation, but it may be useful in other circumstances. There are multiple I2C/TWI implementations selectable in the I2Cdev.h header file if the default Arduino "Wire.h" is not available or preferable for any reason.

There are examples in many of the classes that demonstrate basic usage patterns. The I2Cdev class is built to be used statically, reducing the memory requirement if you have multiple I2C devices in your project. Only one instance of the I2Cdev class is required.

Documentation for each class is created using Doxygen-style comments placed in each class definition file, based on the information available in each device's datasheet. This documentation is available in HTML format on the i2cdevlib.com website, which also holds helpful information for most of the classes present here on the repository.

To use the library, just place the I2Cdev .cpp/.h or .c/.h source files and any device library .cpp/.h or .c/.h source files in the same folder as your sketch (or a suitable place relative to your project build tool), and include just the device library headers that you need. Arduino users will also need to include <Wire.h> in your main sketch source file. Create a single device object (e.g. "ADXL345 accel();"), place any appropriate init function calls in your setup() routine (e.g. "accel.initialize();"), and off you go! See the example sketches inside many of the device class directories for reference.

Want a library for a device that isn't up on the repository? Request it, or fork the code and contribute! Better yet, send me a device on a breakout board to test the code during development. No guarantees on how fast I can get it done, but I'd love to make this the biggest consistent and well-documented I2C device library around.
