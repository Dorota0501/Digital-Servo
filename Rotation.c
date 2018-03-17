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

/*	INICJALIZACJA PINÓW W PLIKU MAIN.C
 *
 * 	PINY 4 i 5 PORTA określają kierunek obrotów silnika
 * 	PORTA = 0b0010 0000 -> LEWO
 * 	PORTA = 0b0001 0000	-> PRAWO
 * 	PORTA = 0b0011 0000 -> ZATRZYMANIE SILNIKA
 */

extern volatile uint16_t position_counter;	//aktualna pozycja w impulsach
uint16_t given_impulse = 0;					//pozycja zadana w impulsach
uint8_t rotateDirection = ROTATE_RIGHT;	//aktualny kierunek obrotów silnika początkowe przypisanie nie ma znaczenia

//----------------------------FUNKCJA POMOCNICZA -----------------------------------------------
void wyswietl(char dane[]) {
	uint8_t i = 0;
	while (dane[i]) {
		uartPut(dane[i]);
		i++;
	}
}
//----------------------------------------------------------------------------------------------

/*
 * MOVEMENT
 * funkcja opowiada za sprawdzenie i wybranie krótszej drogi obrotu
 * do zadanej pozycji
 *
 * Przykład jest podany w stopniach dla ułatwienia zrozumenia algorytmu
 * Funkcja operuje na impulsach
 *
 * np.
 * current = 350,	wanted = 10
 * 10 - 350 = -340
 * move = -340 + 360 = 20
 */
uint8_t movement(int16_t current, int16_t wanted) {
	int16_t move = wanted - current;
	move = move > 0 ? move : move + N;
	if (move > N / 2)
		return 'l';

	if (move < N / 2)
		return 'r';

	if ((move < 15) || (move > (N - 15)))
		return 's';

	return 'x';			//zgłoszenie błędu
}

void rotateTo(uint16_t given_rotation) {

	/*przeliczanie wartosci zadanej na ilosc impulsów według wzoru
	 *
	 * 				   360 - N
	 * 		wartosc_zadana - zadane_impulsy
	 *
	 * 		zadane_impulsy = (N * wartosc_zadana) / 360
	 *
	 */

	given_impulse = (given_rotation * (N / 40)) / (360 / 40);
	/*
	 * program sam robi promocje przy mnożeniu do typu int czyli do 2 bajtów (16 bitów)
	 * jeżeli jest jedna ze zmiennych używanych w mnożeniu większa do int to wtedy promocja jest
	 * zwiększana do 4 bajtów. Jednak tutaj tak nie było, dlatego konieczne jest rzutowanie przy
	 * mnożeniu
	 * tak:		(uint32_t)obrot_zadany * N
	 * lub:		obrot_zadany * N/360UL
	 *
	 * Żeby uniknąć stosowania rzutowania, zmienne zostały przedzielone przez 40 przez co
	 * podczas mnożenia mieszczą się w zakresie 2 bajtów a wynik pozostaje taki sam
	 */

	//na podstawie funkcji movement jest włączany kierunek obrotów
	switch (movement(position_counter, given_impulse)) {

	case 'r':
		Engine_rotation(ROTATE_RIGHT);
		//ustawiam prędkość w zależności od liczby pozostałych do przebycia impulsów
		Set_Speed(absolute_movement(given_impulse));
		uartPut('r');
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

void Engine_rotation(uint8_t command) {
	//uartPut(command);
	if (rotateDirection != command) {
		PORTD |= 0b00011000; //programowe zatrzymanie silnika w przypadku gdy zmieniam kiertunek obrotów
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

void calibrate(){				//funkcja kalibrująca

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
