/*
 * PROCESS_MOTOR_CONTROLLER.h
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_PROCESS_MOTOR_CONTROLLER_H_
#define INC_PROCESS_MOTOR_CONTROLLER_H_


#include"CONFIGURATION_FILES.h"
#include"DECODE_MOTOR_CONTROLLER.h"
#include"MCP2515.h"
#include"SYSTICK.h"

void Process_Motor_Controller_Messages(CAN_Message_t *rxMsg);

#endif /* INC_PROCESS_MOTOR_CONTROLLER_H_ */
