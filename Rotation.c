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
unsigned int iterator = 0;
char wynik[10];
//void wyswietl(char dane[]) {
//	uint8_t i = 0;
//	while (dane[i]) {
//		uartPut(dane[i]);
//		i++;
//	}
//}
//sprawdzenie i wybranie krótszej drogi obrotu do zadanej pozycji
uint8_t movement(int16_t current, int16_t wanted) {
	int16_t move = wanted - current;
	move = move > 0 ? move : move + N;
	if (iterator%30 == 0)
	{
		ltoa(position_counter, wynik ,10);
		wyswietl(wynik);
		uartPut(' ');
	}

	iterator++;

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

	char c = movement(position_counter, given_impulse);
	//uartPut(c);
	switch (c) {

		case 'r':

//			if((TCNT0 & 0xff) % 50 == 0)
//			{
//				PORTB ^= (1<<PB3);
//			}
				//uartPut(TCNT0 & 0xff);
			//uartPut('\n');
			Engine_rotation(ROTATE_RIGHT);
			Set_Speed(absolute_movement(given_impulse));
			break;

		case 'l':
//			if((TCNT0 & 0xff) % 10 == 0)
//			{
//				PORTB ^= (1<<PB3);
//			}
			//PORTB |= (1<<PB3);
			Engine_rotation(ROTATE_LEFT);
			Set_Speed(absolute_movement(given_impulse));
			break;

		case 's':
			PORTB &= ~(1<<PB3);
			Engine_rotation(ROTATE_STOP);
			break;

		case 'x':
			PORTB &= ~(1<<PB3);
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
