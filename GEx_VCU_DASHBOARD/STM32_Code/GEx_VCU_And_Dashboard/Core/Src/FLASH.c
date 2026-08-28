/*
 * FLASH.c
 *
 *  Created on: Aug 24, 2026
 *      Author: MadaYaswanth
 */


#include"FLASH.h"
#include <stddef.h>
static const Wdt_Log_t *Watcch_Dog_Timer_log = (const Wdt_Log_t*)FLASH_LAST_PAGE_ADDR;
/*******************************************************************************
 * Function Name : Flash_WaitBusy
 * Description   : Blocks CPU execution until the current Flash memory operation
 *                 (Erase or Program) completes by polling the BSY flag.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static inline void Flash_Wait_Busy(void)
{
    while (FLASH->SR & FLASH_SR_BSY);
}
/*******************************************************************************
 * Function Name : Flash_Unlock
 * Description   : Unlocks the Flash Control Register (FLASH_CR) by writing the
 *                 two-step magic sequence to FLASH_KEYR if currently locked.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Flash_Unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

/*******************************************************************************
 * Function Name : Flash_Lock
 * Description   : Sets the LOCK bit in the Flash Control Register (FLASH_CR)
 *                 to prevent accidental memory modification.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Flash_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/*******************************************************************************
 * Function Name : Flash_ErasePage
 * Description   : Performs a 1 KB page erase on the specified Flash page address.
 * Scope         : Static (Private to this file)
 * Parameters    : page_addr - Starting base address of the Flash page to erase
 * Return Value  : None
 ******************************************************************************/
static void Flash_Erase_Page(uint32_t page_addr)
{
	Flash_Wait_Busy();
    FLASH->CR |= FLASH_CR_PER;          /* Enable Page Erase     */
    FLASH->AR = page_addr;              /* Target page address   */
    FLASH->CR |= FLASH_CR_STRT;         /* Start Erase operation */
    Flash_Wait_Busy();
    FLASH->CR &= ~FLASH_CR_PER;         /* Disable Page Erase    */
}

/*******************************************************************************
 * Function Name : Flash_WriteHalfWord
 * Description   : Writes a 16-bit half-word to a specified Flash memory address.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Destination half-word aligned Flash address
 *                 data    - 16-bit data value to program
 * Return Value  : None
 ******************************************************************************/
static void Flash_Write_Half_Word(uint32_t address, uint16_t data)
{
	Flash_Wait_Busy();
    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint16_t*)address = data;
    Flash_Wait_Busy();
    FLASH->CR &= ~FLASH_CR_PG;
}

/*******************************************************************************
 * Function Name : WdtLog_CheckAndProcess
 * Description   : Checks RCC status registers for Independent (IWDG) or
 *                 Window (WWDG) watchdog reset flags upon startup. If a
 *                 watchdog reset is detected, it logs/increments the event
 *                 in Flash Page 127 (0x0801FC00) and clears the RCC flags.
 * Scope         : Global (Public API)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Wdt_Log_Check_And_Process(void)
{
	    if (RCC->CSR & RCC_CSR_IWDGRSTF)
	    {
	    	uint16_t new_count = 1U;
			if (Watcch_Dog_Timer_log->wdt_flag == WDT_MAGIC_KEY)
			{
				new_count = Watcch_Dog_Timer_log->reset_count + 1U;
			}

			Flash_Unlock();
			Flash_Erase_Page(FLASH_LAST_PAGE_ADDR);
			Flash_Write_Half_Word(FLASH_LAST_PAGE_ADDR, WDT_MAGIC_KEY);
			Flash_Write_Half_Word(FLASH_LAST_PAGE_ADDR + 2U, new_count);
			Flash_Lock();
			RCC->CSR |= RCC_CSR_RMVF;
	    }
}

/*******************************************************************************
 * Function Name : WdtLog_Read
 * Description   : Reads the stored watchdog reset log structure directly
 *                 from Flash memory and validates the magic key.
 * Scope         : Global (Public API)
 * Parameters    : log_out - Pointer to WdtLog_t structure to store output data
 * Return Value  : true    - Valid watchdog log found (0xA5A5 key present)
 *                 false   - Log invalid, null pointer, or flash erased
 ******************************************************************************/
bool Wdt_Log_Read(Wdt_Log_t *log_out)
{
	Wdt_Log_t *flash_ptr = (Wdt_Log_t*)FLASH_LAST_PAGE_ADDR;

    if ((log_out != NULL) && (flash_ptr->wdt_flag == WDT_MAGIC_KEY))
    {
        log_out->wdt_flag = flash_ptr->wdt_flag;
        log_out->reset_count = flash_ptr->reset_count;
        return true;
    }
    return false;
}

/*******************************************************************************
 * Function Name : WdtLog_ClearFlash
 * Description   : Unlocks and erases Page 127 of Flash memory, resetting the
 *                 stored watchdog log and cumulative reset counter.
 * Scope         : Global (Public API)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void WdtLog_ClearFlash(void)
{
    Flash_Unlock();
    Flash_Erase_Page(FLASH_LAST_PAGE_ADDR);
    Flash_Lock();
}
void Get_Hard_Fault_Watch_Dog_Timer_Data(GEx_Display_t* Wdt_Hf_System_Data)
{
	if (Wdt_Hf_System_Data != NULL)
	{
		if (Watcch_Dog_Timer_log->wdt_flag == WDT_MAGIC_KEY)
		{
			Wdt_Hf_System_Data->System_Data.Is_Watch_Dog_Reset = 1U;
			Wdt_Hf_System_Data->System_Data.Wdt_Count = Watcch_Dog_Timer_log->reset_count;
		}
		else
		{
			Wdt_Hf_System_Data->System_Data.Is_Watch_Dog_Reset = 0U;
			Wdt_Hf_System_Data->System_Data.Wdt_Count = 0U;
		}
	}
}
