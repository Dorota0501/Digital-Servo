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

extern volatile uint32_t position_counter;	//aktualna pozycja wyznaczana impulsami

void PWM_Init(void) {
	//pin 14  PB2 OC0A timer 16b

	//fast pwm mode
	TCCR0A |= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS00) | (1 << CS02);
	//CS10 prescaler 1024
}

uint16_t absolute_movement(int16_t wanted) {
	/*
	 * Funkcja zwraca ile impulsów pozostało do osiagnięcia zadanej pozycji
	 * jednak nie odlicza ich do 0 lecz do (N/2) dlatego zwracana wartość jest
	 * równa (N/2)-movement
	 * W ten sposób osiagam odliczanie do 0
	 * im mniej impulsów zostało do przebycia tym bliżej 0+ sie znajdujemy
	 *
	 * INSTRUKCJA WARUNKOWA => OPERATOR TRÓJARGUMENTOWY
	 * zmienna = (warunek)? (jeżeli prawda) : (jeżeli fałsz);
	 */
	int16_t movement;

	movement =
			(position_counter > wanted) ?
					(position_counter - wanted) : (wanted - position_counter);

	movement = movement >= (N / 2) ? (N - movement) : (movement);

	return ((N / 2) - movement);
}

void Set_Speed(uint16_t Impulse_Left) {
	/*
	 * Ustawienie prędkości obracania się wału silnika, na podstawie funkcji liniowej
	 * y = (215/1500)*x + 40
	 */

	uartPut('\n');
	char wyniczek[10];
	itoa(Impulse_Left, wyniczek, 10);
	wyswietl(wyniczek);

	if (Impulse_Left < 200)
		OCR0A = 40;

	else if (Impulse_Left > 1500)
		OCR0A = 0xff;

	else {

		/* x-> Impulse_Left
		 * y-> PWM (40-255)
		 *  40 = a*0 +b			_
		 * 255 = a*1500 +b
		 *
		 * 215 = 1500a => a = 215/1500
		 * b = (215/1500) *1500 -255 => b = 40
		 *
		 * y = (215/1500)*x + 40
		 */
		OCR0A = ((Impulse_Left * (215 / 1500)) + 40);
	}

}

