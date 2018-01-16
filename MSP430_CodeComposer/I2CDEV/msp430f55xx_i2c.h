/*
 * usci_i2c.h
 *
 *  Created on: Jul 11, 2012
 *      Author: zoellner
 */

/*
 *   Modified by:   Jihoon Lee
 *          Date:   Dec 26, 2017
 *     Changelog:   Added F55xx implementation
 *
 */

#ifndef I2C_H_
#define I2C_H_


#ifdef __cplusplus  // TBD: necessary?
extern "C" {
#endif


#define USCI_I2C			1			// select which USCI module to use

#define I2C_MASTER_ADDR		0x30        // MSP430 address
#define I2C_PRESCALE		0x05		// clk divider value. Source clock in msp430f55xx = ~1Mhz

#if USCI_I2C==0                                       // use USCIB0 for I2C
#define I2C_PxSEL P3SEL
#define SDA_PIN BIT0                                  // msp430f55xx UCB0SDA pin
#define SCL_PIN BIT1                                  // msp430f55xx UCB0SCL pin
#elif USCI_I2C==1
#define I2C_PxSEL P4SEL                               // use USCIB1 for I2C
#define SDA_PIN BIT1                                  // msp430f55xx UCB1SDA pin
#define SCL_PIN BIT2                                  // msp430f55xx UCB1SCL pin
#endif

#define I2C_RX0_BUFF_SIZE	20
#define I2C_TX0_BUFF_SIZE	20

//*****************************************************************************
//
//The following are values that can be passed to the I2C_setMode() API
//as the mode parameter.
//
//*****************************************************************************
#define I2C_TRANSMIT_MODE       UCTR
#define I2C_RECEIVE_MODE        0x00
//*****************************************************************************
//
//The following are values that can be passed to the I2C_enableInterrupt(),
//I2C_disableInterrupt(), I2C_clearInterrupt(), I2C_getInterruptStatus(), API
//as the mask parameter.
//
//*****************************************************************************
//#define I2C_NAK_INTERRUPT                UCNACKIE
//#define I2C_ARBITRATIONLOST_INTERRUPT    UCALIE
//#define I2C_STOP_INTERRUPT               UCSTPIE
//#define I2C_START_INTERRUPT              UCSTTIE
#define I2C_TRANSMIT_INTERRUPT           UCTXIE
#define I2C_RECEIVE_INTERRUPT            UCRXIE


//*****************************************************************************
//
//Prototypes for the APIs.
//
//*****************************************************************************
extern void I2C_masterInit ();
//extern void I2C_slaveInit (unsigned char slaveAddress);
extern void I2C_enable ();
extern void I2C_disable ();
extern void I2C_setSlaveAddress (unsigned char slaveAddress);
extern void I2C_setMode (unsigned char mode);
//extern void I2C_slaveDataPut (unsigned char transmitData);
//extern unsigned char I2C_slaveDataGet ();
extern unsigned char I2C_isBusBusy ();
extern void I2C_enableInterrupt (unsigned char mask);
extern void I2C_disableInterrupt (unsigned char mask);
extern void I2C_clearInterruptFlag (unsigned char mask);
//extern unsigned char I2C_getInterruptStatus (unsigned char mask);
extern void I2C_masterSendSingleByte (unsigned char txData);
//extern unsigned short I2C_masterSendSingleByteWithTimeout (unsigned char txData,unsigned long timeout);
extern void I2C_masterMultiByteSendStart (unsigned char txData);
//extern unsigned short I2C_masterMultiByteSendStartWithTimeout (unsigned char txData, unsigned long timeout);
extern void I2C_masterMultiByteSendNext (unsigned char txData);
//extern unsigned short I2C_masterMultiByteSendNextWithTimeout (unsigned char txData, unsigned long timeout);
extern void I2C_masterMultiByteSendFinish (unsigned char txData);
//extern unsigned short I2C_masterMultiByteReceiveFinishWithTimeout (unsigned char *txData,unsigned long timeout);
extern void I2C_masterMultiByteSendStop ();
//extern unsigned short I2C_masterMultiByteSendStopWithTimeout (unsigned long timeout);
extern void I2C_masterMultiByteReceiveStart ();
extern unsigned char I2C_masterMultiByteReceiveNext ();
extern unsigned char I2C_masterMultiByteReceiveFinish ();
extern void I2C_masterMultiByteReceiveStop ();
extern void I2C_masterSingleReceiveStart ();
//extern unsigned short I2C_masterSingleReceiveStartWithTimeout (unsigned long timeout);
extern unsigned char I2C_masterSingleReceive ();
//extern unsigned long I2C_getReceiveBufferAddressForDMA ();
//extern unsigned long I2C_getTransmitBufferAddressForDMA ();
//extern unsigned char I2C_masterIsSTOPSent ();
//extern void I2C_masterSendStart ();




#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */
