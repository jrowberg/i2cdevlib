#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H

#include "nrf24le1.h"
#include "stdint.h"
#include "stdbool.h"
#include "API.h"

/**************************************************/
/********************TIMER*************************/
#define INTERRUPT_TMR0	1 //timer
#define NOVT0   0x00

uint8_t NBT0L	=	0xEA;// Este tempo equivale a
uint8_t NBT0H	=	0xCB; // Freq. de Amostragem de 100Hz
uint8_t initial_timer_count  = 1; //valor de multiplicacao do timer
uint8_t timer_count = 0;

uint8_t timer_elapsed = 0; //flag of timer elapsed

void start_T0(void);
void stop_T0(void);
void setup_T0_ticks(uint16_t number_beats,uint8_t flag_count);
void setup_T0_elapsed_us(uint16_t T_us,uint8_t flag_count);
void setup_T0_elapsed_ms(uint16_t T_ms,uint8_t flag_count);
void setup_T0_freq(uint8_t Freq_Hz,uint8_t flag_count);
/********************TIMER*************************/
/**************************************************/

/**
 * Configura o tempo do timer de acordo com a frequencia em Hz
 * A frequencia minima é de 21 Hz
 * A frequencia maxima é de 1333333 Hz
 * @param Freq_Hz    Frequencia em Hertz para configurar o timer
 * @param flag_count Numero de vezes para dividir essa frequencia
 */
void setup_T0_freq(uint8_t Freq_Hz,uint8_t flag_count){
  uint16_t qnt_ticks = (4000000 / (3 * Freq_Hz));
  setup_T0_ticks(qnt_ticks, flag_count);
}
/**
 * Configura o tempo do timer de acordo com periodo em ms.
 * O periodo deve ser um multiplo de 0.000750 ms. (ou multiplo de 0.003)
 * O valor maximo é de 49.15125 ms (ou em multiplo de 3 = 49.149 ms)
 * Caso o valor nao seja um multiplo de 0.000750 ms
 * ele sera arredondado para o menor mais proximo.
 * @param T_ms       Tempo em ms, multiplo de 0.00075ms (ou 0.003 ms para facilitar)
 * @param flag_count Numero de vezes para o multiplicar este tempo
 */
void setup_T0_elapsed_ms(uint16_t T_ms,uint8_t flag_count){
  setup_T0_elapsed_us(T_ms*100,flag_count);
}
/**
 * Configura o tempo do timer de acordo com periodo em us.
 * O periodo deve ser um multiplo de 0.750 us. (ou multiplo de 3.)
 * O valor maximo é de 49151.25 us (ou em multiplo de 3 = 49149 us)
 * Caso o valor nao seja um multiplo de 0.75
 * ele sera arredondado para o menor mais proximo.
 * @param T_us       Tempo em us, multiplo de 0.75us (ou 3us para facilitar)
 * @param flag_count Numero de vezes para o multiplicar este tempo
 */
void setup_T0_elapsed_us(uint16_t T_us,uint8_t flag_count){
  uint16_t qnt_ticks = T_us * 4 / 3;
  setup_T0_ticks(qnt_ticks,flag_count);
}
/**
 * Configura o tempo do timer de cacordo com o numeo de ticks
 * Cada tick tem um periodo de 750 ns ((16/12)MHz = 13.333 MHz)
 * é possivel ter ate 0xffff ticks =  49.15125 ms = 20.345 Hz.
 * @param number_beats numero de ticks necessarios para disparar a timer interrupt
 * @param flag_count   numero de timer interrupts necessaria para disparar o sinal timer_elapsed
 */
void setup_T0_ticks(uint16_t number_beats,uint8_t flag_count){
  initial_timer_count = flag_count;
  NBT0H = (0xFFFF - number_beats) << 8;
  NBT0L = (uint8_t) (0xFFFF - number_beats);
}
/**************************************************/
/********************TIMER*************************/
void TMR0_IRQ(void) interrupt INTERRUPT_TMR0
{
	if(!NOVT0)
	{
		timer_count--;
    if(timer_count <= 0){
        timer_elapsed = 1;
        timer_count = initial_timer_count;
    }
		TH0= NBT0H;
		TL0= NBT0L;
	}
}
/**************************************************/
void start_T0(void) {
	TMOD=0x31;						// Select Timer 1 --> STOPPED, Timer 0 --> TIMER/16 bits
	TH0= NBT0H;
	TL0= NBT0L;
	ET0=1;								// Active interrupt on Timer 0
	EA=1;									// Active all interrupts
	TR0=1;								// Timer 0 --> RUN
}
/**************************************************/
void stop_T0(void) {
	TMOD=0x31;						// Select Timer 1 --> STOPPED, Timer 0 --> TIMER/16 bits
	TH0= NBT0H;
	TL0= NBT0L;
	ET0=0;								// Active interrupt on Timer 0
	EA=1;									// Active all interrupts
	TR0=0;								// Timer 0 --> RUN
}
/********************TIMER*************************/
/**************************************************/
#endif
