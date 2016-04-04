#include "avr.h"
#include <avr/io.h>

void
ini_avr(void)
{
	SET_BIT(DDRB, PINB0);
	CLR_BIT(DDRB, PINB1);
}

void
wait_avr(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

int main(){
	ini_avr();
	while(1){
		if(GET_BIT(PINB, PINB1)){
			SET_BIT(PORTB, PINB0);
			//for(long i = 0; i < 500000; ++i);
			wait_avr(500);
			CLR_BIT(PORTB, PINB0);
			//for(long i = 0; i < 500000; ++i);
			wait_avr(500);
		}
	}
}
