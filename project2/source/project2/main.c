#ifndef F_CPU
#define F_CPU 8000000UL // 1 MHz clock speed
#endif

#include "avr.h"
#include "lcd.h"
#include "keyboard.h"
#include <util/delay.h>
#include <math.h>
#include <string.h>

#define TURN_AM 0
#define TURN_PM 1

int hour = 12, min = 0, sec = 0;
int turn = TURN_AM;
int time_lim[] = {12, 59, 59};
int day = 1, month = 10, year = 1991;
int m_len[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char *week_days[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
	
int refresh = 1;

void timer1_init() {
	// set up timer with prescaler = 64 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS12);
	// initialize counter
	TCNT1 = 0;
	// initialize compare value
	OCR1A = 31249;
	// enable compare interrupt
	TIMSK |= (1 << OCIE1A);
	// enable global interrupts
	sei();
}

void itoa(int n, char *s) {
	int d, u;
	u = n % 10;
	d = (n - u) / 10;
	s[0] = d + 48;
	s[1] = u + 48;
	s[2] = '\0';
}

void itoa4(int n, char *s) {
	int m, c, d, u;
	u = n % 10;
	d = ((n - u) % 100) / 10;
	c = ((n - u - 10 * d) % 1000) / 100;
	m = (n - u - 10 * d - 100 * c) / 1000;
	s[0] = m + 48;
	s[1] = c + 48;
	s[2] = d + 48;
	s[3] = u + 48;
	s[4] = '\0';
}

int atoi(char *str, int size) {
	
	int res = 0; // Initialize result
	int i;
	
	// Iterate through all characters of input string and update result
	for (i = 0; i < size; ++i)
		res = res*10 + str[i] - '0';
	
	return res;
}

int is_leap(unsigned int year) {
	if(year % 4 != 0)
		return 0;
	else if(year % 100 != 0)
		return 1;
	else if(year % 400 != 0)
		return 0;
	else
		return 1;
}

void refresh_time() {
	char h[3], m[3], s[3];
	
	itoa(hour, h);
	itoa(min, m);
	itoa(sec, s);
	
	pos_lcd(1, 0);
	puts_lcd2(h);
	put_lcd(':');
	puts_lcd2(m);
	put_lcd(':');
	puts_lcd2(s);
	
	if(turn == TURN_AM) puts_lcd2(" am");
	else puts_lcd2(" pm");
}

void refresh_date() {
	char d[3], m[3], y[5];
	
	itoa(day, d);
	itoa(month + 1, m);
	itoa4(year, y);
	
	pos_lcd(0, 0);
	puts_lcd2(m);
	put_lcd('/');
	puts_lcd2(d);
	put_lcd('/');
	puts_lcd2(y);
}

void inc_date() {
	day++;
	if(day > m_len[month]) {
		day = 1;
		month++;
		if(month > 11) {
			month = 0;
			year++;
			if(is_leap(year))
				m_len[1] = 29;
			else
				m_len[1] = 28;
		}
	}
}

// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
ISR (TIMER1_COMPA_vect) {
		
	if(refresh) {
		refresh_time();
		refresh_date();
	}
	
	sec++;
	if(sec == 60) {
		sec = 0;
		min++;
		if(min == 60) {
			min = 0;
			hour++;
			if(hour == 12) {
				if(turn == TURN_AM) {
					turn = TURN_PM;
				} else {
					turn = TURN_AM;
					inc_date();
				}
			} else if(hour == 13) {
				hour = 1;
			}
		}
	}
}

int update_value2(char *hms, unsigned char r, unsigned char col, int limit, int nzero, char cnext) {
	char c = 'x';
	char l[3];
	char new_hms[2];
	int i = 0;
	
	new_hms[0] = hms[0];
	new_hms[1] = hms[1];
	
	itoa(limit, l);
	
	while(c != 'D' && c != cnext && c != 'C') {
		pos_lcd(r, col + i);
		c = kb_getc();
		if(c >= (char)48 && c <= (char)57) {
			new_hms[i] = c;
			if(atoi(new_hms, 2) > limit) {
				if(i == 0) {
					if(new_hms[0] > l[0]) { new_hms[0] = hms[0]; i--; }
					else new_hms[1] = l[1];
				} else {
					new_hms[1] = hms[1];
					i--;
				}
			}
			if(nzero && atoi(new_hms, 2) == 0) {
				new_hms[0] = hms[0];
				new_hms[1] = hms[1];
				i--;
			}
			
			hms[0] = new_hms[0];
			hms[1] = new_hms[1];
			pos_lcd(r, col);
			put_lcd(hms[0]);
			put_lcd(hms[1]);
			
			i++;
			if(i >= 2) {
				i = 0;
			}
		}
	}
	hms[2] = '\0';
	if(c == 'D') return 1; // Done
	if(c == cnext) return 0; // Next
	return -1; // Cancel
}

int update_turn(int *t) {
	char c = 'x';
	
	while(c != 'D' && c != 'A' && c != 'C') {
		pos_lcd(1, 9);
		c = kb_getc();
		if(c >= (char)48 && c <= (char)57 || c == '*' || c == '#') {
			if(*t == TURN_AM) {
				*t = TURN_PM;
				puts_lcd2("pm");
			} else {
				*t = TURN_AM;
				puts_lcd2("am");
			}
		}
	}
	
	if(c == 'D') return 1; // Done
	if(c == 'A') return 0; // Next
	return -1; // Cancel
}

int update_year(char *y) {
	char c = 'x';
	int i = 0;
	
	while(c != 'D' && c != 'B' && c != 'C') {
		pos_lcd(0, 6 + i);
		c = kb_getc();
		if(c >= (char)48 && c <= (char)57) {
			y[i] = c;
		}
		
		pos_lcd(0, 6);
		put_lcd(y[0]);
		put_lcd(y[1]);
		put_lcd(y[2]);
		put_lcd(y[3]);
		
		i = i == 3 ? 0 : i + 1;
	}
	
	if(c == 'D') return 1; // Done
	if(c == 'B') return 0; // Next
	return -1; // Cancel
}

int main() {
	char c;
	
	ini_lcd();
	kb_init();
	timer1_init();
	
	refresh_time();
	refresh_date();
	
	while(1) {
		c = kb_getc();
		switch(c) {
			char ctime[3][3];
			char d[3], m[3], y[5];
			int action, t;
			int i, new_len, len;
			
			case 'A':
				itoa(hour, ctime[0]);
				itoa(min, ctime[1]);
				itoa(sec, ctime[2]);
				t = turn;
				
				i = 0;
				refresh = 0;
				show_cursor();
				do {
					if(i != 3)
						action = update_value2(ctime[i], 1, 3 * i, time_lim[i], i == 0 ? 1 : 0, 'A');
					else
						action = update_turn(&t);
						
					if(action == 0)
						i = i >= 3 ? 0 : i + 1;
				} while(action != 1 && action != -1);
				
				if(action == 1) {
					hour = atoi(ctime[0], 2);
					min = atoi(ctime[1], 2);
					sec = atoi(ctime[2], 2);
					turn = t;
				}
				refresh = 1;
				hide_cursor();
				break;
				
			case 'B':
				itoa(month + 1, m);
				itoa(day, d);
				itoa4(year, y);
				
				refresh = 0;
				new_len = m_len[1];
				show_cursor();
				do {
					action = update_value2(m, 0, 0, 12, 1, 'B');
					len = atoi(m, 2) == 2 ? new_len : m_len[atoi(m, 2) - 1];
					if(atoi(d, 2) > len) {
						itoa(len, d);
						pos_lcd(0, 3);
						puts_lcd2(d);
					}
					if(action == 0) {
						action = update_value2(d, 0, 3, len, 1, 'B');
						if(action == 0) {
							action = update_year(y);
							if(is_leap(atoi(y, 4))) {
								new_len = 29;
							} else {
								new_len = 28;
								if(atoi(m, 2) == 2 && atoi(d, 2) == 29) {
									d[0] = '2';
									d[1] = '8';
									pos_lcd(0, 3);
									puts_lcd2("28");
								}
							}
						}
					}
				} while(action != 1 && action != -1);
				
				if(action == 1) {
					month = atoi(m, 2) - 1;
					day = atoi(d, 2);
					year = atoi(y, 4);
					m_len[1] = new_len;
				}
				refresh = 1;
				hide_cursor();
		}
	}
}