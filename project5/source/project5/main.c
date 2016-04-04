#include "avr.h"
#include "lcd.h"
#include "keyboard.h"
#include "beep.h"
#include <math.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#define EMPTY         0
#define PLAYER        1
#define P_CROUCH      2
#define P_JUMP        3
#define P_JUMP_BLOCK  4
#define UP_BLOCK      5
#define P_UP_BLOCK    6
#define BLOCK         255

unsigned char map[17] = "0000000000000000";
int playerCrouch = 0;
int playerJump = 0;
char lastStructure = 0;
char gameOver = 0;
int points = 0;

void timer1_init() {
	// set up timer with prescaler = 64 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS12);
	// initialize counter
	TCNT1 = 0;
	// initialize compare value
	OCR1A = 30000;//31249;
	// enable compare interrupt
	TIMSK |= (1 << OCIE1A);
	// enable global interrupts
	//sei();
}

void write_col(unsigned char pos, char upper, char lower) {
	pos_lcd(0, pos);
	put_lcd(upper);
	pos_lcd(1, pos);
	put_lcd(lower);
}

void render(unsigned char display[]) {
	int i;
	for(i = 0; i < 16; i++) {
		switch(display[i]) {
			default:
			case EMPTY:
				write_col(i, ' ', ' ');
				break;
			case PLAYER:
				write_col(i, ' ', PLAYER);
				break;
			case P_CROUCH:
				write_col(i, ' ', P_CROUCH);
				break;
			case P_JUMP:
				write_col(i, PLAYER, ' ');
				break;
			case P_JUMP_BLOCK:
				write_col(i, PLAYER, BLOCK);
				break;
			case UP_BLOCK:
				write_col(i, BLOCK, UP_BLOCK);
				break;
			case P_UP_BLOCK:
				write_col(i, BLOCK, P_UP_BLOCK);
				break;
			case BLOCK:
				write_col(i, ' ', BLOCK);
		}
		
	}
}

char nextStructure(){
	if(lastStructure != EMPTY){
		lastStructure = EMPTY;
		return EMPTY;
	} else {
		char c;
		int r = rand() % 3;
		
		if(r == 0)
			c = EMPTY;
		else if(r == 1)
			c = BLOCK;
		else
			c = UP_BLOCK;
			
		lastStructure = c;
		return c;
	}
}

void scrollMap(){
	//scroll map
	for(int i = 0; i < 15; ++i){
		map[i] = map[i+1];
	}
	
	if(map[1] == P_UP_BLOCK)
		map[1] = UP_BLOCK;
	else if(map[1] == P_JUMP_BLOCK)
		map[1] = BLOCK;
	else if(map[1] == PLAYER || map[1] == P_JUMP || map[1] == P_CROUCH)
		map[1] = EMPTY;
	
	map[15] = nextStructure();
}

// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
ISR (TIMER1_COMPA_vect) {
	scrollMap();
	
	if((map[2] == BLOCK && playerJump == 0)) {
		gameOver = 1;
	}
	if(map[2] == UP_BLOCK && playerCrouch == 0) {
		gameOver = 1;
	}
	
	if(playerJump) {
		beep_440();
		if(map[2] == BLOCK)
			map[2] = P_JUMP_BLOCK;
		else
			map[2] = P_JUMP;
	} else if(playerCrouch) {
		beep_260();
		if(map[2] == UP_BLOCK)
			map[2] = P_UP_BLOCK;
		else
			map[2] = P_CROUCH;
	} else
		map[2] = PLAYER;
		
	render(map);
	
	playerJump = 0;
	playerCrouch = 0;
	
	OCR1A = OCR1A < 5000 ? OCR1A : OCR1A - 50;
	points++;
}

int main() {

	char block_person[8] = {
		0b11111,
		0b11111,
		0b00000,
		0b01110,
		0b01110,
		0b00100,
		0b01110,
		0b01010
	};
	
	char person[8] = {
		0b01110,
		0b01110,
		0b00100,
		0b11111,
		0b00100,
		0b01010,
		0b01010,
		0b01010
	};
	
	char block[8] = {
		0b11111,
		0b11111,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000
	};
	
	char person_down[8] = {
		0b00000,
		0b00000,
		0b00000,
		0b01110,
		0b01110,
		0b00100,
		0b01110,
		0b01010
	};
	
	SET_BIT(DDRC, 0);

	ini_lcd();
	kb_init();
	
	new_char(PLAYER, person);
	new_char(P_CROUCH, person_down);
	new_char(UP_BLOCK, block);
	new_char(P_UP_BLOCK, block_person);
	
	timer1_init();
	
	srand(1);
	
	while(1){
		pos_lcd(0, 0);
		puts_lcd2("  Press any key ");
		pos_lcd(1, 0);
		puts_lcd2("    to start    ");
		kb_getc();
		points = 0;
		
		beep_110();
		_delay_ms(500);
		beep_98();
		_delay_ms(500);
		beep_110();
		_delay_ms(500);
		beep_123_5();
		_delay_ms(500);
		beep_131();
		
		sei();
		
		while(!gameOver) {
			char c = kb_getc();
			if(c == 'A') {
				playerJump = 1;
				playerCrouch = 0;
			} else if(c == 'B') {
				playerCrouch = 1;
				playerJump = 0;
			}
		}
		cli();
		gameOver = 0;
		OCR1A = 30000;
		
		int i;
		
		for(i = 0; i < 17; i++) {
			map[i] = EMPTY;
		}
		
		pos_lcd(0, 0);
		for(i = 0; i < 16; i++) {
			put_lcd(255);
		}
		pos_lcd(1, 0);
		for(i = 0; i < 16; i++) {
			put_lcd(255);
		}
		
		beep_52();
		_delay_ms(500);
		beep_49();
		_delay_ms(500);
		beep_46();
		_delay_ms(500);
		beep_43_7();
		_delay_ms(500);
		beep_41();
		
		char pts[10];
		itoa(points, pts, 10);
		
		clr_lcd();
		pos_lcd(0, 0);
		puts_lcd2("    GAME OVER   ");
		pos_lcd(1, 0);
		puts_lcd2(pts);
		puts_lcd2(" points");
		
		kb_getc();
	}
	
}