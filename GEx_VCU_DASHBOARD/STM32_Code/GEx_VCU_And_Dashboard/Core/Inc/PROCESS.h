/*
 * PROCESS.h
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_
#include"MCP2515.h"
#include"SYSTICK.h"
#include"CAN.h"
void Process_Can_Messages();
void Request_Bms_Messages();
bool Get_MCU_Can_Fault();
bool Get_BMS_Can_Fault();
#endif /* INC_PROCESS_H_ */
