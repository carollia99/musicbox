/********************************************
 *
 *  Name: Carol Liang
 *  Email: carollia@usc.edu
 *  Section: W5
 *  Assignment: Project - Music Box
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include "lcd.h"
#include "adc.h"

#define NUM_NOTES 21
#define NUM_TONES 26

/*
   The note_freq array contains the frequencies of the notes from C3 to C5 in
   array locations 1 to 25.  Location 0 is a zero for rest(?) note.  Used
   to calculate the timer delays.
   */
unsigned int note_freq[NUM_TONES] =
{ 0,   131, 139, 147, 156, 165, 176, 185, 196, 208, 220, 233, 247,
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};

/* Some sample tunes for testing */
/*
   The "notes" array is used to store the music that will be played.  Each value
   is an index into the tone value in the note_freq array.
   */

// G, A, F, F(octive lower), C     Close Encounters
//unsigned char notes[NUM_NOTES] = {20, 22, 18, 6, 13};

// D D B C A B G F# E D A C A B    Fight On
//unsigned char notes[NUM_NOTES] = {15, 15, 12, 13, 10, 12, 8, 7, 5, 3, 10, 13, 10, 12};

// E E F G G F E D C C D E E D D   Ode to Joy
unsigned char notes[NUM_NOTES] = {17, 17, 18, 20, 20, 18, 17, 15, 13, 13, 15, 17, 17, 15, 15 };

void play_note(unsigned short);
void variable_delay_us(int);
void init_TIMER1(void);
void show_initial_screen(void);

int isr_count = 0;
int max_count = 0;
volatile int next_note = 0;
char initial_page[16] = " E E F G G F E >";
char second_page[16] = "<D C C D E E D >";
char last_page[16] = "<D D D D D D ";

int main(void) {
	// Initialize various modules and functions
	lcd_init();
	adc_init();
	lcd_writecommand(1);
	DDRB |= (1 << PB4);
	PORTB = 0;
	PORTC |= ((1 << 1) | (1 << 5));
	init_TIMER1();
	
	//Initialize variables
	int lcd_col = 1;
	int page_num = 1;
	
	show_initial_screen(); // splash screen and initial page

	int i;
	for (i = 0; i < 21; i++) {
		play_note(note_freq[i]);
	}

	
	/*while (1) {
		unsigned char curadc = adc_sample(0);
		//Check if a button on the LCD was pressed
		if (curadc > 0 && curadc < 5) {
			_delay_ms(200);
			lcd_col += 2;
			if (lcd_col > 15) {
				lcd_col = 1;
				lcd_moveto(0,0);
				if (page_num == 1) {
					lcd_stringout(second_page);
					page_num += 1;
				} else if (page_num == 2) {
					lcd_stringout(last_page);
					page_num += 1;
				}
			}
			lcd_moveto(0, lcd_col);
		} else if (curadc > 154 && curadc < 160) {
			_delay_ms(200);
			lcd_col -= 2;
			if (lcd_col < 0) {
				lcd_col = 15;
				lcd_moveto(0,0);
				if (page_num == 2) {
					lcd_stringout(initial_page);
					page_num -= 1;
				} else if (page_num == 3) {
					lcd_stringout(second_page);
					page_num -= 1;
				}
			}
			lcd_moveto(0, lcd_col);
		}*/

		/* If rotary encoder was rotated, change note tone */

	//}

	while (1) {                 // Loop forever


	}
}


/* ------------------------------------------------------------------ */

/*
   Code for showing notes on the screen and playing the notes.
   */
   
void show_initial_screen(void) {
	lcd_stringout("Carol Liang");
	_delay_ms(1000);
	lcd_writecommand(1);
	lcd_moveto(0,0);
	lcd_stringout(initial_page);
}

/* ------------------------------------------------------------------ */

/*
   Code for initializing TIMER1 and its ISR
 */

void init_TIMER1(void) {
	// enable timer interrupt
	TIMSK1 |= (1 << OCIE1A);
	TCCR1B |= (1 << WGM12);
	sei();
}

void play_note(unsigned short freq) // in here, configure timer module
{
	int ocr1a_val = (16000000/(2*freq)) / 64;
	OCR1A = ocr1a_val;
	
	max_count = freq;
	
	//prescalar = 64
	TCCR1B |= ((1 << CS11) | (1 << CS10)); 
	
	while (1) {
		if (next_note == 1) {
			next_note = 0;
			return;
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	PORTB = PORTB ^ (0b00010000); // invert PB4
	isr_count++;
	if (isr_count > max_count) {
		isr_count = 0;
		next_note = 1;
	}
}

/* ------------------------------------------------------------------ */

/*
   Code for initializing TIMER2
   */



