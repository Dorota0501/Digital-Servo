/*
 * main.c

 *
 *  Created on: 09.11.2017
 *      Author: dorota
 */
//#include <avr/iotn2313.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <stdlib.h>

#include "uartt.h"
#include "Quadrature.h"
#include "Rotation.h"

int main(void) {

//---------------------------------INICJALIZACJA PINÓW------------------------------------------------------
	// INICJALIZACJA PINOW DO PCINT
	DDRB |= (0 << PB0) | (0 << PB1);	//PCINT0 i PCINT1 ustawiam jako wejscia
	PORTB |= (1 << PB0) | (1 << PB1);		//PCINT0 i PCINT1 pull-up
	/*	INICJALIZACJA PINÓW DO PCINT
	 *
	 * 	PINY 4 i 5 PORTA określają kierunek obrotów silnika
	 * 	PORTA = 0b0010 0000 -> LEWO
	 * 	PORTA = 0b0001 0000	-> PRAWO
	 * 	PORTA = 0b0011 0000 -> ZATRZYMANIE SILNIKA
	 */

	//INICJALIZACJA PINÓW UART
	DDRD = (0 << PD0) | (1 << PD1);			//rxd=0, txd=1
	PORTD |= (1<PD0);						//pull-up rxd

	//INICJALIZACJA PINÓW DO MOSTKA H
	DDRD |= (1 << PD2) | (1 << PD3);		//uruchomienie pinów 6 i 7
	/*
	 * 	PINY 4 i 5 PORTA określają kierunek obrotów silnika
	 * 	PORTA = 0b0010 0000 -> LEWO
	 * 	PORTA = 0b0001 0000	-> PRAWO
	 * 	PORTA = 0b0011 0000 -> ZATRZYMANIE SILNIKA
	 */

//---------------------------------INICJALIZACJA REJESTRÓW------------------------------------------------------
	//INICJALIZACJA PCINT
	GIMSK |= (1 << PCIE);//włączam przerwania przy zmianie stanu na pinach PCINT1 i PCINT0
	PCMSK |= (1 << PCINT0) | (1 << PCINT1);	// określam które piny mają być ustaniowne jako PCINT

//--------------------------------------------------------------------------------------------------------------

	uartInit();							//inicjalizacja UARTA

	uint8_t myAddr() {
		// funkckcja zwracajaca 1 jeśli adres w str zgadza się z adr na dipswitch lub adr w str == *
		// w przeciwnym przypadku zwraca 0
		return 1; //teraz jeszcze noe sprawdzamy adresu, bo nam się nie chce tego pisać
	}

	Quadrature();
	while (1) {

		getString();

		if (str[0] != 's')
			continue; // jeśli komunikat nie dla modułu serwo, to czekamy na kolejny komunikat (wracamy na początek pętli)

		if (!myAddr())
			continue;

		if (str[2] == 'c') {
			//calibrate();
			continue;
		}

		if (str[2] == 's')
			rotateTo(
					((str[3] - '0') * 100) + ((str[4] - '0') * 10)
							+ ((str[5] - '0') * 1));
		//przy obliczaniu musimy odjąć znak'0' bo inaczej dostajemy numer danego znaku w tablicy ASCI
		//mozna to ominąć odejmując wartość 48 czyli znak'0' bo za nim zaczyna sie adresacja cyfr

	}

	return 0;
}

