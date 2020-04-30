/********************************************
 *
 *  Name:
 *  Email:
 *  Section:
 *  Assignment: Project - Music Box
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>

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



int main(void) {

    // Initialize various modules and functions


    // Show splash screen



    while (1) {
	/* Check if a button on the LCD was pressed */



	/* If rotary encoder was rotated, change note tone */


    }

    while (1) {                 // Loop forever
    }
}


/* ------------------------------------------------------------------ */

/*
  Code for showing notes on the screen and playing the notes.
*/





/* ------------------------------------------------------------------ */

/*
  Code for initializing TIMER1 and its ISR
*/




ISR(TIMER1_COMPA_vect)
{



}

/* ------------------------------------------------------------------ */

/*
  Code for initializing TIMER2
*/


