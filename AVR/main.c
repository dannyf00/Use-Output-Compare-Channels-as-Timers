//#include <avr/io.h>							//we use gcc-avr
#include "gpio.h"
#include "delay.h"							//we use software delays
#include "tmr1oc.h"							//we use timer1 output compare interrupts

//hardware configuration
#define LED_PORT			PORTB
#define LED_DDR				DDRB
#define LED0				(1<<0)
#define LED1				(1<<1)
#define LED2				(1<<2)
//end hardware configuration

//global defines

//global variables

//flip led0
void led0_flp(void) {
	IO_FLP(LED_PORT, LED0);
}

//flip led1
void led1_flp(void) {
	IO_FLP(LED_PORT, LED1);
}

//flip led2
void led2_flp(void) {
	IO_FLP(LED_PORT, LED2);
}

int main(void) {

	mcu_init();								//reset the mcu
	IO_OUT(LED_DDR, LED0 | LED1 | LED2);
	IO_CLR(LED_PORT, LED0 | LED1 | LED2);	//led as output, idles low

	//set up time base
	tmr1_init(TMR1_PS8x);					//set tmr1 prescaler to 8:1
	//install user isr handler for timer1 overflow
	tmr1_act(led0_flp);

	//set up timer1 output compare ch a
	tmr1a_setpr(0xffff/4);					//tmr1oc ch a triggers every 0xffff/4 x 8 ticks
	tmr1a_act(led1_flp);					//install handler for timer1 OC ch a

	//set up timer1 output compare ch b
	tmr1b_setpr(0xffff/2);					//tmr1oc ch a triggers every 0xffff/2 x 8 ticks
	tmr1b_act(led2_flp);					//install handler for timer1 OC ch b

	ei();									//enable global interrupts

	while(1) {
	}

	return 0;
}
