/*
 * FLASH.h
 *
 *  Created on: Aug 24, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include"main.h"
#include <stdint.h>
#include <stdbool.h>
#include"DWIN_DISPLAY.h"

/*******************************************************************************
 * Function Name : Flash_WriteHalfWord
 * Description   : Writes a 16-bit half-word to a specified Flash memory address.
 * Scope         : Global (Public API)
 * Parameters    : address - Destination half-word aligned Flash address
 *                 data    - 16-bit data value to program
 * Return Value  : None
 ******************************************************************************/
void Flash_Write_Half_Word(uint32_t address, uint16_t data);

/*******************************************************************************
 * Function Name : Flash_ErasePage
 * Description   : Performs a 1 KB page erase on the specified Flash page address.
 * Scope         : Global (Public API)
 * Parameters    : page_addr - Starting base address of the Flash page to erase
 * Return Value  : None
 ******************************************************************************/
void Flash_Erase_Page(uint32_t page_addr);

/*******************************************************************************
 * Function Name : Flash_Lock
 * Description   : Sets the LOCK bit in the Flash Control Register (FLASH_CR)
 *                 to prevent accidental memory modification.
 * Scope         : Global (Public API)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Flash_Lock(void);
/*******************************************************************************
 * Function Name : Flash_Unlock
 * Description   : Unlocks the Flash Control Register (FLASH_CR) by writing the
 *                 two-step magic sequence to FLASH_KEYR if currently locked.
 * Scope         : Global (Public API)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Flash_Unlock(void);
/*******************************************************************************
 * Function Name : Flash_WaitBusy
 * Description   : Blocks CPU execution until the current Flash memory operation
 *                 (Erase or Program) completes by polling the BSY flag.
 * Scope         : Global (Public API)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Flash_Wait_Busy(void);
#endif /* INC_FLASH_H_ */
