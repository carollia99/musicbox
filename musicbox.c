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
#include <string.h>

#include "lcd.h"
#include "adc.h"
#include "encoder.h"

#define NUM_NOTES 21
#define NUM_TONES 26
#define EEPROM_ADDRESS 100

/*
   The note_freq array contains the frequencies of the notes from C3 to C5 in
   array locations 1 to 25.  Location 0 is a zero for rest(?) note.  Used
   to calculate the timer delays.
   */
unsigned int note_freq[NUM_TONES] =
{ 0,   131, 139, 147, 156, 165, 176, 185, 196, 208, 220, 233, 247,
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523};

char *letter_notes[NUM_TONES] = {"   ","C 3","C#3","D 3","D#3","E 3","F 3","F#3","G 3","G#3","A 3","A#3","B 3","C 4","C#4","D 4","D#4","E 4","F 4","F#4","G 4","G#4","A 4","A#4","B 4","C 5"};

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
unsigned char notes[NUM_NOTES] = {17, 17, 18, 20, 20, 18, 17, 15, 13, 13, 15, 17, 17, 15, 15, 0, 0, 0, 0, 0, 0 };
unsigned char testnotes[NUM_NOTES];

void play_note(unsigned short);
void variable_delay_us(int);
void init_TIMER1(void);
void show_initial_screen(void);
void move_cursor_ifneeded(void);
void check_if_select_pressed(void);
void verify_eeprom(void);

int isr_count = 0;
int max_count = 0;
volatile int next_note = 0;

char pages[48];
	
//one d character array, read characters in when arduino loads
//letter_notes is strings - three characters for string
//share location. 

//counting cursor and page
int lcd_col = 1;
unsigned char page_num = 0; // pages are zero-indexed for my ease
unsigned char note_num = 0;

//in musicbox.c, make "encoder.h". declare variables extern in encoder.h. declare variables normally in encoder.c
int main(void) {
	// Initialize various modules and functions
	lcd_init();
	adc_init();
	lcd_writecommand(1);
	DDRB |= (1 << PB4);
	PORTB = 0;
	PORTC |= ((1 << 1) | (1 << 5));
	init_TIMER1();
	init_encoder(); //rotary encoder
	//LED
	DDRB |= (1 << PB3);
	PORTB &= ~( 1 << 3);
	
	//check factory reset
	unsigned char adcval = adc_sample(0);
	if (adcval > 250) {
		//check if all values stored in eeprom work. If not, notes takes default state
		verify_eeprom();
	}
	
	// splash screen and initial screen
	show_initial_screen();
	lcd_show_notes();
	lcd_moveto(0,1);
	

	while (1) { //TODO: rotary encoder bugs
		move_cursor_ifneeded(); // polls checks if button on LCD is pressed, moves cursor/pages
		change_note_ifneeded(); // if rotary encoder was rotated, change note tone 
		check_if_select_pressed();
	}
}

void verify_eeprom(void) {
	char eeprom_good = 1;
	eeprom_read_block(testnotes, (void *) EEPROM_ADDRESS, NUM_NOTES);

	int i;
	for (i = 0; i < NUM_NOTES; i++) {
		if (testnotes[i] == 0xFF || testnotes[i] < 0 || testnotes[i] > 25) {
			eeprom_good = 0;
			break;
		}
	}

	if (eeprom_good) {
		strncpy(notes, testnotes, NUM_NOTES);
	} 
}


void move_cursor_ifneeded(void) {
	unsigned char curadc = adc_sample(0);
	//check_cursor_move(curadc);
	//Check if a button on the LCD was pressed
	if ((curadc > 0 && curadc < 30) || (curadc > 45 && curadc < 60)) {
		_delay_ms(200);
		lcd_col += 2;
		note_num += 1;
		if (lcd_col > 15) {
			lcd_col = 1;
			lcd_moveto(0,0);
			if (page_num == 0) {
				page_num += 1;
				note_num = page_num * 7;
				lcd_show_notes();
			} else if (page_num == 1) {
				page_num += 1;
				note_num = page_num * 7;
				lcd_show_notes();
			}
		}
		lcd_moveto(0, lcd_col);
	} else if ((curadc > 154 && curadc < 160) || (curadc > 95 && curadc < 115)) {
		_delay_ms(200);
		lcd_col -= 2;
		note_num -= 1;
		if (lcd_col < 0) {
			lcd_col = 15;
			lcd_moveto(0,0);
			if (page_num == 1) {
				page_num -= 1;
				note_num = page_num * 7 + 8;
				lcd_show_notes();
			} else if (page_num == 2) {
				page_num -= 1;
				note_num = page_num * 7 + 8;
				lcd_show_notes();
			}
		}
		lcd_moveto(0, lcd_col);
	}
}
/* ------------------------------------------------------------------ */

void check_if_select_pressed(void) {
	unsigned char curadc = adc_sample(0);
		if (curadc > 205 && curadc < 215) {
			_delay_ms(200);
			int i;
			for (i = 0; i < NUM_NOTES; i++) {
				play_note(note_freq[notes[i]]);
				TCCR1B &= ~((1 << CS11) | (1 << CS10));
			}
			eeprom_update_block(notes, (void *) EEPROM_ADDRESS, NUM_NOTES);
			eeprom_update_block(notes, (void *) EEPROM_ADDRESS, NUM_NOTES);
		}
}
/*
   Code for showing notes on the screen and playing the notes.
   */

void show_initial_screen(void) {
	lcd_stringout("Carol Liang");
	_delay_ms(1000);
	lcd_writecommand(1);
	lcd_moveto(0,0);
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
	if (freq == 0) {
		_delay_ms(500);
		return;
	}
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

void init_TIMER2(void) {
}
/*
   Code for initializing TIMER2
   */



