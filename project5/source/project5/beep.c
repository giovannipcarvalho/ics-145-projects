#include "beep.h"
#include "avr.h"
#include <util/delay.h>

void beep_440() {
	int i;
	for(i = 0; i < 110; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(2272);
		CLR_BIT(PORTC, 0);
		_delay_us(2272);
	}
}

void beep_260() {
	int i;
	for(i = 0; i < 65; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(3846);
		CLR_BIT(PORTC, 0);
		_delay_us(3846);
	}
}

void beep_52() {
	int i;
	for(i = 0; i < 13; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(19230);
		CLR_BIT(PORTC, 0);
		_delay_us(19230);
	}
}

void beep_49() {
	int i;
	for(i = 0; i < 12; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(20408);
		CLR_BIT(PORTC, 0);
		_delay_us(20408);
	}
}

void beep_46() {
	int i;
	for(i = 0; i < 11; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(21739);
		CLR_BIT(PORTC, 0);
		_delay_us(21739);
	}
}

void beep_43_7() {
	int i;
	for(i = 0; i < 11; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(22883);
		CLR_BIT(PORTC, 0);
		_delay_us(22883);
	}
}

void beep_41() {
	int i;
	for(i = 0; i < 10; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(24390);
		CLR_BIT(PORTC, 0);
		_delay_us(24390);
	}
}

void beep_98() {
	int i;
	for(i = 0; i < 25; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(10204);
		CLR_BIT(PORTC, 0);
		_delay_us(10204);
	}
}

void beep_110() {
	int i;
	for(i = 0; i < 28; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(9090);
		CLR_BIT(PORTC, 0);
		_delay_us(9090);
	}
}

void beep_123_5() {
	int i;
	for(i = 0; i < 31; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(8097);
		CLR_BIT(PORTC, 0);
		_delay_us(8097);
	}
}

void beep_131() {
	int i;
	for(i = 0; i < 33; i++) {
		SET_BIT(PORTC, 0);
		_delay_us(7634);
		CLR_BIT(PORTC, 0);
		_delay_us(7634);
	}
}