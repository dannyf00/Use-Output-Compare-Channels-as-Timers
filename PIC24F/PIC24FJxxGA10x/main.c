#include "config.h"					//configuration words - for C30. Use config.h for XC16
#include "gpio.h"
//#include "delay.h"						//we use software delays
#include "tmroc.h"						//output compare channels, using tmr2 as time base

//hardware configuration
#define LED_PORT			LATB
#define LED_DDR				TRISB
#define LED					(1<<5)
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
	tmroc_init5(TMR_PS1x);				//initialize system clock, with prescaler at 1:1 - not used for ga10x families
	tmroc_setpr5(50000);				//set up oc1 period
	tmroc_act5(led_flp);				//activate user handler for oc1
	tmroc_init4(TMR_PS1x);				//initialize system clock, with prescaler at 1:1 - not used for ga10x familiies
	tmroc_setpr4(50000*1.01);			//set up oc2 period, with a slight offset
	tmroc_act4(led_flp);				//activate the same user handler for oc2
	ei();
	while (1) {
	}
}
