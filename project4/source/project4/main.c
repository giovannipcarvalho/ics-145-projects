#include "avr.h"
#include "lcd.h"
#include <math.h>

int n = 0;
float max = 0;
float min = 5;
float avg = 0;

void addv(float v) {
	n++;
	max = v > max ? v : max;
	min = v < min ? v : min;
	avg = (((float)n - 1) / (float)n * avg) + (1 / (float)n) * v;
}

float convertv(int v) {
	return (float)v / 1023 * 5;
}

// reverses a string 'str' of length 'len'
void reverse(char *str, int len) {
	int i = 0, j = len - 1, temp;
	
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}

 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
 int intToStr(int x, char str[], int d) {
	 int i = 0;
	 while (x) {
		 str[i++] = (x % 10) + '0';
		 x = x / 10;
	 }
	 
	 // If number of digits required is more, then
	 // add 0s at the beginning
	 while (i < d)
	 str[i++] = '0';
	 
	 reverse(str, i);
	 str[i] = '\0';
	 return i;
 }

// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint) {
	// Extract integer part
	int ipart = (int)n;
	
	// Extract floating part
	float fpart = n - (float)ipart;
	
	// convert integer part to string
	int i = intToStr(ipart, res, 0);
	
	// check for display option after point
	if (afterpoint != 0) {
		res[i] = '.';  // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}

void InitADC() {
	ADMUX = (1 << REFS0); // For Aref=AVcc;
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Prescalar div factor =128
}

uint16_t ReadADC(uint8_t ch) {
	//Select ADC Channel ch must be 0-7
	ch = ch & 0b00000111;
	ADMUX |= ch;

	//Start Single conversion
	ADCSRA |= (1 << ADSC);

	//Wait for conversion to complete
	while(!(ADCSRA & (1 << ADIF)));

	//Clear ADIF by writing one to it
	ADCSRA |= (1 << ADIF);

	return(ADC);
}

void timer1_init() {
	// set up timer with prescaler = 64 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
	// initialize counter
	TCNT1 = 0;
	// initialize compare value
	OCR1A = 62499;
	// enable compare interrupt
	TIMSK |= (1 << OCIE1A);
	// enable global interrupts
	//sei();
}

// this ISR is fired whenever a match occurs
// hence, toggle led here itself..
ISR (TIMER1_COMPA_vect) {
	uint16_t v;
	float volt;
	char s[10];
	
	v= ReadADC(0);
	volt = convertv(v);
	addv(volt);
	
	clr_lcd();
	
	ftoa(volt, s, 2);
	puts_lcd2("I:");
	puts_lcd2(s);
	
	puts_lcd2("  AVG:");
	ftoa(avg, s, 2);
	puts_lcd2(s);
		
	pos_lcd(1, 0);
	
	puts_lcd2("Mi:");
	ftoa(min, s, 2);
	puts_lcd2(s);
		
	puts_lcd2("  Ma:");
	ftoa(max, s, 2);
	puts_lcd2(s);
		
	pos_lcd(0, 0);
}

void wait_btn() {
	int btn = 0;
	int hold = 0;
	
	while(1) {
		btn = btn << 1;
		if(GET_BIT(PINA, 1)) {
			btn = btn | 1;
		}
		if(btn == 0xff) {
			hold = 1;
		} else if(btn == 0 && hold == 1) {
			return;
		}
		wait_avr(1);
	}
}


int main() {

	int tmren = 0;

	ini_lcd();
	InitADC();
	timer1_init();
	cli();
	
	CLR_BIT(DDRA, 1);
	
	puts_lcd2("I:----  AVG:----");
	pos_lcd(1, 0);
	puts_lcd2("Mi:----  Ma:----");
	
	while(1) {
		wait_btn();
		if(tmren) {
			cli();
			tmren = 0;
		}
		else {
			sei();
			tmren = 1;
		}
		
	}
}