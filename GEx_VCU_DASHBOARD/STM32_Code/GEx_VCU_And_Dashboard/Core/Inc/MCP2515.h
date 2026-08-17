/*
 * MCP2515.h
 *
 *  Created on: Aug 3, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_MCP2515_H_
#define INC_MCP2515_H_

#include"main.h"
#include"CONFIGURATION_FILES.h"
#include "MCP2515_DEF.h"
/*******************************************************************************
 * Function Name : Micro_Chip_Product_2515_Init
 * Description   : Wrapper function that initializes the MCP2515 CAN controller
 *                 with default application settings (250 Kbps, 8 MHz clock).
 * Scope         : Global
 * Parameters    : None
 * Return Value  : bool (true = Initialization successful, false = Failed)
 ******************************************************************************/
bool Micro_Chip_Product_2515_Init(void);
/*******************************************************************************
 * Function Name : MCP2515_Can_Collect_Frame
 * Description   : Checks if a CAN message is available in the receive buffers,
 *                 and if so, retrieves and stores it into the provided frame structure.
 * Scope         : Public / Driver API
 * Parameters    : Frame - Pointer to the CAN_Message_t structure to store the collected frame
 * Return Value  : bool (true = Frame successfully collected, false = No message available)
 ******************************************************************************/
bool MCP2515_Can_Collect_Frame(CAN_Message_t *Frame);
/*******************************************************************************
 * Function Name : Configure_Mcp2515_Can_Ids
 * Description   : Configures the MCP2515 hardware filters using a predefined list
 *                 of standard CAN IDs based on the active MCU target (e.g., Nanjing Quark),
 *                 leaving extended ID parameters empty.
 * Scope         : Global / Application Level
 * Parameters    : None
 * Return Value  : bool (true if filters were applied successfully, false otherwise)
 ******************************************************************************/
bool Configure_Mcp2515_Can_Ids();


#endif /* INC_MCP2515_H_ */
