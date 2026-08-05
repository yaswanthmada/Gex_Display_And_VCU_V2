/*
 * PROCESS.c
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */


#include"PROCESS.h"
#include"PROCESS_MOTOR_CONTROLLER.h"
void Process_Can_Messages()
{
	CAN_Message_t rx_msg;
	if(MCP2515_EchoMessage(&rx_msg))
	{
      Process_Motor_Controller_Messages(&rx_msg);
      rx_msg.id=rx_msg.id+6;
      MCP2515_SendMessage(&rx_msg);
	}
}
