/**
 * @file   i2c.c
 * @Author zoellner
 * @date   Apr 1, 2013
 * @brief  Driver for the I2C Module for MSP430F2618.
 *
 * Based on msp430ware driverlib. Not yet supported are baseAddress configuration,
 * timeout version of send and receive functions, certain interrupts (only TC and RX for now)
 * Created as part of CSK Zero G Test
 */

#include <stdint.h>
#include <msp430.h>
#include "i2c.h"

//*****************************************************************************
//
//Receive buffer for multi byte mode.
//
//*****************************************************************************
//#define MSP430_BUFFER_LENGTH 32
//static uint8_t receiveBuffer[MSP430_BUFFER_LENGTH];
static uint8_t receiveCount = 0;
static uint8_t *receiveBufferPointer;

static uint8_t *transmitData;
static uint8_t transmitCounter = 0;
static uint8_t TXLENGTH;


//*****************************************************************************
//
//! Initializes the I2C Master block.
//!
//!
//! This function initializes operation of the I2C Master block.  Upon
//! successful initialization of the I2C block, this function will have set the
//! bus speed for the master; however I2C module is still disabled till
//! I2C_enable is invoked
//!
//!
//! Modified bits are \b UCMST,UCMODE_3,\b UCSYNC of \b UCBxCTL0 register
//!                   \b UCSSELx, \b UCSWRST, of \b UCBxCTL1 register
//!                   \b UCBxBR0 and \b UCBxBR1 regsiters
//! \return None.
//
//*****************************************************************************

void I2C_masterInit() {
	I2C_PxSEL |= SDA_PIN + SCL_PIN;             // Assign I2C pins to USCI_B0
	UCB1CTL1 = UCSWRST;                         // Enable SW reset
	UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
	UCB1CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
	UCB1BR0 = I2C_PRESCALE;                         // set prescaler
	UCB1BR1 = 0;
}

//*****************************************************************************
//
//! Enables the I2C block.
//!
//! \param baseAddress is the base address of the USCI I2C module.
//!
//! This will enable operation of the I2C block.
//! Modified bits are \b UCSWRST of \b UCBxCTL1 register.
//!
//! \return None.
//
//*****************************************************************************
void I2C_enable ()
{
    //Reset the UCSWRST bit to enable the USCI Module
	UCB1CTL1 &= ~(UCSWRST);
}

//*****************************************************************************
//
//! Disables the I2C block.
//!
//! \param baseAddress is the base address of the USCI I2C module.
//!
//! This will disable operation of the I2C block.
//! Modified bits are \b UCSWRST of \b UCBxCTL1 register.
//!
//! \return None.
//
//*****************************************************************************
void I2C_disable ()
{
    //Set the UCSWRST bit to disable the USCI Module
	UCB1CTL1 |= UCSWRST;
}


//*****************************************************************************
//
//! Sets the address that the I2C Master will place on the bus.
//!
//! \param slaveAddress 7-bit slave address
//!
//! This function will set the address that the I2C Master will place on the
//! bus when initiating a transaction.
//! Modified bits are \b UCSWRST of \b UCBxCTL1 register
//!                   \b UCBxI2CSA register
//!
//! \return None.
//
//*****************************************************************************

void I2C_setSlaveAddress (unsigned char slaveAddress)
{
    //Set the address of the slave with which the master will communicate.
	UCB1I2CSA = slaveAddress;
}

//*****************************************************************************
//
//! Sets the mode of the I2C device
//!
//! \param receive indicates whether module is in transmit/receive mode
//!
//! When the receive parameter is set to I2C_TRANSMIT_MODE, the address will
//! indicate that the I2C module is in receive mode; otherwise, the I2C module
//! is in send mode. Valid values are
//!     \b I2C_TRANSMIT_MODE
//!     \b I2C_RECEIVE_MODE [Default value]
//! Modified bits are \b UCTR of \b UCBxCTL1 register
//!
//! \return None.
//
//*****************************************************************************
void I2C_setMode ( unsigned char mode ) {
    UCB1CTL1 &= ~I2C_TRANSMIT_MODE;
    UCB1CTL1 |= mode;
}

//*****************************************************************************
//
//! Indicates whether or not the I2C bus is busy.
//!
//!
//! This function returns an indication of whether or not the I2C bus is
//! busy.This function checks the status of the bus via UCBBUSY bit in
//! UCBxSTAT register.
//!
//! \return Returns I2C_BUS_BUSY if the I2C Master is busy; otherwise, returns
//! I2C_BUS_NOT_BUSY.
//
//*****************************************************************************
unsigned char I2C_isBusBusy ()
{
    //Return the bus busy status.
    return (UCB1STAT & UCBBUSY);
}



//*****************************************************************************
//
//! Enables individual I2C interrupt sources.
//!
//! \param interruptFlags is the bit mask of the interrupt sources to be enabled.
//!
//! Enables the indicated I2C interrupt sources.  Only the sources that
//! are enabled can be reflected to the processor interrupt; disabled sources
//! have no effect on the processor.
//!
//! The mask parameter is the logical OR of any of the following:
//!
//! - \b I2C_RECEIVE_INTERRUPT -Receive interrupt
//! - \b I2C_TRANSMIT_INTERRUPT - Transmit interrupt
//!
//! Modified registers are UCBxIFG and OFS_UCBxIE.
//!
//! \return None.
//
//*****************************************************************************
void I2C_enableInterrupt ( unsigned char mask )
{

	UC1IFG &= ~(mask);

    //Enable the interrupt masked bit
    UC1IE |= mask;
}

//*****************************************************************************
//
//! Disables individual I2C interrupt sources.
//!
//! \param mask is the bit mask of the interrupt sources to be
//! disabled.
//!
//! Disables the indicated I2C interrupt sources.  Only the sources that
//! are enabled can be reflected to the processor interrupt; disabled sources
//! have no effect on the processor.
//!
//! The mask parameter is the logical OR of any of the following:
//!
//! - \b I2C_RECEIVE_INTERRUPT -Receive interrupt
//! - \b I2C_TRANSMIT_INTERRUPT - Transmit interrupt
//!
//! Modified register is \b UCBxIE.
//!
//! \return None.
//
//*****************************************************************************
void I2C_disableInterrupt ( unsigned char mask )
{
    //Disable the interrupt masked bit
    UC1IE &= ~(mask);
}

//*****************************************************************************
//
//! Clears I2C interrupt sources.
//!
//! \param mask is a bit mask of the interrupt sources to be cleared.
//!
//! The I2C interrupt source is cleared, so that it no longer asserts.
//! The highest interrupt flag is automatically cleared when an interrupt vector
//! generator is used.
//!
//! The mask parameter has the same definition as the mask
//! parameter to I2C_enableInterrupt().
//!
//! Modified register is \b UCBxIFG.
//!
//! \return None.
//
//*****************************************************************************
void I2C_clearInterruptFlag ( unsigned char mask )
{
    //Clear the I2C interrupt source.
    UC1IFG &= ~(mask);
}


//*****************************************************************************
//
//! Does single byte transmission from Master to Slave
//!
//! \param txData is the data byte to be transmitted
//!
//! This function is used by the Master module to send a single byte.
//! This function
//! - Sends START
//! - Transmits the byte to the Slave
//! - Sends STOP
//!
//! Modified registers are \b UCBxIE, \b UCBxCTL1, \b UCBxIFG, \b UCBxTXBUF,
//! \b UCBxIE
//!
//! \return None.
//
//*****************************************************************************
void I2C_masterSendSingleByte ( unsigned char txData   )
{
    //Store current TXIE status
    unsigned char txieStatus = UC1IE & UCB1TXIE;

    //Disable transmit interrupt enable
    UC1IE &= ~(UCB1TXIE);

    //Send start condition.
    UCB1CTL1 |= UCTR + UCTXSTT;

    //Poll for transmit interrupt flag.
    while (!(UC1IFG & UCB1TXIFG)) ; //todo test removing this poll. unnecessary according to http://e2e.ti.com/support/microcontrollers/msp430/f/166/p/255954/897581.aspx

    //Send single byte data.
    UCB1TXBUF = txData;

    //Poll for transmit interrupt flag.
    while (!(UC1IFG & UCB1TXIFG)) ;

    //Send stop condition.
    UCB1CTL1 |= UCTXSTP;

    //Clear transmit interrupt flag before enabling interrupt again
    UC1IFG &= ~(UCB1TXIFG);

    //Reinstate transmit interrupt enable
    UC1IE |= txieStatus;
}


//*****************************************************************************
//
//! Starts multi-byte transmission from Master to Slave
//!
//! \param txData is the first data byte to be transmitted
//!
//! This function is used by the Master module to send a single byte.
//! This function
//! - Sends START
//! - Transmits the first data byte of a multi-byte transmission to the Slave
//!
//! Modified registers are \b UCBxIE, \b UCBxCTL1, \b UCBxIFG, \b UCBxTXBUF,
//! \b UCBxIE
//!
//! \return None.
//
//*****************************************************************************
void I2C_masterMultiByteSendStart ( unsigned char txData )
{
    //Store current TXIE status
    unsigned char txieStatus = UC1IE & UCB1TXIE;

    //Disable transmit interrupt enable
    UC1IE &= ~(UCB1TXIE);

    //Send start condition.
    UCB1CTL1 |= UCTR + UCTXSTT;

    //Poll for transmit interrupt flag.
    while (!(UC1IFG & UCB1TXIFG)) ;

    //Send single byte data.
    UCB1TXBUF = txData;

    //Reinstate transmit interrupt enable
    UC1IE |= txieStatus;
}


//*****************************************************************************
//
//! Continues multi-byte transmission from Master to Slave
//!
//! \param txData is the next data byte to be transmitted
//!
//! This function is used by the Master module continue each byte of a
//! multi-byte trasmission. This function
//! - Transmits each data byte of a multi-byte transmission to the Slave
//!
//! Modified registers are \b UCBxTXBUF
//!
//! \return None.
//
//*****************************************************************************
void I2C_masterMultiByteSendNext ( unsigned char txData )
{
    //If interrupts are not used, poll for flags
    if (!(UC1IE & UCB1TXIE)){
        //Poll for transmit interrupt flag.
        while (!(UC1IFG & UCB1TXIFG)) ;
    }

    //Send single byte data.
    UCB1TXBUF = txData;
}


//*****************************************************************************
//
//! Finishes multi-byte transmission from Master to Slave
//!
//! \param txData is the last data byte to be transmitted in a multi-byte
//! tramsission
//!
//! This function is used by the Master module to send the last byte and STOP.
//! This function
//! - Transmits the last data byte of a multi-byte transmission to the Slave
//! - Sends STOP
//!
//! Modified registers are \b UCBxTXBUF and \b UCBxCTL1.
//!
//! \return None.
//
//*****************************************************************************
void I2C_masterMultiByteSendFinish ( unsigned char txData )
{
    //If interrupts are not used, poll for flags
    if (!(UC1IE & UCB1TXIE)){
        //Poll for transmit interrupt flag.
        while (!(UC1IFG & UCB1TXIFG)) ;
    }

    //Send single byte data.
    UCB1TXBUF = txData;

    //Poll for transmit interrupt flag.
    while (!(UC1IFG & UCB1TXIFG)) ;

    //Send stop condition.
    UCB1CTL1 |= UCTXSTP;
}


//*****************************************************************************
//
//! Send STOP byte at the end of a multi-byte transmission from Master to Slave
//!
//!
//! This function is used by the Master module send STOP at the end of a
//! multi-byte trasmission
//!
//! This function
//! - Send a STOP after current transmission is complete
//!
//! Modified bits are \b UCTXSTP bit of \b UCBxCTL1.
//! \return None.
//
//*****************************************************************************
void I2C_masterMultiByteSendStop ()
{
    //If interrupts are not used, poll for flags
    if (!(UC1IE & UCB1TXIE)){
        //Poll for transmit interrupt flag.
        while (!(UC1IFG & UCB1TXIFG)) ;
    }

    //Send stop condition.
    UCB1CTL1 |= UCTXSTP;
}


//*****************************************************************************
//
//! Starts multi-byte reception at the Master end
//!
//!
//! This function is used by the Master module initiate reception of a single
//! byte. This function
//! - Sends START
//!
//! Modified bits are \b UCTXSTT bit of \b UCBxCTL1.
//! \return None.
//
//*****************************************************************************
void I2C_masterMultiByteReceiveStart ()
{
    //Set USCI in Receive mode
    UCB1CTL1 &= ~UCTR;
    //Send start
    UCB1CTL1 |= UCTXSTT;
}

//*****************************************************************************
//
//! Starts multi-byte reception at the Master end one byte at a time
//!
//!
//! This function is used by the Master module to receive each byte of a
//! multi-byte reception
//! This function reads currently received byte
//!
//! Modified register is \b UCBxRXBUF.
//! \return Received byte at Master end.
//
//*****************************************************************************
unsigned char I2C_masterMultiByteReceiveNext ()
{
//    return (UCB1RXBUF);
	unsigned char result = UCB1RXBUF;
	return result;
}

//*****************************************************************************
//
//! Finishes multi-byte reception at the Master end
//!
//!
//! This function is used by the Master module to initiate completion of a
//! multi-byte reception
//! This function
//! - Receives the current byte and initiates the STOP from Master to Slave
//!
//! Modified bits are \b UCTXSTP bit of \b UCBxCTL1.
//!
//! \return Received byte at Master end.
//
//*****************************************************************************
unsigned char I2C_masterMultiByteReceiveFinish ()
{
    //Send stop condition.
    UCB1CTL1 |= UCTXSTP;

    //Wait for Stop to finish
    while (UCB1CTL1 & UCTXSTP)

    // Wait for RX buffer
    while (!(UC1IFG & UCB1RXIFG)) ;

    //Capture data from receive buffer after setting stop bit due to
    //MSP430 I2C critical timing.
    unsigned char result = UCB1RXBUF;
    return result;
}


//*****************************************************************************
//
//! Sends the STOP at the end of a multi-byte reception at the Master end
//!
//!
//! This function is used by the Master module to initiate STOP
//!
//! Modified bits are UCTXSTP bit of UCBxCTL1.
//!
//! \return None.
//
//*****************************************************************************
void I2C_masterMultiByteReceiveStop ()
{
    //Send stop condition.
    UCB1CTL1 |= UCTXSTP;
}

//*****************************************************************************
//
//! Initiates a single byte Reception at the Master End
//!
//!
//! This function sends a START and STOP immediately to indicate Single byte
//! reception
//!
//! Modified bits are \b UCTXSTT and \b UCTXSTP of \b UCBxCTL1, GIE
//!
//! \return None.
//
//*****************************************************************************
void I2C_masterSingleReceiveStart ()
{
    //local variable to store GIE status
    unsigned int gieStatus;

    //Store current SR register
    gieStatus = __get_SR_register() & GIE;

    //Disable global interrupt
    __disable_interrupt();

	//Set USCI in Receive mode
    UCB1CTL1 &= ~UCTR;

    //Send start condition.
    UCB1CTL1 |= UCTXSTT;

    //Poll for Start bit to complete
    while (UCB1CTL1 & UCTXSTT) ;

    //Send stop condition.
    UCB1CTL1 |= UCTXSTP;

    //Reinstate SR register
    __bis_SR_register(gieStatus);
}


//*****************************************************************************
//
//! Receives a byte that has been sent to the I2C Master Module.
//!
//! \param baseAddress is the base address of the I2C module.
//!
//! This function reads a byte of data from the I2C receive data Register.
//!
//! \return Returns the byte received from by the I2C module, cast as an
//! unsigned char.
//
//*****************************************************************************
unsigned char I2C_masterSingleReceive ()
{
    //Read a byte.
//    return (UCB1RXBUF);
    unsigned char result = UCB1RXBUF;
    return result;
}


////******************************************************************************
////
////This is called from the USCI_B1 interrupt vector service routine.
////
////******************************************************************************
//inline void I2C_Rx_master_ISR()
//{
//	//Decrement RX byte counter
//	receiveCount--;
//
//	if (receiveCount){
//		if (receiveCount == 1){
//			//Initiate end of reception -> Receive byte with NAK
//			*receiveBufferPointer++ = I2C_masterMultiByteReceiveFinish();
//		} else   {
//			//Keep receiving one byte at a time
//			*receiveBufferPointer++ = I2C_masterMultiByteReceiveNext();
//		}
//	} else   {
//		//Receive last byte
//		*receiveBufferPointer = I2C_masterMultiByteReceiveNext();
//		__bic_SR_register_on_exit(LPM0_bits);
//	}
//}
//
//
////******************************************************************************
////
////This is called from the USCI_B1 interrupt vector service routine.
////
////******************************************************************************
//inline void I2C_Tx_master_ISR()
//{
//	//Check TX byte counter
//	if (transmitCounter < TXLENGTH){ //TODO switch this to counting down to zero to avoid TXLENGTH variable
//		//Initiate send of character from Master to Slave
//		I2C_masterMultiByteSendNext(transmitData[transmitCounter]);
//
//		//Increment TX byte counter
//		transmitCounter++;
//	} else   {
//		//Initiate stop only
//		I2C_masterMultiByteSendStop();
//
//		//Clear master interrupt status
//		I2C_clearInterruptFlag(I2C_TRANSMIT_INTERRUPT);
//
//		//Exit LPM0 on interrupt return
////		__bic_SR_register_on_exit(LPM0_bits);
//	}
//}
#if USCI_I2C==0
//TODO implement
#elif USCI_I2C==1
#pragma vector=USCIAB1TX_VECTOR
__interrupt void USCIAB1TX_ISR (void)
{
	if(UC1IFG&UCB1TXIFG) {                       // Check for I2C TX
		//Check TX byte counter
		if (transmitCounter < TXLENGTH){ //TODO switch this to counting down to zero to avoid TXLENGTH variable
			//Initiate send of character from Master to Slave
			I2C_masterMultiByteSendNext(transmitData[transmitCounter]);

			//Increment TX byte counter
			transmitCounter++;
		} else   {
			//Initiate stop only
			I2C_masterMultiByteSendStop();

			//Clear master interrupt status
			I2C_clearInterruptFlag(I2C_TRANSMIT_INTERRUPT);

			//Exit LPM0 on interrupt return
//			__bic_SR_register_on_exit(LPM0_bits);
		}
	}
	if(UC1IFG&UCB1RXIFG) {
//		I2C_Rx_master_ISR();                   // Get RX data
		//Decrement RX byte counter
		receiveCount--;

		if (receiveCount){
			if (receiveCount == 1){
				//Initiate end of reception -> Receive byte with NAK
				*receiveBufferPointer++ = I2C_masterMultiByteReceiveFinish();
			} else   {
				//Keep receiving one byte at a time
				*receiveBufferPointer++ = I2C_masterMultiByteReceiveNext();
			}
		} else {
			//Receive last byte
			*receiveBufferPointer = I2C_masterMultiByteReceiveNext();
			//This is also the active branch for the SingleReceive mode.
//			__bic_SR_register_on_exit(LPM0_bits);
		}
	}
}
#endif

//todo move the next two functions to I2Cdev.cpp (as methods of new class)
void I2C_readBytesFromAddress(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
	//Specify slave address
	I2C_setSlaveAddress(devAddr);

	//Set in transmit mode
	I2C_setMode(I2C_TRANSMIT_MODE);

	//Enable I2C Module to start operations
	I2C_enable();

	//Enable TX interrupt
//    I2C_enableInterrupt(I2C_TRANSMIT_INTERRUPT);

	//Set transmit length
	TXLENGTH = 0;
	//Load TX byte counter
	transmitCounter = 0;
	I2C_masterSendSingleByte ( regAddr );
	//Initiate start and send first character
//	I2C_masterMultiByteSendStart(transmitData[0]);
//	__bis_SR_register(LPM0_bits + GIE);

	//Delay until transmission completes
	while (I2C_isBusBusy()) ;

	//Disable TX interrupt
//    I2C_disableInterrupt(I2C_TRANSMIT_INTERRUPT);
	//Disable I2C Module to stop operations
	I2C_disable();

	//Set receive mode
	I2C_setMode(I2C_RECEIVE_MODE);
	//Enable I2C Module to start operations
	I2C_enable();
	//Enable master Receive interrupt
	I2C_clearInterruptFlag(I2C_RECEIVE_INTERRUPT);
	I2C_enableInterrupt(I2C_RECEIVE_INTERRUPT);

	//set pointer to receive buffer at beginning of data
	receiveBufferPointer = data;
	receiveCount = length;

	if (receiveCount==1) {
		I2C_masterSingleReceiveStart();
	} else {
		//Initialize multi reception
		I2C_masterMultiByteReceiveStart();
		//Enter low power mode 0 with interrupts enabled.
//        __bis_SR_register(LPM0_bits + GIE);
	}

	//Delay until transmission completes
	while (I2C_isBusBusy()) ;

	//Disable RX interrupt
	I2C_disableInterrupt(I2C_RECEIVE_INTERRUPT);
	//Disable I2C Module to stop operations
	I2C_disable();
}

void I2C_writeBytesToAddress(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
	//Specify slave address
	I2C_setSlaveAddress(devAddr);

	//Set in transmit mode
	I2C_setMode(I2C_TRANSMIT_MODE);

	//Enable I2C Module to start operations
	I2C_enable();

	//Enable TX interrupt
	I2C_enableInterrupt(I2C_TRANSMIT_INTERRUPT);


	/* Note: It is not ok to send regAddr and data separately to MPU9150. Has to happen without a STOP condition in between.
	 * This works without copying regAddr and data into one array by sending the first byte (regAddr) to the I2C routine and
	 * pointing the transmitData pointer to the data buffer for the following bytes. */

	//Set transmit length (data length + 1 byte regAddr)
	TXLENGTH = length;
	//Load TX byte counter and point to data to be transmitted
	transmitCounter = 0;
	transmitData = data;

	//Initiate start and send first byte (regAddr), will be followed by TXLENGTH bytes of data
	I2C_masterMultiByteSendStart(regAddr);
//	__bis_SR_register(LPM0_bits + GIE);

	//Delay until transmission completes
	while (I2C_isBusBusy()) ;

	//Disable TX interrupt
	I2C_disableInterrupt(I2C_TRANSMIT_INTERRUPT);
	//Disable I2C Module to stop operations
	I2C_disable();
}

