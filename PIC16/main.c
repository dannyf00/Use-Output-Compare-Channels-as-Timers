#include <stdint.h>
#include "config.h"						//configuration words
#include "gpio.h"                           //we use gpio functions
#include "delay.h"                          //we use software delays
#include "tmr1oc.h"

//hardware configuration
#define LED_PORT			PORTC
#define LED_DDR				TRISC
#define LED					(1<<0)
#define LED_PR				10000		//16bit max
//end hardware configuration

//global defines

//global variables

//global interrupt
void interrupt isr(void) {
	if (CCP1IF) ccp1_isr();							//run the user isr
	if (CCP2IF) ccp2_isr();							//run the user isr
	if (CCP3IF) ccp3_isr();							//run the user isr
	if (CCP4IF) ccp4_isr();							//run the user isr
	if (CCP5IF) ccp5_isr();							//run the user isr
}

//flip the led
void led_flp(void) {
	IO_FLP(LED_PORT, LED);
}
		
int main(void) {
	
	mcu_init();							    //initialize the mcu
	
	IO_OUT(LED_DDR, LED);					//led as output
	tmr1oc_init();							//reset tmr1oc
	tmr1oc_setpr1(LED_PR);					//set ccp1 period
	tmr1oc_act1(led_flp);					//install user handler
	tmr1oc_setpr5(LED_PR*1.1);				//set ccp2 period, with a slight offset
	tmr1oc_act5(led_flp);					//install user handler
	ei();									//enable global interrupt
	while (1) {
	}
}

