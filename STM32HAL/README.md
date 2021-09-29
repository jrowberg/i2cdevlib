# I2C Device Library for STM32

## Devices
Currently MPU6050 (with DMP), HMC5883L and BMP180/BMP085 are ported (GY-87 board). ADXL345 is coming soon.
Adding more functions and devices should be straighforward after reading the source code of the driver.
Porting drivers should be as simple, as renaming public methods from *DRIVER::func()* to *DRIVER_func()* and moving private fields and functions to *.c file

## BMP180 Example for STM32F429I-DISCO board
```C
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "I2Cdev.h"
#include "BMP085.h"

I2C_HandleTypeDef hi2c3;

int main(void)
{
    SystemInit();
    HAL_Init();
    
    GPIO_InitTypeDef GPIO_InitStruct;

    /**I2C3 GPIO Configuration
    PC9     ------> I2C3_SDA
    PA8     ------> I2C3_SCL
    */

    __GPIOA_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __I2C3_CLK_ENABLE();

    hi2c3.Instance = I2C3;
    hi2c3.Init.ClockSpeed = 400000;
    hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c3.Init.OwnAddress1 = 0x10;
    hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hi2c3.Init.OwnAddress2 = 0x11;
    hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;

    HAL_I2C_Init(&hi2c3);

    I2Cdev_init(&hi2c3); // init of i2cdevlib.  
    // You can select other i2c device anytime and 
    // call the same driver functions on other sensors

    while(!BMP085_testConnection()) ;

    BMP085_initialize();
    char buf[128];
    while (1)
    {
        BMP085_setControl(BMP085_MODE_TEMPERATURE);
        HAL_Delay(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_TEMPERATURE));
        float t = BMP085_getTemperatureC();

        BMP085_setControl(BMP085_MODE_PRESSURE_3);
        HAL_Delay(BMP085_getMeasureDelayMilliseconds(BMP085_MODE_PRESSURE_3));
        float p = BMP085_getPressure();

        float a = BMP085_getAltitude(p, 101325);
    }
}

void SysTick_Handler()
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}
```

## Licence
I2Cdev device library code is placed under the MIT license.

_Copyright (c) 2011 Jeff Rowberg. Copyright (c) 2015 Andrey Voloshin._
