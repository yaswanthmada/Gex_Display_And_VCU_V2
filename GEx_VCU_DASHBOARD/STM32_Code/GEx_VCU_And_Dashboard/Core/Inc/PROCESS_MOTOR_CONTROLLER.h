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
typedef struct
{
#ifdef MCU_NANJING_QUARK
	MCU_Msg_0x011_t MCU_MSG_0x011;
	MCU_Msg_0x012_t MCU_MSG_0x012;
	MCU_Msg_0x013_t MCU_MSG_0x013;
	MCU_Msg_0x014_t MCU_MSG_0x014;
#endif
	    bool mcu_comm_ok;
}Mcu_Msgs_t;


void Process_Motor_Controller_Messages(CAN_Message_t *rxMsg);

#endif /* INC_PROCESS_MOTOR_CONTROLLER_H_ */
