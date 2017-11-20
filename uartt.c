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

void uartInit(void) {
	UBRRH = (uint8_t) (MYUBBR >> 8);
	UBRRL = (uint8_t) (MYUBBR);
	UCSRB = (1 << RXEN) | (1 << TXEN);
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

uint8_t i;

void getString(void)			//funkcja odp. za pobieranie tablicy z UARTa
{
	i = 0;

	while (1) {

		str[i] = uartGet();
		if (str[i] == '\n')
			break;			// '\n' znak konca wiersza

		i++;
		i &= 0x0f;					//maska zabezpieczajaca zeby i<15
	}
}

void putInt(uint16_t x) {

}

