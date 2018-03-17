/*
 * PWM.h
 *
 *  Created on: 30.11.2017
 *      Author: dorota
 */

#ifndef PWM_H_
#define PWM_H_

void PWM_Init(void);
void Set_Speed(uint16_t Impulse_Left);
uint16_t absolute_movement(int16_t wanted);

#endif /* PWM_H_ */
