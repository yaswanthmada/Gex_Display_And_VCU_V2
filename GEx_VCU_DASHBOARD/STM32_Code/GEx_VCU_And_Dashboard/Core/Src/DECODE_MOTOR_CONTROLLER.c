/*
 * DECODE_MOTOR_CONTROLLER.c
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */


#include"CONFIGURATION_FILES.h"
#include"DECODE_MOTOR_CONTROLLER.h"
#include"PROCESS_MOTOR_CONTROLLER.h"
Mcu_Msgs_t Mcu_Msgs={0};
void Process_Motor_Controller_Messages(CAN_Message_t *Frames)
{
	switch(Frames->id)
	{
#ifdef MCU_NANJING_QUARK
	case MCU_MSG_0x011_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x011(Frames->data,&Mcu_Msgs.MCU_MSG_0x011);
		break;
	case MCU_MSG_0x012_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x012(Frames->data,&Mcu_Msgs.MCU_MSG_0x012);
		break;
	case MCU_MSG_0x013_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x013(Frames->data,&Mcu_Msgs.MCU_MSG_0x013);
		break;
	case MCU_MSG_0x014_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x014(Frames->data,&Mcu_Msgs.MCU_MSG_0x014);
		break;
#endif
	}
}
