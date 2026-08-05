/*
 * RCC.h
 *
 *  Created on: Aug 3, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_RCC_H_
#define INC_RCC_H_


#include"main.h"

void Stm32f103_System_Clock_Init(void);
uint32_t Get_SYSCLK_freq(void);
uint32_t Get_AHB_freq(void) ;
uint32_t Get_APB1_freq(void);
uint32_t Get_APB2_freq(void) ;


#endif /* INC_RCC_H_ */
