// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// 11/28/2014 by Marton Sebok <sebokmarton@gmail.com>
//
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2014-11-28 - ported to PIC18 peripheral library from Arduino code

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg
Copyright (c) 2014 Marton Sebok

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

/*
 * The project can be run without modification on Microchip's 8-bit Wireless
 * Development Kit. Please set the type of your IC in project properties/Conf/
 * Device and change the code as needed.
 */

// PLL Prescaler Selection bits (Divide by 2 (8 MHz oscillator input))
#pragma config PLLDIV = 2

// Extended Instruction Set (Disabled)
#pragma config XINST = OFF

// CPU System Clock Postscaler (CPU system clock divide by 2)
#pragma config CPUDIV = OSC2_PLL2

// Oscillator (HS+PLL, USB-HS+PLL)
#pragma config OSC = HSPLL

#define USE_OR_MASKS
#define _XTAL_FREQ 24000000

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <plib/usart.h>

/*
 * You must add the I2Cdev and MPU6050 directories as include directories in
 * project properties/Conf/General options/XC8 global option/XC8 compiler/
 * Preprocessing and messages/Include directories.
 */
#include "I2Cdev.h"
#include "MPU6050.h"

#define LED_PIN LATBbits.LATB1

/**
 * Write one char to USART
 * @param data
 */
void putch(char data) {
    while (Busy1USART());
    Write1USART(data);
}

/**
 * Main function
 */
int main() {
    int16_t ax, ay, az, gx, gy, gz;
    
    // Initialize system
    // PLL (24 MHz)
    OSCTUNEbits.PLLEN = 1;
    __delay_ms(25);
    __delay_ms(25);

    // LED
    LED_PIN = 0;
    TRISBbits.TRISB1 = 0;

    // Serial (115200 kbps)
    Open1USART(USART_TX_INT_OFF | USART_RX_INT_OFF | USART_ASYNCH_MODE |
        USART_EIGHT_BIT | USART_CONT_RX | USART_BRGH_HIGH, 12);
    baud1USART(BAUD_8_BIT_RATE | BAUD_AUTO_OFF);

    // I2C (400 kHz)
    OpenI2C(MASTER, SLEW_OFF);
    SSP1ADD = 14;

    // MPU6050
    MPU6050(MPU6050_ADDRESS_AD0_LOW);

    // Initialize device
    printf("Initializing I2C devices...\r\n");
    MPU6050_initialize();

    // Verify connection
    printf("Testing device connections...\r\n");
    printf(MPU6050_testConnection() ? "MPU6050 connection successful\r\n" :
        "MPU6050 connection failed\r\n");

    printf("Reading internal sensor offsets...\r\n");
    printf("%d\t", MPU6050_getXAccelOffset());
    printf("%d\t", MPU6050_getYAccelOffset());
    printf("%d\t", MPU6050_getZAccelOffset());
    printf("%d\t", MPU6050_getXGyroOffset());
    printf("%d\t", MPU6050_getYGyroOffset());
    printf("%d\t\n", MPU6050_getZGyroOffset());

    while (true) {
        // Read raw accel/gyro measurements from device
        MPU6050_getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        // Display tab-separated accel/gyro x/y/z values
        printf("a/g:\t%d\t%d\t%d\t%d\t%d\t%d\r\n", ax, ay, az, gx, gy, gz);

        // Blink LED to indicate activity
        LED_PIN ^= 1;

        __delay_ms(25);
        __delay_ms(25);
    }
}
