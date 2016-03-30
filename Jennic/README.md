# I2C Device Library for Jennic

## I2Cdev
We use JenOS peripheral libraries.

## Devices
Currently only MPU6050 is supported without DMP functions. There is no address set dynamically so currently only one MPU6050 can be read (or address must be changed manually each time before reading/writing)

Adding more functions and devices should be straighforward after reading the source code.

## Licence
I2Cdev device library code is placed under the MIT license.

_Copyright (c) 2011 Jeff Rowberg. Copyright (c) 2014 Marton Sebok. Copyright (c) 2015 Grégoire Surrel._
