#ifndef _TMR1OC_H
#define _TMR1OC_H
//header file to use tmr1
//use output compare as timer

#include "gpio.h"
#include <avr/interrupt.h>

//hardware configuration
//end hardware configuration

//prescaler defines
#define TMR1_NOCLK			0x00		//cs210=no clock selected
#define TMR1_PS1x			0x01		//clk/1
#define TMR1_PS8x			0x02		//clk/8
#define TMR1_PS64x			0x03		//clk/64
#define TMR1_PS256x			0x04		//clk/256
#define TMR1_PS1024x		0x05		//clk/1024
#define TMR1_EXTN			0x06		//external clock on Tn pin, negative transistion
#define TMR1_EXTP			0x07		//external clock on Tn pin, positive transistion
#define TMR1_PSMASK			0x07

//tmr period settings
#define TMR_ms				(F_CPU / 1000)				//1ms period - minimum period
#define TMR_1ms				(TMR_ms * 1)				//1ms
#define TMR_2ms				(TMR_ms * 2)				//2ms period
#define TMR_5ms				(TMR_ms * 5)				//5ms period
#define TMR_10ms			(TMR_ms * 10)				//10ms
#define TMR_20ms			(TMR_ms * 20)				//20ms period
#define TMR_50ms			(TMR_ms * 50)				//50ms period
#define TMR_100ms			(TMR_ms * 100)				//100ms
#define TMR_200ms			(TMR_ms * 200)				//200ms period
#define TMR_500ms			(TMR_ms * 500)				//500ms period
#define TMR_1000ms			(TMR_ms * 1000)				//1000ms
#define TMR_2000ms			(TMR_ms * 2000)				//2000ms period
#define TMR_5000ms			(TMR_ms * 5000)				//5000ms period
#define TMR_10000ms			(TMR_ms * 10000)			//10000ms

//set dc for channel a
//0b10->clear on compare match
//need to pay attention to the mode bits (wgm13..0) and dc value
void tmr1_init(uint8_t prescaler);

//set dc for channel b
//0b10->clear on compare match
//need to pay attention to the mode bits (wgm13..0) and dc value
void tmr1a_setpr(uint16_t dc);
void tmr1b_setpr(uint16_t dc);

//install user handler
void tmr1_act(void (*isr_ptr)(void));					//user handler for timer1 overflow
void tmr1a_act(void (*isr_ptr)(void));					//user handler for timer1 cha output compare
void tmr1b_act(void (*isr_ptr)(void));					//user handler for timer1 chb output compare

#endif
