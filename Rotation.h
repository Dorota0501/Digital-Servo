/*
 * Rotation.h
 *
 *  Created on: 11.11.2017
 *      Author: dorota
 */

#ifndef ROTATION_H_
#define ROTATION_H_

#define ROTATE_RIGHT 1
#define ROTATE_LEFT 2
#define ROTATE_STOP 0
#define N 4480				// ilość impulsów zczytywanych z enkodera na obrót silnika

void Engine_rotation(uint8_t command);		//ustawia mostek H wg command np: ROTATE_LEFT)
void rotateTo(uint16_t wartoscZadana);		//przelicza kąt obrotu na impulsy i wywołuje obrot
											//w momencie osiagniecia zadanej pozycji ztrzymuje silnik


#endif /* ROTATION_H_ */

