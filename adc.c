#include <avr/io.h>

#include "adc.h"


void adc_init(void)
{
	// initialize ADMUX and ADCSRA registers
	ADMUX |= 0b01000000;
	ADMUX |= (1 << ADLAR); 
	ADCSRA |= 0b00000111;
	ADCSRA |= (1 << ADEN);
}

unsigned char adc_sample(unsigned char channel)
{
	// Set ADC input mux bits to 'channel' value
	ADMUX &= ~(0b00001111);
	ADMUX |= (0b00001111 & channel);

	// Convert an analog input and return the 8-bit result
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)) != 0) {}
	unsigned char convertedresult = ADCH;
	return convertedresult;
}
