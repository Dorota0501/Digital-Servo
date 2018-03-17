#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "Quadrature.h"
#include "uartt.h"
#include "Rotation.h"

uint8_t old_state = 0;						//poprzednia pozycja kanału enkodera
uint8_t current_state = 0;					//aktualna 	 pozycja kanału enkodera
volatile uint32_t position_counter = 0;		//zliczona liczba impulsów

//		index		value
uint8_t q[] = {
	/*0000*/'!',
	/*0001*/'r',
	/*0010*/'l',
	/*0011*/'!',
	/*0100*/'l',
	/*0101*/'!',
	/*0110*/'!',
	/*0111*/'r',
	/*1000*/'r',
	/*1001*/'!',
	/*1010*/'!',
	/*1011*/'l',
	/*1100*/'!',
	/*1101*/'l',
	/*1110*/'r',
	/*1111*/'!',
};

void Quadrature() {
//---------------------------------INICJALIZACJA REJESTRÓW------------------------------------------------------
//INICJALIZACJA PCINT

	GIMSK |= (1 << PCIE);
	PCMSK |= (1 << PCINT0) | (1 << PCINT1);

//--------------------------------------------------------------------------------------------------------------
	sei();
}

ISR(PCINT_vect)	//przerwania Pin Change Interrupt
{

	current_state = PINB & 0x03;	//zczytywanie stanu na pinach PCINT

	uint8_t index = (old_state << 2) | (current_state);

	if (q[index] == 'r')
		position_counter++;

	else if (q[index] == 'l')
		position_counter--;

	if (position_counter == N)
		position_counter = 0;

	if (position_counter == -1)
		position_counter = N - 1;

	old_state = current_state;
}

