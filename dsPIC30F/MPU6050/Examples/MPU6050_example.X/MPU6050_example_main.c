/*
 * File:   LEDMain.c
 * Author: tommycc
 *
 * Created on March 1, 2017, 3:49 PM
 */


// DSPIC30F4013 Configuration Bit Settings

// 'C' source line config statements

// FOSC
#pragma config FOSFPR = XT_PLL8        // Oscillator (XT w/PLL 8x)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_OFF         // PBOR Enable (Disabled)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include<xc.h>
#include<uart.h>
#include<stdio.h>
#include<i2c.h>
#include"MPU6050.h"
#include"I2Cdev.h"
/* Received data is stored in array Buf  */
char Buf[80];
char* Receivedddata = Buf;
/* This is UART1 transmit ISR */
void __attribute__((__interrupt__)) _U2TXInterrupt(void)
{  
   IFS1bits.U2TXIF = 0;
} 
/* This is UART1 receive ISR */
//void __attribute__((__interrupt__)) _U1RXInterrupt(void)
//{
//    IFS0bits.U1RXIF = 0;
/* Read the receive buffer till atleast one or more character can be read */ 
//    while( DataRdyUART1())
//    {
//        ( *(Receivedddata)++) = ReadUART1();
//    } 
//} 

int main(void)
{
    TRISCbits.TRISC14 = 0;
    LATCbits.LATC14 = 1;
            
    while(1){
    /* Data to be transmitted using UART communication module */
        //TRISFbits.TRISF3 = 0;
        //PORTFbits.RF3 = 0;
        //delay_ms(10);
        //PORTFbits.RF3 = 1;
        TRISFbits.TRISF3 = 1;
    char Buffer[80];
    //char 
    /* Holds the value of baud register   */
    unsigned int baudvalue;   
    /* Holds the value of uart config reg */
    unsigned int U2MODEvalue;
    /* Holds the information regarding uart
    TX & RX interrupt modes */   
    unsigned int U2STAvalue;
    CloseI2C();
    /* Turn off UART1module */
    CloseUART2();
    /* Configure uart1 transmit interrupt */
    ConfigIntUART2(  UART_TX_INT_DIS & UART_TX_INT_PR2);
    /* Configure UART1 module to transmit 8 bit data with one stopbit. Also Enable loopback mode  */
    baudvalue = 129;  //9600
    U2MODEvalue = UART_EN & UART_IDLE_CON & 
                  UART_DIS_WAKE & UART_DIS_LOOPBACK  &
                  UART_EN_ABAUD & UART_NO_PAR_8BIT  &
                  UART_1STOPBIT;
    U2STAvalue  = UART_INT_TX_BUF_EMPTY  &  
                  UART_TX_PIN_NORMAL &
                  UART_TX_ENABLE & UART_INT_RX_3_4_FUL &
                  UART_ADR_DETECT_DIS &
                  UART_RX_OVERRUN_CLEAR;
    unsigned int I2C_config1,I2C_config2;
    I2C_config1 = (I2C_ON & I2C_IDLE_CON & I2C_CLK_REL &
             I2C_IPMI_DIS & I2C_7BIT_ADD &
             I2C_SLW_EN & I2C_SM_DIS &
             I2C_GCALL_DIS & I2C_STR_DIS &
             I2C_ACK & I2C_ACK_EN & I2C_RCV_EN &
             I2C_STOP_DIS & I2C_RESTART_EN &
             I2C_START_DIS);
    I2C_config2 = 181;
    ConfigIntI2C(MI2C_INT_OFF & SI2C_INT_OFF);
    OpenI2C(I2C_config1,I2C_config2);
    OpenUART2(U2MODEvalue, U2STAvalue, baudvalue);
    
    MPU6050(MPU6050_ADDRESS_AD0_LOW);
    sprintf(Buf,"Initialize MPU6050\n\0");
    /* Load transmit buffer and transmit the same till null character is encountered */
    putsUART2 ((unsigned int *)Buf);
    /* Wait for  transmission to complete */
    while(BusyUART2());
    
    MPU6050_initialize();

    sprintf(Buf,"Testing device connections...\n\0");
    putsUART2 ((unsigned int *)Buf);
    /* Wait for  transmission to complete */
    while(BusyUART2());
    
    sprintf(Buf,MPU6050_testConnection() ? "MPU6050 connection successful\r\n" :
        "MPU6050 connection failed\r\n");
    putsUART2 ((unsigned int *)Buf);
    /* Wait for  transmission to complete */
    while(BusyUART2());
    
    
    
    
    //CloseUART2();
    
    }
    return 0;
}
