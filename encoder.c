#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "encoder.h"
#define NUM_NOTES 21
#define NUM_TONES 26
#define EEPROM_ADDRESS 100

unsigned volatile char encoder_new_state, encoder_old_state;
unsigned volatile char encoder_changed = 0;  // Flag for state change
unsigned volatile char encoderA, encoderB;
unsigned volatile char encoderVal;
unsigned volatile char encoder_changed_up;
extern unsigned char note_num;
extern unsigned char page_num;
extern int lcd_col;
extern unsigned char notes[NUM_NOTES];
char *letter_notes[NUM_TONES];

void init_encoder(void) {
	PORTC |= (1 << 1); // enable pull-up resistors for rotary encoder
	PORTC |= (1 << 5);
	//Get interrupts working for rotary encoder
	PCICR |= (1 << PCIE1);
	PCMSK1 |= ((1 << PCINT9) | (1 << PCINT13));
	sei();

	//get current state of encoder
	encoderVal = PINC;
	encoderA = (encoderVal & (1 << 1));
	encoderB = (encoderVal & (1 << 5));

	if (!encoderB && !encoderA)
		encoder_old_state = 0;
	else if (!encoderB && encoderA)
		encoder_old_state = 1;
	else if (encoderB && !encoderA)
		encoder_old_state = 2;
	else
		encoder_old_state = 3;

	encoder_new_state = encoder_old_state;
}

void change_note_ifneeded(void) {
	char *p;
	if (encoder_changed) {
		encoder_changed = 0;
		if (encoder_changed_up) {
			int index = notes[note_num]; //numeric value of note (0,13, 17,etc)
			if (index < NUM_TONES-1) {
				notes[note_num] = index+1;
				p = letter_notes[index+1];
				lcd_writedata(*p); //print out that letter string
				lcd_writedata(*(p+1));
				lcd_moveto(1, lcd_col);
				lcd_writedata(*(p+2));
				lcd_moveto(0,lcd_col); //move cursor back
			}
		} else {
			int index = notes[note_num]; //numeric value of note (0,13, 17,etc)
			if (index > 0) {
				notes[note_num] = index - 1;
				p = letter_notes[index-1];
				lcd_writedata(*p);
				lcd_writedata(*(p+1));
				lcd_moveto(1, lcd_col);
				lcd_writedata(*(p+2));
				lcd_moveto(0,lcd_col);
			}
		}
	}
}

ISR(PCINT1_vect) {
	encoderVal = PINC;
	encoderA = (encoderVal & (1 << 1));
	encoderB = (encoderVal & (1 << 5));

	if (encoder_old_state == 0) {
		if (encoderA) {
			encoder_new_state = 1;
			encoder_changed_up = 1;
		}
		else if (encoderB) {
			encoder_new_state = 2;
			encoder_changed_up = 0;
		}
		// Handle A and B inputs for state 0
	}
	else if (encoder_old_state == 1) {
		if (!encoderA) {
			encoder_new_state = 0;
			encoder_changed_up = 0;
		}
		else if (encoderB) {
			encoder_new_state = 3;
			encoder_changed_up = 1;
		}
		// Handle A and B inputs for state 1
	}
	else if (encoder_old_state == 2) {
		if (encoderA) {
			encoder_new_state = 3;
			encoder_changed_up = 0;
		}
		else if (!encoderB) {
			encoder_new_state = 0;
			encoder_changed_up = 1;
		}
		// Handle A and B inputs for state 2
	}
	else {   // old_state = 3
		if (!encoderA) {
			encoder_new_state = 2;
			encoder_changed_up = 1;
		}
		else if (!encoderB) {
			encoder_new_state = 1;
			encoder_changed_up = 0;
		}
		// Handle A and B inputs for state 3
	}

	// If state changed, update the value of old_state,
	// and set a flag that the state has changed.
	if (encoder_new_state != encoder_old_state) {
		encoder_changed = 1;
		encoder_old_state = encoder_new_state;
	}
}