#include "tmroc.h"					//output compare using tmr2 as time base

//global defines

//for compatibility reasons
#if defined(USE_TMR2)
#define _TxMD				_T2MD
#define _TxON				T2CONbits.TON
#define _TxCS				T2CONbits.TCS
#define _TxGE				T2CONbits.TGATE
#define _TxCKPS				T2CONbits.TCKPS
#define _Tx_32				T2CONbits.T32
#define _TxIE				IEC0bits.T2IE
#define _TxIF				IFS0bits.T2IF			//tmr1 overflow flag
#define PRx					PR2
#define TMRx				TMR2
#define OCTSEL				0						//OCTSEL bit in OCxCON: '0'->tmr2 as time base, '1'->tmr3 as time base
#else												//use tmr3 instead
#define _TxMD				_T3MD
#define _TxON				T3CONbits.TON
#define _TxCS				T3CONbits.TCS
#define _TxGE				T3CONbits.TGATE
#define _TxCKPS				T3CONbits.TCKPS
//#define _Tx_32				T3CONbits.T32		//not on tmr3
#define _TxIE				IEC0bits.T3IE
#define _TxIF				IFS0bits.T3IF			//tmr1 overflow flag
#define PRx					PR3
#define TMRx				TMR3
#define OCTSEL				1						//OCTSEL bit in OCxCON: '0'->tmr2 as time base, '1'->tmr3 as time base
#endif

#define _OC1IF				IFS0bits.OC1IF
#define _OC1IE				IEC0bits.OC1IE
#define _OC2IF				IFS0bits.OC2IF
#define _OC2IE				IEC0bits.OC2IE
#define _OC3IF				IFS1bits.OC3IF
#define _OC3IE				IEC1bits.OC3IE
#define _OC4IF				IFS1bits.OC4IF
#define _OC4IE				IEC1bits.OC4IE
#define _OC5IF				IFS2bits.OC5IF
#define _OC5IE				IEC2bits.OC5IE

//empty handler
static void empty_handler(void) {
	//do nothing here
}

//isr pointers
void (*tmroc1_isrptr)(void) = empty_handler;
void (*tmroc2_isrptr)(void) = empty_handler;
void (*tmroc3_isrptr)(void) = empty_handler;
void (*tmroc4_isrptr)(void) = empty_handler;
void (*tmroc5_isrptr)(void) = empty_handler;
	
//tmr output compare period
volatile uint16_t tmroc1_pr = 0xffff;
volatile uint16_t tmroc2_pr = 0xffff;
volatile uint16_t tmroc3_pr = 0xffff;
volatile uint16_t tmroc4_pr = 0xffff;
volatile uint16_t tmroc5_pr = 0xffff;

//output compare isr
void _ISR _OC1Interrupt(void) {
	_OC1IF=0;							//clear tmr1 interrupt flag
	OC1R += tmroc1_pr;					//go to the next match point
	tmroc1_isrptr();					//run user handler
}

//output compare isr
void _ISR _OC2Interrupt(void) {
	_OC2IF=0;							//clear tmr1 interrupt flag
	OC2R += tmroc2_pr;					//go to the next match point
	tmroc2_isrptr();					//run user handler
}

//output compare isr
void _ISR _OC3Interrupt(void) {
	_OC3IF=0;							//clear tmr1 interrupt flag
	OC3R += tmroc3_pr;					//go to the next match point
	tmroc3_isrptr();					//run user handler
}

//output compare isr
void _ISR _OC4Interrupt(void) {
	_OC4IF=0;							//clear tmr1 interrupt flag
	OC4R += tmroc4_pr;					//go to the next match point
	tmroc4_isrptr();					//run user handler
}

//output compare isr
void _ISR _OC5Interrupt(void) {
	_OC5IF=0;							//clear tmr1 interrupt flag
	OC5R += tmroc5_pr;					//go to the next match point
	tmroc5_isrptr();					//run user handler
}

//rest tmr output compare
void tmroc_init(uint16_t ps) {
	tmroc1_isrptr = tmroc2_isrptr = tmroc3_isrptr = tmroc4_isrptr = tmroc5_isrptr = empty_handler;
	tmroc1_pr = tmroc2_pr = tmroc3_pr = tmroc4_pr = tmroc5_pr = 0xffff;
	
	_TxMD = 0;							//enable power to tmr
	_TxON = 0;							//turn off rtc1
	PRx=0xffff;							//period-1;							//minimum rtc resolution is 1ms
	_TxCS = 0;							//use internal clock = Fosc / 2
#if defined(USE_TMR2)
	_Tx_32 = 0;							//clock as two 16-bit timer/counter
#endif
	//T1CON = (T1CON & 0xcf) | ((TMR1_PS_1x & 0x03) << 4);	//set the prescaler
	_TxCKPS=ps & TMR_PSMASK;			//set prescaler to 1:1
	_TxGE = 0;							//rtc1 gate disabled
	TMRx = 0;							//reset the timer/counter

	_TxIF = 0;							//reset the flag
	_TxIE = 0;							//disable interrupt
	//PEIE=1;								//peripheral interrupt on. no PEIE for PIC24 mcus

	_OC1IF = _OC2IF = _OC3IF = _OC4IF = _OC5IF = 0;
	_OC1IE = _OC2IE = _OC3IE = _OC4IE = _OC5IE = 0;

	_TxON = 1;							//turn on tmr
}

//set output compare channel period
void tmroc_setpr1(uint16_t pr) {
	OC1CON = 	(0<<13) |					//'0'->output continues in idle
				(OCTSEL<<3) |				//'0'->use tmr2 as time base, '1'->use tmr3 as time base
				(0b010<<0) |			//0b010->OCx idles high, output compare match forces it low
				0x00;
	tmroc1_pr = pr - 1;
	OC1R = TMRx + tmroc1_pr;			//load the top
	_OC1IF = _OC1IE = 0;				//clear the flag. no interrupt
}

//install user handler
void tmroc_act1(void (*isrptr)(void)) {
	tmroc1_isrptr = isrptr;
	_OC1IF = 0;
	_OC1IE = 1;
}

//set output compare channel period
void tmroc_setpr2(uint16_t pr) {
	OC2CON = 	(0<<13) |					//'0'->output continues in idle
				(OCTSEL<<3) |				//'0'->use tmr2 as time base, '1'->use tmr3 as time base
				(0b010<<0) |			//0b010->OCx idles high, output compare match forces it low
				0x00;
	tmroc2_pr = pr - 1;
	OC2R = TMRx + tmroc2_pr;			//load the top
	_OC2IF = _OC2IE = 0;				//clear the flag. no interrupt
}

//install user handler
void tmroc_act2(void (*isrptr)(void)) {
	tmroc2_isrptr = isrptr;
	_OC2IF = 0;
	_OC2IE = 1;
}

//set output compare channel period
void tmroc_setpr3(uint16_t pr) {
	OC3CON = 	(0<<13) |					//'0'->output continues in idle
				(OCTSEL<<3) |				//'0'->use tmr2 as time base, '1'->use tmr3 as time base
				(0b010<<0) |			//0b010->OCx idles high, output compare match forces it low
				0x00;
	tmroc3_pr = pr - 1;
	OC3R = TMRx + tmroc3_pr;			//load the top
	_OC3IF = _OC3IE = 0;				//clear the flag. no interrupt
}

//install user handler
void tmroc_act3(void (*isrptr)(void)) {
	tmroc3_isrptr = isrptr;
	_OC3IF = 0;
	_OC3IE = 1;
}

//set output compare channel period
void tmroc_setpr4(uint16_t pr) {
	OC4CON = 	(0<<13) |					//'0'->output continues in idle
				(OCTSEL<<3) |				//'0'->use tmr2 as time base, '1'->use tmr3 as time base
				(0b010<<0) |			//0b010->OCx idles high, output compare match forces it low
				0x00;
	tmroc4_pr = pr - 1;
	OC4R = TMRx + tmroc4_pr;			//load the top
	_OC4IF = _OC4IE = 0;				//clear the flag. no interrupt
}

//install user handler
void tmroc_act4(void (*isrptr)(void)) {
	tmroc4_isrptr = isrptr;
	_OC4IF = 0;
	_OC4IE = 1;
}

//set output compare channel period
void tmroc_setpr5(uint16_t pr) {
	OC5CON = 	(0<<13) |					//'0'->output continues in idle
				(OCTSEL<<3) |				//'0'->use tmr2 as time base, '1'->use tmr3 as time base
				(0b010<<0) |			//0b010->OCx idles high, output compare match forces it low
				0x00;
	tmroc5_pr = pr - 1;
	OC5R = TMRx + tmroc5_pr;			//load the top
	_OC5IF = _OC5IE = 0;				//clear the flag. no interrupt
}

//install user handler
void tmroc_act5(void (*isrptr)(void)) {
	tmroc5_isrptr = isrptr;
	_OC5IF = 0;
	_OC5IE = 1;
}
