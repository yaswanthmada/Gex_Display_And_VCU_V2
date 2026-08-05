/*
 * PRINT_MOTOR_CONTROLLER.h
 *
 *  Created on: Aug 5, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_PRINT_MOTOR_CONTROLLER_H_
#define INC_PRINT_MOTOR_CONTROLLER_H_

#include"DECODE_MOTOR_CONTROLLER.h"

void Print_MCU_0x011(const MCU_Msg_0x011_t* Message);
void Print_MCU_0x012(const MCU_Msg_0x012_t* Message);
void Print_MCU_0x013(const MCU_Msg_0x013_t* Message);
void Print_MCU_0x014(const MCU_Msg_0x014_t* Message);
void Print_Motor_Messages(void);
#endif /* INC_PRINT_MOTOR_CONTROLLER_H_ */
