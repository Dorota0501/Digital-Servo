/*
 * uart.c

 *
 *  Created on: 09.11.2017
 *      Author: dorota
 */
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <stdlib.h>
#include "uartt.h"
//#include "Rotation.h"

uint8_t volatile newCommandReceived = 0;

void uartInit(void) {
	UBRRH = (uint8_t) (MYUBBR >> 8);
	UBRRL = (uint8_t) (MYUBBR);
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	UCSRC = (1 << USBS) | (3 << UCSZ0);		// 2 bity stopu, 8 bit danych
}

void uartPut(uint8_t c) {
	while (!(UCSRA & (1 << UDRE)))
		;
	UDR = c;
}

uint8_t uartGet() {
	while (!(UCSRA & (1 << RXC)))
		;
	return UDR;
}

uint8_t i = 0;

//przerwanie zakończenia odbierania danych z uarta
//odpowiada za pobieranie tablicy z UARTa
ISR(USART_RX_vect) {

	str[i] = UDR;
	if (str[i] == '\n') { // '\n' znak konca wiersza
		newCommandReceived = 1;
		i = 0;
		return;
	}

	i++;
	i &= 0x0f;					//maska zabezpieczajaca zeby i<15

}
