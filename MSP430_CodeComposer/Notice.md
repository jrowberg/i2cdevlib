MSP430 I2C library. Upgraded previous MSP430 repository by Zollenier, which only supported MSP430x261x family. (https://github.com/jrowberg/i2cdevlib/tree/master/MSP430/I2Cdev)
The design follows TI Launchpads. Redefine clock dividers if you are using custom PCB with different oscillators from Launchpads.

Updates:
  - MSP430F55xx family support added.
  - MSP430x261x devices reuse previous code from old MSP430 directory.
  - Now fully implemented I2CDEV files.
Usage:
  - include I2CDEV.h, I2CDEV.c, and device library you are using in your project. (i.e include msp430f55xx_i2c.h and msp430f55xx_i2c.c if you are using an f55xx device)
  - redefine DEVICE macro in I2CDEV to that of device you are using.

Tests:
  - Basic functionalities with ADS1115 worked successfully. (save for multiple words)
