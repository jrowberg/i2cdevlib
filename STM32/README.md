# I2C Device Library for STM32

## I2Cdev
For implementing I2Cdev [Keil MDK Pack](https://www.keil.com/dd2/pack/) is used. It has many advantages comparing to HAL/SPL. The most
crucial one is that of compatibility: using it makes I2Cdev compatible with any device listed on MDK Pack software page. All you need is to download pack for your device, enable your driver and specify used peripherial in I2Cdev.h.

## Devices
Supported devices are MPU6050 without DMP and HMC5883. Actually I did not find any differences in registers between HMC5883 and HMC5983 therefore the latter might work as well.

Porting other devices should be easy and any help is appreciated.

## Licence
I2Cdev device library code is placed under the MIT license.

_Copyright (c) 2011 Jeff Rowberg. Copyright (c) 2017 Kamnev Yuriy._
