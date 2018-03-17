/*
 * main.c

 *
 *  Created on: 09.11.2017
 *      Author: dorota
 */
//#include <avr/iotn2313.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "uartt.h"
#include "Quadrature.h"
#include "Rotation.h"
#include "PWM.h"

uint16_t readValue();
void analyseStr();

extern volatile uint8_t newCommandReceived;
char command;
uint16_t volatile value;

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
	PORTD |= (1 < PD0);						//pull-up rxd

	//INICJALIZACJA PINÓW DO MOSTKA H
	DDRD |= (1 << PD4) | (1 << PD3);		//uruchomienie pinów 7 i 8
	/*
	 * 	PINY 3 i 4 PORTD określają kierunek obrotów silnika
	 * 	PORTD = 0b0001 0000 -> LEWO
	 * 	PORTD = 0b0000 1000	-> PRAWO
	 * 	PORTD = 0b0001 1000 -> ZATRZYMANIE SILNIKA
	 */

	//INICJALIZACJA PINÓW DO PWM
	DDRB |= (1 << PB2);

	//INICJALIZACJA PINU DO KONTAKTRONU INT0
	DDRD |= (0 << PD2);
	PORTD |= (1 << PD2);				//pull up

	uartInit();							//inicjalizacja UARTA
	Quadrature();						//inicjalizacja przerwań na PCINT
	sei();
	//umożlwienie wykonywania przerwań w programie
	PWM_Init();							//inicjalizacja PWM

	while (1) {
		/*
		 * Po otrzymaniu nowej komendy, analizuje ją i zeruje flage otrzymania komendy,
		 * następnie zgodnie z nową komendą wykonuje określone funkcje.
		 */

		if (newCommandReceived) {//jezeli flaga otrzymania nowej komendy zostanie ustawiona na 1

			analyseStr();					//analiza otrzymanej komendy
			newCommandReceived = 0;	//zerowanie flagi otrzymania nnowej komendy
			//wyswietl(str);
		}
		switch (command) {
		case 'c':
			calibrate();		// funkcja kalibracja
			command = 0;
			break;

		case 'o':
			rotateTo(value);
			break;

		case 's':
			//bezwzgledny stop
			Engine_rotation(ROTATE_STOP);
			break;

		case 'r':
			//obrot w prawo z zadana predkoscia
			Engine_rotation(ROTATE_RIGHT);
			OCR0A 	=	value;
			break;

		case 'l':
			//obrot w lewo z zadana predkoscia
			Engine_rotation(ROTATE_LEFT);
			OCR0A 	=	value;
			break;


		}

	}

	return 0;
}

uint16_t readValue() {
	return (((str[3] - '0') * 100) + ((str[4] - '0') * 10)
			+ ((str[5] - '0') * 1));
} //przy obliczaniu musimy odjąć znak'0' bo inaczej dostajemy numer danego znaku w tablicy ASCI
//mozna to ominąć odejmując wartość 48 czyli znak'0' bo za nim zaczyna sie adresacja cyfr

uint8_t myAddr() {
	// funkckcja zwracajaca 1 jeśli adres w str zgadza się z adr na dipswitch lub adr w str == *
	// w przeciwnym przypadku zwraca 0
	return 1; //teraz jeszcze noe sprawdzamy adresu, bo nam się nie chce tego pisać
}

void analyseStr() {

	if (str[0] != 's')
		return; // jeśli komunikat nie dla modułu serwo, to czekamy na kolejny komunikat (wracamy na początek pętli)

	if (!myAddr())
		return;

	switch (str[2]) {
	case 'c':					// kalibracja
		command = 'c';

		break;

	case 'o':
		command = 'o';			// obrót
		value = readValue();
		break;

	case 's': 					// stop
		command = 's';
		break;

	case 'r':					// obrót w prawo z zadana predkościa
		command = 'r';
		value = readValue();
		break;

	case 'l':					// obrót w lewo z zadana predkościa
		command = 'l';
		value = readValue();
		break;

	case 'x':					//zgłoszony został bład, silnik zostanie zatrzymany
		command = 's';			//bezwględny stop
		break;

	}

}
