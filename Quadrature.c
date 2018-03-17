#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <stdlib.h>

#include "Quadrature.h"
#include "uartt.h"
#include "Rotation.h"

uint8_t old_state = 0;						//poprzednia pozycja kanału enkodera
uint8_t current_state = 0;					//aktualna 	 pozycja kanału enkodera
volatile uint32_t position_counter = 0;

/*
 * old_state i current_state pozwalają okreslić w którą strone kręci się silnik
 * old_state przyjmuje pozycje z jednego kanału a current_state z drugiego dzięki temu
 * wiem który kazał jest kanałem prowadzącym. Kierunek zawiera się w tablicy q[index]
 * index = (old_state << 2) | (current_state)
 */

/*
 * position_counter określa aktualne położenie silnka dzięki inkrementacji i dekrementacji
 * jego wartości w zależności od kierunku jego obrotów
 */

//		index		value
uint8_t q[] = {
	/*0000*/'!',
	/*0001*/'r',
	/*0010*/'l',
	/*0011*/'!',
	/*0100*/'l',
	/*0101*/'!',
	/*0110*/'!',
	/*0111*/'r',
	/*1000*/'r',
	/*1001*/'!',
	/*1010*/'!',
	/*1011*/'l',
	/*1100*/'!',
	/*1101*/'l',
	/*1110*/'r',
	/*1111*/'!',

	/*
	 * index = XXZZ -  XX-old_state, ZZ-current_state
	 * dla każdego zestawienia tych wartości tablica zwraca kierunek obrotów silnika
	 *
	 * value '!' - błąd (niektóre wartości nie mogą nastąpić zaraz po sobie, jeżeli jedna
	 * 			   tak sie stanie oznacza to że coś jest bardzo nie tak)
	 * 		 'l' - oborty w lewo
	 * 		 'r' - obroty w prawo
	 */
};

void Quadrature() {
//---------------------------------INICJALIZACJA REJESTRÓW------------------------------------------------------
//INICJALIZACJA PCINT

	GIMSK |= (1 << PCIE);//włączam przerwania przy zmianie stanu na pinach PCINT1 i PCINT0
	PCMSK |= (1 << PCINT0) | (1 << PCINT1);	// określam które piny mają być ustaniowne jako PCINT

//--------------------------------------------------------------------------------------------------------------
	sei();				//zezwalam na wykonanie przerwań w programie

}

ISR(PCINT_vect)	//przerwania wywoływane zmianą stanu na którymś z pinów PCINT
{

	current_state = PINB & 0x03;
	/*
	 * odczytywanie wartości z pinów okreslonych w programie jako PCINT (PINB & 0b0000 0011)
	 * wartość jednego z kanałów enkodera kwadraturowego
	 */

	uint8_t index = (old_state << 2) | (current_state);
	/*
	 * określanie z której komórki tablicy bede czytac kierunek obrotów
	 * NP:
	 * old_state     = 0b0000 0011
	 * current_state = 0b0000 0010
	 * old_state << 2 = 0b0000 1100
	 * | opreacja OR (+)
	 * 0b0000 1100
	 * 0b0000 0010	+
	 * ----------------
	 * 0b0000 1110
	 * zczytuje z komórki 14 -> obroty w prawo
	 */

	if (q[index] == 'r')
		position_counter++;	//jeżeli kręci się w prawo inkrementuje wartość pozycji silnika
	else if (q[index] == 'l')
		position_counter--;	//jeżeli w lewno to dekrementuje wartość pozycji silnika
	//flaga = q[index];

	if (position_counter == N)
		position_counter = 0;
	/*
	 * jeżeli licznik pozycji osiągnie maksymalną liczbę impulsów na obrót silnika zeruje jego wartość
	 * dzięki temu osiągam dokładność określenia pozycji taką jaką oferuje enkoder kwadraturowy o N = 4480 imp/obr
	 */
	if (position_counter == -1)
		position_counter = N - 1;
	/*
	 * jeżzeli licznik pozycji osiągnie wartość -1 to zmieniamy jego wartość na liczbę o 1 mniejszą od
	 * maksymalnej liczby impuslów na obrót wału silnika (rozdzielczość enkodera-1 = 4480-1)
	 */

	old_state = current_state;
	/*
	 * przypisuję wartość aktualnie zczytaną z kanału enkodera kwadraturowego to zmiennej old_state
	 * dzięki temu przy następnej zmianię na którymś kanale będę wiedziała w którą stronę obraca się silnik
	 */

//	uartPut(position_counter >> 8);
//	uartPut(position_counter & 0xff);
}

