/*
 * PWM.c
 *
 *  Created on: 30.11.2017
 *      Author: dorota
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "Quadrature.h"
#include "Rotation.h"
#include "uartt.h"

extern volatile uint32_t position_counter;	//aktualna pozycja

void wyswietl(char dane[]) {
		uint8_t i = 0;
		while (dane[i]) {
			uartPut(dane[i]);
			i++;
		}
	}
void PWM_Init(void) {

	//fast PWM
	TCCR0A |= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS00) | (1 << CS02);				//prescaler 1024
}

//obliczanie długości toru ruchu w impulsach
uint16_t absolute_movement(int16_t wanted) {

	int16_t movement;

	movement =
			(position_counter > wanted) ?
					(position_counter - wanted) : (wanted - position_counter);

	movement = movement >= (N / 2) ? (N - movement) : (movement);

	return ((N / 2) - movement);
}
volatile int32_t wynik;
//ustawianie prędkości na podstawie odległości do przebycia
void Set_Speed(uint16_t Impulse_Left) {

	if (Impulse_Left < 200){
		OCR0A = 40;

	}

	else if (Impulse_Left > 1500)
		OCR0A = 0xff;

	else {
		OCR0A = ((((uint16_t)(Impulse_Left) * (uint16_t)(43)))/260) + 7;
	}

}

