#include <avr/io.h>
#include <util/delay.h>
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