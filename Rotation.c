/*
 * Rotation.c
 *
 *  Created on: 11.11.2017
 *      Author: dorota
 */
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <stdlib.h>
#include "uartt.h"
#include "Rotation.h"
#include "Quadrature.h"
#include "PWM.h"


extern volatile uint16_t position_counter;	//aktualna pozycja w impulsach
uint16_t given_impulse = 0;					//pozycja zadana w impulsach
uint8_t rotateDirection = ROTATE_RIGHT;	//aktualny kierunek obrotów silnika początkowe przypisanie nie ma znaczenia


//sprawdzenie i wybranie krótszej drogi obrotu do zadanej pozycji
uint8_t movement(int16_t current, int16_t wanted) {
	int16_t move = wanted - current;
	move = move > 0 ? move : move + N;

	if (move < 15 )
		return 's';

	if (move > (N / 2))
		return 'l';

	if (move < (N / 2))
		return 'r';

	return 'x';			//zgłoszenie błędu
}


//przeliczanie wartosci zadanej na ilosc impulsów według wzoru
void rotateTo(uint16_t given_rotation) {

	given_impulse = (given_rotation * (N / 40)) / (360 / 40);

	switch (movement(position_counter, given_impulse)) {

		case 'r':
			Engine_rotation(ROTATE_RIGHT);
			Set_Speed(absolute_movement(given_impulse));
			break;

		case 'l':
			Engine_rotation(ROTATE_LEFT);
			Set_Speed(absolute_movement(given_impulse));
			break;

		case 's':
			Engine_rotation(ROTATE_STOP);
			break;

		case 'x':
			Engine_rotation(ROTATE_STOP);
	}
}

//obrót silnika
void Engine_rotation(uint8_t command) {

	if (rotateDirection != command) {
		PORTD |= 0b00011000; 		//programowe zatrzymanie silnika
	}
	switch (command) {

	case ROTATE_LEFT:
		PORTD |= (1 << PD3);
		PORTD &= ~(1 << PD4);

		break;
	case ROTATE_RIGHT:
		PORTD |= (1 << PD4);
		PORTD &= ~(1 << PD3);

		break;
	default:
		PORTD |= (1 << PD3) | (1 << PD4);
		break;
	}

	rotateDirection = command;		//przypisanie nowego kierunku obrotów
}


//funkcja kalibrująca
void calibrate(){

	Engine_rotation(ROTATE_LEFT);
	OCR0A = 50;								//prędkość kalibracji

	uint8_t temp = (PIND & (1 << PD2));		//zapisuje aktualną wartość pinu pd2
	while(1){

		if((temp != 0) && ((PIND & (1 << PD2)) == 0 ) ){	//sprawdzam czy wystąpiło zbocze opadające
			break;
		}
		temp = (PIND & (1 << PD2));							//zapisuje aktalną wartośc pinu do temp

	}
	position_counter = 0;									//jeżeli wystąpiło zbocze opadające zeruje licznik imp
	Engine_rotation(ROTATE_STOP);							//zatrzymuje silnik
}
