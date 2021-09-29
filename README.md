# I2C Device Library

The I2C Device Library (i2cdevlib) is a collection of mostly uniform and well-documented classes to provide simple and intuitive interfaces to I2C devices. Each device is built to make use of the generic "I2Cdev" class, which abstracts the I2C bit- and byte-level communication away from each specific device class, making it easy to keep the device class clean while providing a simple way to modify just one class to port the I2C communication code onto different platforms (Arduino, PIC, MSP430, Jennic, simple bit-banging, etc.). Device classes are designed to provide complete coverage of all functionality described by each device's documentation, plus any generic convenience functions that are helpful.

There are examples in many of the classes that demonstrate basic usage patterns. The I2Cdev class is built to be used statically, reducing the memory requirement if you have multiple I2C devices in your project. Only one instance of the I2Cdev class is required. Recent additions as of late 2021 have also made it possible to pass in non-default `Wire` objects (in the Arduino environment) to allow using multiple I2C transceivers at the same time, specifically because of the number of people who wanted to use up to four MPU-6050 IMUs without I2C mux ICs involved.

Documentation for each class is created using Doxygen-style comments placed in each class definition file, based on the information available in each device's datasheet. This documentation is available in HTML format on the i2cdevlib.com website, which also holds helpful information for most of the classes present here on the repository.

## Installation

Due to my...ahem...unfortunate ignorance way back when I first created this project, the entire codebase (all platforms, cores, and device libraries) are all inside of this one giant repository. That means there's no easy IDE integration the way most libraries work in the Arduino world and elsewhere. Instead, do the following:

1. Clone or [download a .zip archive](https://github.com/jrowberg/i2cdevlib/archive/refs/heads/master.zip) of the repo
2. Move or copy the relevant core and device drivers into your project tree or library subfolder<br />(For Arduino, this means the `/Arduino/I2Cdev` and `/Arduino/MPU6050` folders, for example)
3. Rescan libraries or restart your IDE if necessary

For both usage and development, I've found that it's best to clone using the git client of your choice, and then create symlinks as needed from the master repository sources into your development location(s). This is usually more intuitive for people who use Linux, but it can be done in Windows as well using the `mklink /D` command. See [this page](https://www.howtogeek.com/howto/16226/complete-guide-to-symbolic-links-symlinks-on-windows-or-linux/) for a set of Windows-specific instructions with screenshots.

## Usage

Exact usage varies from device to device, but most (especially the more popular ones) include example projects demonstrating the basics. Refer to those examples for the best material currently available.

## Contributing

Want a library for a device that isn't up on the repository? You can either request it in the discussion area for this repo on Github, or fork the code and write it yourself.

Realistically, Option B is more reliable. Try to mimic the structure and code conventions of the existing codebase as much as possible. If you go this route, please use the following approach:

1. Fork the repository to your own user
2. Create a new branch specific to your new code
3. Write, test, and commit your new code
4. Submit a pull request from your branch back to the original source

I and a few others will review the pull request and comment as needed, and then hopefully merge it.

<hr />

***Note: additional details about this project can be found at https://www.i2cdevlib.com***

***Another note: this project has a fledgling successor that aims to [address all of its shortcomings](https://community.perilib.io/t/perilib-i2cdevlib-reborn/15), which can be found at https://github.com/perilib***
