/** @file API.h
 *
 * @author Runar Kjellhaug
 *
 * @compiler This program has been tested with Keil C51 V7.50.
 *
 * @copyright
 * Copyright (c) 2005 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 5.12.05 14:14 $
 * $Revision: 5 $
 *
 */
// BYTE type definition
#ifndef _BYTE_DEF_
#define _BYTE_DEF_
typedef unsigned char BYTE;
#endif   /* _BYTE_DEF_ */

/*----------
// Define interface to nRF24L01
#ifndef _SPI_PIN_DEF_
#define _SPI_PIN_DEF_
// Define SPI pins
sbit SCK  = P0^0; // Master Out, Slave In pin (output)
sbit MISO = P0^1; // Master In, Slave Out pin (input)
sbit MOSI = P0^2; // Serial Clock pin, (output)
sbit CSN  = P0^3; // Slave Select pin, (output to CSN, nRF24L01)

// Define CE & IRQ pins
sbit CE   = P0^4; // Chip Enable pin signal (output)
sbit IRQ  = P0^5; // Interrupt signal, from nRF24L01 (input)
#endif

// Macro to read SPI Interrupt flag
#define WAIT_SPIF (!(SPI0CN & 0x80))  // SPI interrupt flag(µC platform dependent)

// Declare SW/HW SPI modes
#define SW_MODE   0x00
#define HW_MODE   0x01

// Define nRF24L01 interrupt flag's
#define MAX_RT  0x10  // Max #of TX retrans interrupt
#define TX_DS   0x20  // TX data sent interrupt
#define RX_DR   0x40  // RX data received

#define SPI_CFG 0x40  // SPI Configuration register value
#define SPI_CTR 0x01  // SPI Control register values
#define SPI_CLK 0x00  // SYSCLK/2*(SPI_CLK+1) == > 12MHz / 2 = 6MHz
#define SPI0E   0x02  // SPI Enable in XBR0 register
--------------*/
//********************************************************************************************************************//
// SPI(nRF24L01) commands
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register
//new command for 24L01
#define ACTIVATE  		0x50  // activate "R_RX_PL_WIDTH","W_ACK_PAYLOAD","W_TX_PAYLOAD_NOACK"
#define R_RX_PLD_WIDTH	0x60  // read rx payload width
#define W_ACK_PAYLOAD	0xA8  // write datas to Tx fifo with ack on pipe PPP
#define W_TX_PAYLOAD_NOACK	 0xB0  // write to tx fifo but no need ack
//********************************************************************************************************************//
// SPI(nRF24L01) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

#define DYNPD			0x1C  // dynamic payload length every pipes
#define FEATURE			0x1D  // bit2:EN_DPL,bit1:EN_ACK_PAY,bit0:EN_DYN_ACK 
#define AGC_CONFIG		0x1E  // Low 3 bits	of 1st byte 
//********************************************************************************************************************//
///                                       FUNCTION's PROTOTYPES                                                       //
//********************************************************************************************************************//
 
 void SPI_Init(BYTE Mode);     // Init HW or SW SPI
 BYTE SPI_RW(BYTE byte);                                // Single SPI read/write
 BYTE SPI_Read(BYTE reg);                               // Read one byte from nRF24L01
 BYTE SPI_RW_Reg(BYTE reg, BYTE byte);                  // Write one byte to register 'reg'
 BYTE SPI_Write_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);  // Writes multiply bytes to one register
 BYTE SPI_Read_Buf(BYTE reg, BYTE *pBuf, BYTE bytes);   // Read multiply bytes from one register
//********************************************************************************************************************/
 