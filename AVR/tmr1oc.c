#include "tmr1oc.h"					//we use timer1 output compare

//global defines

//for portability
//tmr1
#define TCCRxA			TCCR1A
#define TCCRxB			TCCR1B
#define OCRxA			OCR1A
#define OCRxB			OCR1B
#define TCNTx			TCNT1
#define TIFRx			TIFR1
#define TIMSKx			TIMSK1
#define COMxA1			COM1A1
#define COMxA0			COM1A0
#define COMxB1			COM1B1
#define COMxB0			COM1B0
#define WGMx3			WGM13
#define WGMx2			WGM12
#define WGMx1			WGM11
#define WGMx0			WGM10
#define OCFxA			OCF1A
#define OCFxB			OCF1B
#define TOVx			TOV1
#define OCIExA			OCIE1A
#define OCIExB			OCIE1B
#define TOIEx			TOIE1
#define TMRx_PSMASK		TMR1_PSMASK

//global variables

//empty handler
static void /*_tmr1_*/empty_handler(void) {
	//default tmr handler
}

static void (* _isrptr_tov)(void)=empty_handler;	//tmr1_ptr pointing to empty_handler by default
static void (* _isrptr_oca)(void)=empty_handler;	//tmr1_ptr pointing to empty_handler by default
static void (* _isrptr_ocb)(void)=empty_handler;	//tmr1_ptr pointing to empty_handler by default
uint16_t _inc_oca;						//compare point for oc1a increment / period
uint16_t _inc_ocb;						//compare point for oc1b increment / period

//timer overflow interrupt
ISR(TIMER1_OVF_vect) {
	//clear the flag - done automatically
	//OCR1A += _inc_oca;					//advance to the next compare point
	_isrptr_tov();					//run the user handler
}

//output compare ch a isr
ISR(TIMER1_COMPA_vect) {
	//clear the flag - done automatically
	OCRxA += _inc_oca;					//advance to the next compare point
	_isrptr_oca();					//run the user handler
}

//output compare b isr
ISR(TIMER1_COMPB_vect) {
	//clear the flag - done automatically
	OCRxB += _inc_ocb;					//advance to the next compare point
	_isrptr_ocb();					//run the user handler
}
//reset the tmr
//default: normal mode (16-bit top at 0xffff)
void tmr1_init(uint8_t prescaler) {
	//reset user isr handlers and default output compare increments
	_isrptr_tov = _isrptr_oca = _isrptr_ocb = empty_handler;
	_inc_oca = _inc_ocb = 0xffff;						//default values

	TCCRxB =	TCCRxB & (~TMRx_PSMASK);			//turn off tmr1
	///*_tmr1*/_isr_ptr=/*_tmr1_*/empty_handler;			//reset isr ptr
	TCCRxA =	(0<<COMxA1) | (0<<COMxA0) |	//output compare a pins normal operation
				(0<<COMxB1) | (0<<COMxB0) |	//output compare b pins normal operation
				//(0<<COM1C1) | (0<<COM1C0) |	//output compare c pins normal operation
				(0<<WGMx1) | (0<<WGMx0)		//wgm13..0 = 0b0000 -> default
				;
	TCCRxB =	(TCCRxB & ~((1<<WGMx3) | (1<<WGMx2))) |	//clear wgm13..2
				(0<<WGMx3) | (0<<WGMx2);	//wgm13.0=0b0000 - default
	//TCCR1C =	(0<<FOC1A) |				//forced output on ch a disabled
	//			(0<<FOC1B) |				//forced output on ch b disabled
	//			(0<<FOC1C)					//forced output on ch c disabled
	//			;
	//OCR1A = period-1;
	TCNTx = 0;								//reset the timer / counter
	TIFRx |= (1<<OCFxA) | (1<<OCFxB) | (1<<TOVx);		//clear the flag by writing '1' to it
	TIMSKx =	//(0<<TICIE1) |				//input capture isr: disabled
				//(0<<OCIE1C) |				//output compare isr for ch a: disabled
				(0<<OCIExB) |				//output compare isr for ch b: disabled
				(0<<OCIExA) |				//output compare isr for ch c: disabled
				(0<<TOIEx)					//tmr overflow interrupt: disabled
				;
	TCCRxB |=	(prescaler & TMRx_PSMASK)	//prescaler, per the header file
				;
	//now timer1 is running
}

//set dc for channel a
//0b10->clear on compare match
//need to pay attention to the mode bits (wgm13..0) and dc value
void tmr1a_setpr(uint16_t pr) {
	///*_tmr1*/_isr_ptr=isr_ptr;					//reassign tmr1 isr ptr
	//TIFR |= (1<<OCF1A);						//clear the flag by writing '1' to it
	//TIMSK |=	(1<<OCIE1A)					//tmr overflow interrupt: enabled
	//			;

	//IO_OUT(PWM1A_DDR, PWM1A);				//oc1a as output
	_inc_oca = pr - 1;
	OCRxA = TCNTx + _inc_oca;							//set dc
}

//set dc for channel b
//0b10->clear on compare match
//need to pay attention to the mode bits (wgm13..0) and dc value
void tmr1b_setpr(uint16_t pr) {
	///*_tmr1*/_isr_ptr=isr_ptr;					//reassign tmr1 isr ptr
	//TIFR |= (1<<OCF1A);						//clear the flag by writing '1' to it
	//TIMSK |=	(1<<OCIE1A)					//tmr overflow interrupt: enabled
	//			;

	//IO_OUT(PWM1B_DDR, PWM1B);				//oc1b as output

	_inc_ocb = pr - 1;
	OCRxB = TCNTx + _inc_ocb;							//set dc
}

//install user handler for timer1 overflow
void tmr1_act(void (*isr_ptr)(void)) {
	_isrptr_tov=isr_ptr;					//reassign tmr1 isr ptr
	TIFRx |= (0<<OCFxA) | (0<<OCFxB) | (1<<TOVx);		//clear the flag by writing '1' to it
	TIMSKx |=	//(0<<TICIE1) |				//input capture isr: disabled
				//(0<<OCIE1C) |				//output compare isr for ch a: disabled
				(0<<OCIExB) |				//output compare isr for ch b: disabled
				(0<<OCIExA) |				//output compare isr for ch c: disabled
				(1<<TOIEx)					//tmr overflow interrupt: disabled
				;

}

//install user handler
void tmr1a_act(void (*isr_ptr)(void)) {
	_isrptr_oca=isr_ptr;					//reassign tmr1 isr ptr
	TIFRx |= (1<<OCFxA) | (0<<OCFxB) | (0<<TOVx);		//clear the flag by writing '1' to it
	TIMSKx |=	//(0<<TICIE1) |				//input capture isr: disabled
				//(0<<OCIE1C) |				//output compare isr for ch a: disabled
				(0<<OCIExB) |				//output compare isr for ch b: disabled
				(1<<OCIExA) |				//output compare isr for ch c: disabled
				(0<<TOIEx)					//tmr overflow interrupt: disabled
				;

}

//install user handler
void tmr1b_act(void (*isr_ptr)(void)) {
	_isrptr_ocb=isr_ptr;					//reassign tmr1 isr ptr
	TIFRx |= (0<<OCFxA) | (1<<OCFxB) | (0<<TOVx);		//clear the flag by writing '1' to it
	TIMSKx |=	//(0<<TICIE1) |				//input capture isr: disabled
				//(0<<OCIE1C) |				//output compare isr for ch a: disabled
				(1<<OCIExB) |				//output compare isr for ch b: disabled
				(0<<OCIExA) |				//output compare isr for ch c: disabled
				(0<<TOIEx)					//tmr overflow interrupt: disabled
				;

}
