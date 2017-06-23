#include "config.h"					//configuration words - for C30. Use config.h for XC16
#include "gpio.h"
//#include "delay.h"						//we use software delays
#include "tmroc.h"						//output compare channels, using tmr2 as time base

//hardware configuration
#define LED_PORT			LATB
#define LED_DDR				TRISB
#define LED					(1<<0)
//end hardware configuration

//global defines

//global variables

//flip the led
void led_flp(void) {
	IO_FLP(LED_PORT, LED);
}


int main(void) {
	mcu_init();							//reset the mcu
	IO_OUT(LED_DDR, LED);				//led as output
	tmroc_init(TMR_PS1x);				//initialize tmr2/3, with prescaler at 1:1
	tmroc_setpr1(10000);				//set up oc1 period
	tmroc_act1(led_flp);				//activate user handler for oc1
	tmroc_setpr2(10000*1.1);			//set up oc2 period, with a slight offset
	tmroc_act2(led_flp);				//activate the same user handler for oc2
	ei();
	while (1) {
	}
}
