#include "avr.h"
#include "keyboard.h"
#include <util/delay.h>

char kb[16] = {0};
char kb_state[16] = {KB_RELEASED};

void kb_init() {
	KB_DDR = 0x0f;
	KB_PORT = 0x00;
}

void kb_update() {
	int i, j;
	for(i = 0; i < 4; i++) {
		SET_BIT(KB_PORT, i);
		NOP(); NOP(); NOP();
		for(j = 4; j < 8; j++) {
			kb[i * 4 + j - 4] = kb[i * 4 + j - 4] << 1;
			if(GET_BIT(KB_PIN, j)) {
				kb[i * 4 + j - 4] = kb[i * 4 + j - 4] | 1;
			}
		}
		CLR_BIT(KB_PORT, i);
	}
}

char kb_getc() {
	int i;
	while(1) {
		kb_update();
		for(i = 0; i < 16; i++) {
			if(kb[i] == (char)0xff) {
				kb_state[i] = KB_HOLD;
			}
			else {
				if(kb[i] == (char)0x00) {
					if(kb_state[i] == KB_HOLD) {
						kb_state[i] = KB_RELEASED;
						return kb_pos2c(i);
					}
					kb_state[i] = KB_RELEASED;
				}
			}
		}
		_delay_us(100);
	}
}

char kb_pos2c(int pos) {
	switch(pos) {
		case 0:  return '1';
		case 1:  return '2';
		case 2:  return '3';
		case 3:  return 'A';
		case 4:  return '4';
		case 5:  return '5';
		case 6:  return '6';
		case 7:  return 'B';
		case 8:  return '7';
		case 9:  return '8';
		case 10: return '9';
		case 11: return 'C';
		case 12: return '*';
		case 13: return '0';
		case 14: return '#';
		case 15: return 'D';
		default: return 'x';
	}
}


