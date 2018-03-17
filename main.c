/*
 * main.c

 *
 *  Created on: 09.11.2017
 *      Author: dorota
 */

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


//---------------------------------INICJALIZACJA PINÓW-------------------------------------

	// INICJALIZACJA PINOW DO PCINT
	DDRB |= (0 << PB0) | (0 << PB1);
	PORTB |= (1 << PB0) | (1 << PB1);

	//INICJALIZACJA PINÓW UART
	DDRD = (0 << PD0) | (1 << PD1);
	PORTD |= (1 < PD0);

	//INICJALIZACJA PINÓW DO MOSTKA H
	DDRD |= (1 << PD4) | (1 << PD3);

	//INICJALIZACJA PINÓW DO PWM
	DDRB |= (1 << PB2);

	//INICJALIZACJA PINU DO KONTAKTRONU INT0
	DDRD |= (0 << PD2);
	PORTD |= (1 << PD2);

//-------------------------------------------------------------------------------------------

	uartInit();							//inicjalizacja UART'a
	Quadrature();						//inicjalizacja przerwań na PCINT
	sei();
	PWM_Init();							//inicjalizacja PWM



	while (1) {

		if (newCommandReceived) {
			analyseStr();				//analiza otrzymanej komendy
			newCommandReceived = 0;		//zerowanie flagi otrzymania nowej komendy
		}

		switch (command) {
			case 'c':
				calibrate();			// funkcja kalibracja
				command = 0;
				break;

			case 'o':
				rotateTo(value);		//funkcja obrotu
				break;

			case 's':					//bezwzgledny stop
				Engine_rotation(ROTATE_STOP);
				break;

			case 'r':					//obrot w prawo z zadana predkoscia
				Engine_rotation(ROTATE_RIGHT);
				OCR0A 	=	value;
				break;

			case 'l':					//obrot w lewo z zadana predkoscia
				Engine_rotation(ROTATE_LEFT);
				OCR0A 	=	value;
				break;
		}

	}

	return 0;
}

//odczytywanie wartości z UART'a
uint16_t readValue() {

//	volatile int value = atoi(str+2);
//	return value;
	return (((str[3] - '0') * 100) + ((str[4] - '0') * 10)
			+ ((str[5] - '0') * 1));

}

//sprawdzanie czy komenda jest zaadresowana do odpowiedniego podzespołu
uint8_t myAddr() {
	return 1;
}



//analiza odebranej komendy
void analyseStr() {

	//sprawdzam czy adresacja jest odpowiednia
	if (str[0] != 's')
		return;

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
