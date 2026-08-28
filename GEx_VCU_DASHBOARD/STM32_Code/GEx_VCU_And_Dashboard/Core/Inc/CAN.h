/*
 * CAN.h
 *
 *  Created on: Aug 6, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include"MCP2515.h"
#include"main.h"
#include"RCC.h"
#include"CONFIGURATION_FILES.h"
#include"DECODE_BATTERY_MANAGEMENT_SYSTEM.h"
#include"DWIN_DISPLAY.h"
#define CAN1_500_BAUD_RATE 500U //in Kb
#define CAN1_250_BAUD_RATE 250U //in Kb

/*******************************************************************************
 * Function Name : Controller_Area_Network_Init
 * Description   : Public wrapper function to initialize the CAN1 peripheral,
 *                 GPIO remapping, bit timing, and hardware acceptance filters.
 * Scope         : Global / Public
 * Parameters    : None
 * Return Value  : bool (true = CAN bus hardware initialized successfully, false = Failed)
 ******************************************************************************/
bool Controller_Area_Network_Init();

/*******************************************************************************
 * Function Name : Can_Collect_Frame
 * Description   : Checks if any received CAN message is pending in hardware FIFOs
 *                 and reads the message into the provided buffer if available.
 * Scope         : Global / Public
 * Parameters    : Frame - Pointer to buffer structure to store received frame (CAN_Message_t*)
 * Return Value  : bool (true = Frame successfully read, false = No pending messages)
 ******************************************************************************/

bool Can_Collect_Frame(CAN_Message_t *Frame);
/*******************************************************************************
 * Function Name : Can_Send_Frame
 * Description   : Checks for an available hardware transmit mailbox and sends
 *                 the provided CAN frame if a mailbox is free.
 * Scope         : Global / Public
 * Parameters    : Frame - Pointer to CAN frame structure to transmit (CAN_Message_t*)
 * Return Value  : bool (true = Frame loaded into mailbox, false = All mailboxes busy)
 ******************************************************************************/
bool Can_Send_Frame(CAN_Message_t *Frame);

void Configure_Can_filter_Ids();
#endif /* INC_CAN_H_ */
