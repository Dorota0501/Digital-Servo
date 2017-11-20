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

/*	INICJALIZACJA PINÓW W PLIKU MAIN.C
 *
 * 	PINY 4 i 5 PORTA określają kierunek obrotów silnika
 * 	PORTA = 0b0010 0000 -> LEWO
 * 	PORTA = 0b0001 0000	-> PRAWO
 * 	PORTA = 0b0011 0000 -> ZATRZYMANIE SILNIKA
 */

extern volatile uint32_t position_counter;
uint32_t zadane_impulsy = 0;
uint8_t rotateDirection = ROTATE_RIGHT; //aktualny kierunek obrotów silnika początkowe przypisanie nie ma znaczenia

void rotateTo(uint16_t obrot_zadany) {

	/*przeliczanie wartosci zadanej na ilosc impulsów według wzoru
	 *
	 * 				   360 - N
	 * 		wartosc_zadana - zadane_impulsy
	 *
	 * 		zadane_impulsy = (N * wartosc_zadana) / 360
	 *
	 */

	char wyniczek[10];
	zadane_impulsy = (uint32_t) obrot_zadany * N / 360;
	/*
	 * program sam robi promocje przy mnożeniu do typu int czyli do 2 bajtów (16 bitów)
	 * jeżeli jest jedna ze zmiennych używanych w mnożeniu większa do int to wtedy promocja jest
	 * zwiększana do 4 bajtów. Jednak tutaj tak nie było, dlatego konieczne jest rzutowanie przy
	 * mnożeniu
	 * tak:		(uint32_t)obrot_zadany * N
	 * lub:		obrot_zadany * N/360UL
	 *
	 */
	ltoa(zadane_impulsy, wyniczek, 10);

	void wyswietl(char dane[]) {
		uint8_t i = 0;
		while (1) {
			uartPut(dane[i]);
			if (dane[i] == '\n')
				break;
			i++;
		}
	}

	uartPut('\n');
	wyswietl(wyniczek);
	uartPut('\n');
	uint32_t movement(uint32_t current, uint32_t wanted) {
		int32_t move = wanted - current;
		move = move >= 0 ? move : move + N;
		return move;
	}

	uint32_t pozycja;
	pozycja = movement(position_counter, zadane_impulsy);
	ltoa(pozycja, wyniczek, 10);

	wyswietl(wyniczek);

	if (movement(position_counter, zadane_impulsy) > (N/2)) {
		Engine_rotation(ROTATE_RIGHT);
		uartPut('\n');
		uartPut('r');
	} else
		Engine_rotation(ROTATE_LEFT);

	while (1) {

		if (position_counter == zadane_impulsy) {
			Engine_rotation(ROTATE_STOP);
			break;
		}
	}

}

void Engine_rotation(uint8_t command) {
	//uartPut(command);
	if (rotateDirection != command) {
		PORTD |= 0b00001100; //programowe zatrzymanie silnika w przypadku gdy zmieniam kiertunek obrotów
	}
	switch (command) {

	case ROTATE_LEFT:
		PORTD |= (1 << PD2);
		PORTD &= ~(1 << PD3);

		break;
	case ROTATE_RIGHT:
		PORTD |= (1 << PD3);
		PORTD &= ~(1 << PD2);

		break;
	default:
		PORTD |= (1 << PD2) | (1 << PD3);
		break;
	}

	rotateDirection = command;		//przypisanie nowego kierunku obrotów
}
