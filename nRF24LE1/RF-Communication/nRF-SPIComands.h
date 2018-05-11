#ifndef NRF_SPICOMANDS_H
#define NRF_SPICOMANDS_H

#include "API.h"
#include <stdint.h>
#include <stdbool.h>

#define INTERRUPT_RFIRQ	9

#define TX_ADR_WIDTH    5   	// 5 bytes TX(RX) address width
#define TX_PLOAD_WIDTH  32   //max


/** Available data rates
 * The input argument of rf_init must be defined in this @c enum
 */
typedef enum
{
    RF_DATA_RATE_1Mbps,
    RF_DATA_RATE_2Mbps,
    RF_DATA_RATE_250kbps
} rf_data_rate_t;

/** Available tx power modes
 * The input argument of rf_init must be defined in this @c enum
 */
typedef enum
{
    RF_TX_POWER_NEGATIVE_18dBm,
    RF_TX_POWER_NEGATIVE_12dBm,
    RF_TX_POWER_NEGATIVE_6dBm,
    RF_TX_POWER_0dBm
} rf_tx_power_t;

//Enderecos:
//Definido como endere�o da pipe 0
uint8_t code ADDR_HOST[TX_ADR_WIDTH] = {0xE7,0xE7,0xE7,0xE7,0xE7}; // Define a host adr

uint8_t xdata rx_buf[TX_PLOAD_WIDTH];
uint8_t xdata tx_buf[TX_PLOAD_WIDTH];

uint8_t bdata sta;
sbit	RX_DR	= sta^6;
sbit	TX_DS	= sta^5;
sbit	MAX_RT  = sta^4;

bit newPayload = 0;     // Flag to show new Payload from host
uint8_t xdata payloadWidth = 0;
/**
 * TODO: perguntar o serginho
 * [SPI_RW description]
 * @param  value [description]
 * @return       [description]
 */
uint8_t SPI_RW(uint8_t value);

/**
 * TODO: perguntar o serginho
 * [SPI_RW_Reg description]
 * @param  reg   [description]
 * @param  value [description]
 * @return       [description]
 */
uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value);

/**
 * TODO: perguntar o serginho
 * [SPI_Read description]
 * @param  reg [description]
 * @return     [description]
 */
uint8_t SPI_Read(uint8_t reg);

/**
 * TODO: perguntar o serginho
 * @param  reg   [description]
 * @param  pBuf  [description]
 * @param  bytes [description]
 * @return       [description]
 */
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);

/**
 * TODO: perguntar o serginho
 * [SPI_Write_Buf description]
 * @param  reg   [description]
 * @param  pBuf  [description]
 * @param  bytes [description]
 * @return       [description]
 */
uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes);

/**
 * Inicia o estado recepcao, onde o nrf estara aguardando a interrupcao rf
 */
void RX_Mode(void);

/**
 * Entra no estado de transmissao, com pacote sem ACK.
 * Transmitindo o atual valor em tx_buff
 * RF transceiver is never in TX mode longer than 4 ms.
 * @param payloadLength tamanho do pacote escrito em tx buff, maximo 32
 */
void TX_Mode_NOACK(short int payloadLength);

/**
 * Inicia a comunicacao RF, apos configura-la ativa todas as interrupcoes e aguarda em RX Mode
 * @param rx_addr      Endereço RX de 5 bytes
 * @param tx_addr      Endereço RX de 5 bytes
 * @param rf_channel   Valor em MHz a ser somado a 2.4Hz como canal, entre 0 e 125.
 * @param rf_data_rate Velocidade de transmissao nor ar que deseja utilizar
 * @param rf_pwr       Power of the Transmission
 */
void rf_init(uint8_t *rx_addr,uint8_t *tx_addr, uint8_t rf_channel, rf_data_rate_t rf_data_rate, rf_tx_power_t rf_pwr);


////////////
//.C File //
////////////

/**
 * TODO: perguntar o serginho
 * [SPI_RW description]
 * @param  value [description]
 * @return       [description]
 */
uint8_t SPI_RW(uint8_t value) {
    SPIRDAT = value;			 			 							//spidat

    while(!(SPIRSTAT & 0x02));  							// wait for byte transfer finished

    return SPIRDAT;             							// return SPI read value
}

/**
 * TODO: perguntar o serginho
 * [SPI_RW_Reg description]
 * @param  reg   [description]
 * @param  value [description]
 * @return       [description]
 */
uint8_t SPI_RW_Reg(uint8_t reg, uint8_t value) {
    uint8_t status;

    RFCSN = 0;                   						// CSN low, init SPI transaction?
    status = SPI_RW(reg);      							// select register
    SPI_RW(value);             							// ..and write value to it..
    RFCSN = 1;                   						// CSN high again  ??rfcon^1

    return(status);            							// return nRF24L01 status byte
}

/**
 * TODO: perguntar o serginho
 * [SPI_Read description]
 * @param  reg [description]
 * @return     [description]
 */
uint8_t SPI_Read(uint8_t reg) {
    uint8_t reg_val;

    RFCSN = 0;                											// CSN low, initialize SPI communication...
    SPI_RW(reg);            												// Select register to read from..
    reg_val = SPI_RW(0);    												// ..then read registervalue
    RFCSN = 1;                											// CSN high, terminate SPI communication RF

    return(reg_val);        												// return register value
}

/**
 * TODO: perguntar o serginho
 * @param  reg   [description]
 * @param  pBuf  [description]
 * @param  bytes [description]
 * @return       [description]
 */
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes){
    uint8_t status,byte_ctr;

    RFCSN = 0;                    								// Set CSN low, init SPI tranaction
    status = SPI_RW(reg);       									// Select register to write to and read status byte

    for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);    							// Perform SPI_RW to read byte from nRF24L01

    RFCSN = 1;                         					  // Set CSN high again

    return(status);                    				  	// return nRF24L01 status byte
}

/**
 * TODO: perguntar o serginho
 * [SPI_Write_Buf description]
 * @param  reg   [description]
 * @param  pBuf  [description]
 * @param  bytes [description]
 * @return       [description]
 */
uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes){
    uint8_t status,byte_ctr;

    RFCSN = 0;                   									// Set CSN low, init SPI tranaction
    status = SPI_RW(reg);    											// Select register to write to and read status byte
    for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) 	// then write all byte in buffer(*pBuf)
    SPI_RW(*pBuf++);
    RFCSN = 1;                 										// Set CSN high again
    return(status);          											// return nRF24L01 status byte
}

/**
 * Inicia o estado recepcao, onde o nrf estara aguardando a interrupcao rf
 */
void RX_Mode(void) {
    sta = 0;
    newPayload = 0;
    RFCE=0;																					//RFCON ^ 0: RF controle transceptor 1: Ativar
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   				// Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
    RFCE = 1; 																			// Set CE pin high to enable RX device
}

/**
 * Entra no estado de transmissao, com pacote sem ACK.
 * Transmitindo o atual valor em tx_buff
 * RF transceiver is never in TX mode longer than 4 ms.
 * @param payloadLength tamanho do pacote escrito em tx buff, maximo 32
 */
void TX_Mode_NOACK(short int payloadLength) {
    RFCE=0;							   														//RFCON ^ 0: RF controle transceptor 1: Ativar
    SPI_RW_Reg(WRITE_REG + CONFIG, 0x0E);     				//???? Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, ADDR_HOST, TX_ADR_WIDTH);
    SPI_Write_Buf(WR_TX_PLOAD, tx_buf, payloadLength); // Writes data to TX payload
    //Endere?o da porta P2, matrizes tx_buf (), o comprimento da matriz ? enviada)
    RFCE=1;		       																	 //RFCON ^ 0: RF controle transceptor 1: Desativar
	while (!(TX_DS|MAX_RT));
}

/**
 * Interrupt handle para evento de recepcao de payload
 * ativa o sinalizador newPayload
 * o tamanho da payload é armazenado em payloadWidth
 */
void RF_IRQ(void) interrupt INTERRUPT_RFIRQ
{
    sta=SPI_Read(STATUS);																// read register STATUS's value
    if(RX_DR) {																						// if receive data ready (RX_DR) interrupt
        SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);	// read receive payload from RX_FIFO buffer
        SPI_RW_Reg(FLUSH_RX,0);
        newPayload = 1;
        payloadWidth = SPI_Read(R_RX_PLD_WIDTH);  				// Return the number of bytes on receved payload
        if(payloadWidth > 32){
            payloadWidth = 0;
            SPI_RW_Reg(FLUSH_RX,0);
            newPayload = 0;
        }
    }
    if(MAX_RT)
    SPI_RW_Reg(FLUSH_TX,0);
    if(TX_DS)
    SPI_RW_Reg(FLUSH_TX,0);
    SPI_RW_Reg(WRITE_REG+STATUS,0x70);								// clear RX_DR or TX_DS or MAX_RT interrupt flag
}

/**
 * Inicia a comunicacao RF, apos configura-la ativa todas as interrupcoes e aguarda em RX Mode
 * @param rx_addr      Endereço RX de 5 bytes
 * @param tx_addr      Endereço RX de 5 bytes
 * @param rf_channel   Valor em MHz a ser somado a 2.4Hz como canal, entre 0 e 125.
 * @param rf_data_rate Velocidade de transmissao nor ar que deseja utilizar
 * @param rf_pwr       Power of the Transmission
 */
void rf_init(uint8_t *rx_addr,uint8_t *tx_addr, uint8_t rf_channel, rf_data_rate_t rf_data_rate, rf_tx_power_t rf_pwr) {
		uint8_t rf_setup_byte = 0x07; //0000 0111
		RFCE = 0; // Radio chip enable low
    RFCKEN = 1; // Radio clk enable
    RF = 1;
    RFCE = 0; // Radio chip enable low
    RFCKEN = 1; // Radio clk enable

    switch (rf_pwr) {
      case RF_TX_POWER_NEGATIVE_18dBm:
      rf_setup_byte &= 0xF9; //1111 1001
      break;
      case RF_TX_POWER_NEGATIVE_12dBm:
      rf_setup_byte &= 0xFB;//1111 1011
      rf_setup_byte |= 0x02;//0000 0010
      break;
      case RF_TX_POWER_NEGATIVE_6dBm:
      rf_setup_byte &= 0xFD;//1111 1101
      rf_setup_byte |= 0x04;//0000 0100
      break;
      case RF_TX_POWER_0dBm:
      rf_setup_byte |= 0x04;//0000 0110
      break;
    }
    switch (rf_data_rate) {
      case RF_DATA_RATE_250kbps:
      rf_setup_byte &= 0xF7;//1111 0111
      rf_setup_byte |= 0x20;//0010 0000
      break;
      case RF_DATA_RATE_1Mbps:
      rf_setup_byte &= 0xD7;//1101 0111
      break;
      case RF_DATA_RATE_2Mbps:
      rf_setup_byte &= 0xDF;//1101 1111
      rf_setup_byte |= 0x08;//0000 1000
      break;
    }
    //Transmit Address.
    SPI_Write_Buf(WRITE_REG + TX_ADDR, tx_addr, TX_ADR_WIDTH);
    //Receive Address
    SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, rx_addr, TX_ADR_WIDTH);
    // Enable Pipe0 (only pipe0)
    SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);
    // Disable Auto.Ack
    SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);
    // Time to automatic retransmition selected: 250us, retransmition disabled
    SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x00);
    // Select RF channel 40
    SPI_RW_Reg(WRITE_REG + RF_CH, rf_channel);
    // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
    SPI_RW_Reg(WRITE_REG + RF_SETUP, rf_setup_byte);//0 0 0 0 0 11 1
    // Ativa Payload din?mico em data pipe 0
    SPI_RW_Reg(WRITE_REG + DYNPD, 0x01);
    // Ativa Payload din?mico, com ACK e comando W_TX_PAY
    SPI_RW_Reg(WRITE_REG + FEATURE, 0x07);
    //Stay in RX Mode waiting for command
    EA = 1;
    RX_Mode();
}
/**************************************************/
#endif
