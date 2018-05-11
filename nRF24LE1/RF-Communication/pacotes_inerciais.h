#ifndef PACOTES_INERCIAIS_H
/* Se a biblioteca mpu.h não for definida, defina-a.
Verificar é preciso para que não haja varias chamadas da
mesma biblioteca. */
#define	PACOTES_INERCIAIS_H

#include <nRF-SPIComands.h>
#include <hal_uart.h>

//Subenderecos usados no sistema
#define HOST_SUB_ADDR 0xFF //Sub addr do host
#define BROADCAST_ADDR 0xFE //Sub addr that all sensors will respond

//UART Packet: Start Signal - Command
#define UART_START_SIGNAL  0x53
#define UART_END_SIGNAL  0x04 //TODO: figure out, it will really be used?

/////////////
//Comandos //
/////////////
#define CMD_OK  0x00 //Ack - Uart Command
#define CMD_ERROR 0x01 //Error flag - Uart Command
#define CMD_START 0x02 //Start Measuring - Uart Command
#define CMD_STOP  0x03 //Stop Measuring - Uart Command
#define CMD_CONNECTION  0x04 //Teste Connection - Uart Command
#define CMD_CALIBRATE 0x05 //Calibrate Sensors Command
#define CMD_DISCONNECT 0x06 //Some sensor has gone disconected
#define CMD_READ 0x07 //Request a packet of readings
#define CMD_SET_PACKET_TYPE 0x08
#define CMD_GET_ACTIVE_SENSORS 0x09 //Retorna a variavel do host active sensors
#define CMD_SET_ACTIVE_SENSORS 0x0A //Altera a variavel do host active sensors
#define CMD_TEST_RF_CONNECTION 0x0B
#define CMD_LIGHT_UP_LED 0x0C
#define CMD_TURN_OFF_LED 0x0D
#define CMD_GET_OFFSETS 0x0E

////////////////////////
//Pacotes de leituras //
////////////////////////
#define PACKET_TYPE_ACEL      0x01
#define PACKET_TYPE_GIRO      0x02
#define PACKET_TYPE_MAG       0x03
#define PACKET_TYPE_M6        0x04
#define PACKET_TYPE_M9        0x05
#define PACKET_TYPE_QUAT      0x06
#define PACKET_TYPE_FIFO_NO_MAG       0x07
#define PACKET_TYPE_FIFO_ALL_READINGS     0x08

//Size of DMP packages
#define PSDMP 42
#define PS 20

///////////////////////////////////////////
//Pacotes de Debug e comunicacao Diversa //
///////////////////////////////////////////
#define PACKET_TYPE_STRING    0x09
#define PACKET_TYPE_HEX       0x0A
#define PACKET_TYPE_BIN       0x0B
#define PACKET_TYPE_UINT16    0x0C
#define PACKET_TYPE_INT16     0x0D
#define PACKET_TYPE_FLOAT16		0x0E
#define PACKET_TYPE_UINT8		0x0F
#define PACKET_TYPE_INT8			0x10

/* ================================================================================================ *
 | Default MotionApps v2.0 42-byte FIFO packet structure:                                           |
 |                                                                                                  |
 | [QUAT W][      ][QUAT X][      ][QUAT Y][      ][QUAT Z][      ][GYRO X][      ][GYRO Y][      ] |
 |   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  |
 |                                                                                                  |
 | [GYRO Z][      ][ACC X ][      ][ACC Y ][      ][ACC Z ][      ][      ]                         |
 |  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41                          |
 * ================================================================================================ */
#define MOTIONAPPS_FIFO_I_QUAT_WH 0 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_QUAT_WL 1 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_QUAT_XH 4 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_QUAT_XL 5 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_QUAT_YH 8 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_QUAT_YL 9 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_QUAT_ZH 12 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_QUAT_ZL 13 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_ACCEL_XH  28 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_ACCEL_XL  29 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_ACCEL_YH  32 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_ACCEL_YL  33 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_ACCEL_ZH  36 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_ACCEL_ZL  37 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_GYRO_XH 16 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_GYRO_XL 17 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_GYRO_YH 20 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_GYRO_YL 21 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_GYRO_ZH 24 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_GYRO_ZL 25 //Index of it in FIFO from DMP
//TODO: figure out how to put the mag in the fifo
#define MOTIONAPPS_FIFO_I_MAG_XH 0 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_MAG_XL 0 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_MAG_YH 0 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_MAG_YL 0 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_MAG_ZH 0 //Index of it in FIFO from DMP
#define MOTIONAPPS_FIFO_I_MAG_ZL 0 //Index of it in FIFO from DMP
//////////////////////
//Functions headers //
//////////////////////

/**
 * Envia um pacote rf de acordo com os tipos definidoes em Pacotes de leituras
 * Formato do pacote: [sensor_id] [tipo de pacote] [dados] [...]
 * @param pkt_type    Tipo de definido acima nesta biblioteca
 * @param fifo_buffer buffer da MPU_6050 de acordo com MotionApps library
 * @param my_sub_addr Sub endereco do sensor q esta a enviar o pacote
 */
void send_inertial_packet_by_rf(uint8_t pkt_type,uint8_t *fifo_buffer, uint8_t my_sub_addr);

/**
 * Envia um pacote rf no seguinte formato: [Destinatario ID] [comando]
 * @param cmd2send  Comando a ser enviado
 * @param sensor_id Sub addr do destinatario
 */
void send_rf_command(uint8_t cmd2send, uint8_t sensor_id);

////////////
//.C file //
////////////

void send_inertial_packet_by_rf(uint8_t pkt_type,uint8_t *fifo_buffer, uint8_t my_sub_addr){
  tx_buf[0] = my_sub_addr;
  tx_buf[1] = pkt_type;
  switch (pkt_type) {
    case PACKET_TYPE_ACEL://Acelerometer [X][Y][Z]
    tx_buf[2] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XH];
    tx_buf[3] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XL];//X_AC
    tx_buf[4] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YH];
    tx_buf[5] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YL];//Y_AC
    tx_buf[6] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZH];
    tx_buf[7] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZL];//Z_AC
    TX_Mode_NOACK(8);
    break;
    case PACKET_TYPE_GIRO://Gyroscope [X][Y][Z]
    tx_buf[2] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XH];
    tx_buf[3] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XL];//X_GY
    tx_buf[4] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YH];
    tx_buf[5] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YL];//Y_GY
    tx_buf[6] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZH];
    tx_buf[7] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZL];//Z_GY
    TX_Mode_NOACK(8);
    break;
    case PACKET_TYPE_MAG://Magnetometer [X][Y][Z]
    //TODO: MAG
		tx_buf[2] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_XH];
    tx_buf[3] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_XL];//X_Mag
    tx_buf[4] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_YH];
    tx_buf[5] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_YL];//Y_Mag
    tx_buf[6] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_ZH];
    tx_buf[7] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_ZL];//Z_Mag
    TX_Mode_NOACK(8);
    break;
    case PACKET_TYPE_M6://Motion6 [Acel][Gyro]
    tx_buf[2] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XH];
    tx_buf[3] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XL];//X_AC
    tx_buf[4] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YH];
    tx_buf[5] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YL];//Y_AC
    tx_buf[6] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZH];
    tx_buf[7] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZL];//Z_AC
    tx_buf[8] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XH];
    tx_buf[9] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XL];//X_GY
    tx_buf[10] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YH];
    tx_buf[11] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YL];//Y_GY
    tx_buf[12] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZH];
    tx_buf[13] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZL];//Z_GY
    TX_Mode_NOACK(14);
    break;
    case PACKET_TYPE_M9://Motion9 [Acel][Gyro][Mag]
    tx_buf[2] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XH];
    tx_buf[3] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XL];//X_AC
    tx_buf[4] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YH];
    tx_buf[5] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YL];//Y_AC
    tx_buf[6] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZH];
    tx_buf[7] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZL];//Z_AC
    tx_buf[8] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XH];
    tx_buf[9] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XL];//X_GY
    tx_buf[10] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YH];
    tx_buf[11] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YL];//Y_GY
    tx_buf[12] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZH];
    tx_buf[13] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZL];//Z_GY
		//TODO: MAG
    tx_buf[14] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_XH];
    tx_buf[15] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_XL];//X_Mag
    tx_buf[16] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_YH];
    tx_buf[17] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_YL];//Y_Mag
    tx_buf[18] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_ZH];
    tx_buf[19] = fifo_buffer[MOTIONAPPS_FIFO_I_MAG_ZL];//Z_Mag
    TX_Mode_NOACK(20);
    break;
    case PACKET_TYPE_QUAT:
    tx_buf[2] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_WH];
    tx_buf[3] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_WL];//W_quat
    tx_buf[4] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_XH];
    tx_buf[5] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_XL];//X_quat
    tx_buf[6] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_YH];
    tx_buf[7] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_YL];//Y_quat
    tx_buf[8] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_ZH];
    tx_buf[9] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_ZL];//Z_quat
    TX_Mode_NOACK(10);
    break;
    case PACKET_TYPE_FIFO_NO_MAG://Quaternion [W][X][Y][Z]
    tx_buf[2] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XH];
    tx_buf[3] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_XL];//X_AC
    tx_buf[4] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YH];
    tx_buf[5] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_YL];//Y_AC
    tx_buf[6] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZH];
    tx_buf[7] = fifo_buffer[MOTIONAPPS_FIFO_I_ACCEL_ZL];//Z_AC
    tx_buf[8] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XH];
    tx_buf[9] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_XL];//X_GY
    tx_buf[10] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YH];
    tx_buf[11] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_YL];//Y_GY
    tx_buf[12] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZH];
    tx_buf[13] = fifo_buffer[MOTIONAPPS_FIFO_I_GYRO_ZL];//Z_GY
    tx_buf[14] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_WH];
    tx_buf[15] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_WL];//W_quat
    tx_buf[16] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_XH];
    tx_buf[17] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_XL];//X_quat
    tx_buf[18] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_YH];
    tx_buf[19] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_YL];//Y_quat
    tx_buf[20] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_ZH];
    tx_buf[21] = fifo_buffer[MOTIONAPPS_FIFO_I_QUAT_ZL];//Z_quat
    TX_Mode_NOACK(22);
    break;
    default:
    //NOTE: sei la
    break;
  }/*END of Switch packet type*/
  RX_Mode();
}

void send_rf_command(uint8_t cmd2send, uint8_t sensor_id){
  tx_buf[0] = sensor_id;
  tx_buf[1] = cmd2send;
  TX_Mode_NOACK(2);
  RX_Mode();
}
//TODO: document
void send_rf_command_with_arg(uint8_t cmd2send,uint8_t agr2send, uint8_t sensor_id){
  tx_buf[0] = sensor_id;
  tx_buf[1] = cmd2send;
	tx_buf[2] = agr2send;
  TX_Mode_NOACK(3);
  RX_Mode();
}
void send_offset_values(uint8_t sensor_id, uint8_t cmd2send, int16_t Xac_of, int16_t Yac_of, int16_t Zac_of, int16_t Xgy_of, int16_t Ygy_of, int16_t Zgy_of){
  tx_buf[0] = sensor_id;
  tx_buf[1] = cmd2send;
  tx_buf[2] = (uint8_t) (Xac_of >> 8);
  tx_buf[3] = (uint8_t) (Xac_of && 0x00FF);
  tx_buf[4] = (uint8_t) (Yac_of >> 8);
  tx_buf[5] = (uint8_t) (Yac_of && 0x00FF);
  tx_buf[6] = (uint8_t) (Zac_of >> 8);
  tx_buf[7] = (uint8_t) (Zac_of && 0x00FF);
  tx_buf[8] = (uint8_t) (Xgy_of >> 8);
  tx_buf[9] = (uint8_t) (Xgy_of && 0x00FF);
  tx_buf[10] = (uint8_t) (Ygy_of >> 8);
  tx_buf[11] = (uint8_t) (Ygy_of && 0x00FF);
  tx_buf[12] = (uint8_t) (Zgy_of >> 8);
  tx_buf[13] = (uint8_t) (Zgy_of && 0x00FF);
  TX_Mode_NOACK(14);
  RX_Mode();
}
//TODO: organizar e documentar

//Packet Type | Sensor id |  ... | data | ... |
void send_packet_to_host(uint8_t packet_type, uint8_t *data_to_send, uint8_t data_len, uint8_t my_sub_addr){
	unsigned int i;
  tx_buf[0] = packet_type;
	tx_buf[1] = my_sub_addr;
	for(i=2; i<data_len+2; i++){
		tx_buf[i] = data_to_send[i-2];
	}
	TX_Mode_NOACK(data_len+2);
	RX_Mode();
}


//Start signal | Packet Type | Packet Length | ... | data | ... | End signal
void send_packet_to_computer(uint8_t packet_type, uint8_t *data_to_send, uint8_t data_len){
	unsigned int i;
    hal_uart_putchar(UART_START_SIGNAL);
    hal_uart_putchar(packet_type);
    hal_uart_putchar(data_len);
		for(i=1; i<data_len+1; i++){
			hal_uart_putchar(data_to_send[i]);
		}
	hal_uart_putchar(UART_END_SIGNAL);
}
//Start signal | Packet Type | Packet Length | ... | data | ... | End signal
void redirect_rf_pkt_to_serial(uint8_t *data_to_send, uint8_t data_len){
	unsigned int i;
	hal_uart_putchar(UART_START_SIGNAL);
	hal_uart_putchar(data_len);
	for(i=0; i<data_len; i++){
		hal_uart_putchar(data_to_send[i]);
	}
	hal_uart_putchar(UART_END_SIGNAL);//TODO: revise end
}
//Start signal | Packet Type | Packet Length | ... | data | ... | End signal
void send_packet_from_host_to_computer(uint8_t packet_type, uint8_t *data_to_send, uint8_t data_len,uint8_t my_sub_addr){
	unsigned int i;
    hal_uart_putchar(UART_START_SIGNAL);
    hal_uart_putchar(packet_type);
    hal_uart_putchar(data_len+1);
		hal_uart_putchar(my_sub_addr);
	for(i=0; i<data_len; i++){
		hal_uart_putchar(data_to_send[i]);
	}
	hal_uart_putchar(UART_END_SIGNAL);
}
//TODO: cabeçalho
void hal_uart_putstring(char *str_2_send){
  while(*str_2_send != 0)
	hal_uart_putchar(*str_2_send++);
}

#endif
