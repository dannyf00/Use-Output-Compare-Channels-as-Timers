#include "gpio.h"					//we use F_CPU + gpio functions
#include "delay.h"					//we use software delays
//#include "coretick.h"				//we use dwt
//#include "dhry.h"					//we use dhrystone benchmark
#include "tim2oc.h"					//we use tim2 output compare as timers

//hardware configuration
#define LEDG_PORT		GPIOC
#define LEDG			(1<<9)		//green led on PC9 on Discovery board

#define LEDB_PORT		GPIOC
#define LEDB			(1<<8)		//blue led on PC8 on Discovery board
//end hardware configuration

//global defines

//global variables

//task 1 - flip green led
void ledg_flp(void) {
	IO_FLP(LEDG_PORT, LEDG);
}

//task 1 - flip blue led
void ledb_flp(void) {
	IO_FLP(LEDB_PORT, LEDB);
}
int main(void) {

	mcu_init();									//reset the  mcu

	//initialize the pins to outout
	IO_OUT(LEDG_PORT, LEDG);					//ledg/b as output
	IO_OUT(LEDB_PORT, LEDB);

	//set up timer2/prescaler
	tim2_init(10);								//prescaler set to 10:1
	//set up oc1/2 for ledg fading
	tim2_setpr1(10000);							//oc1 period to 10000
	tim2_act1(ledg_flp);						//install user handler
	tim2_setpr2(10000*1.01);					//oc2 period slightly off
	tim2_act2(ledg_flp);
	//set up oc3/4 for ledb fading
	tim2_setpr3(20000);							//oc1 period to 10000
	tim2_act3(ledb_flp);						//install user handler
	tim2_setpr4(20000*1.01);					//oc2 period slightly off
	tim2_act4(ledb_flp);

	//ei();										//enable global interrupts
	while (1) {
	};
}
