/*
 * IWDT.c
 *
 *  Created on: Jul 15, 2026
 *      Author: MadaYaswanth
 */


#include"main.h"
#include"IWDT.h"
static const Wdt_Log_t *Watcch_Dog_Timer_log = (const Wdt_Log_t*)FLASH_LAST_PAGE_ADDR;

/**
 * @brief  Initialize Independent Watchdog (IWDG)
 * @param  timeout_ms : Required timeout in milliseconds.
 * @note
 *         Assumes LSI Clock = 40kHz.
 *         Prescaler is fixed to 64.
 *         Maximum reload value = 4095.
 *
 *         Counter Clock = 40000 / 64 = 625 Hz
 *         Reload = (timeout_ms × 625) / 1000
 *
 * @retval None
 */
void IWDG_Init(uint32_t timeout_ms)
{
    uint32_t reload;

    /* Enable write access to PR and RLR registers */
    IWDG->KR = 0x5555;

    /* Set Prescaler = 64
       PR = 4 corresponds to divide-by-64 */
    IWDG->PR = 4;

    /* Calculate reload value based on timeout */
    reload = (timeout_ms * 625UL) / 1000UL;

    /* Limit reload value to hardware range */
    if (reload > 4095)
        reload = 4095;

    /* Prevent invalid reload value */
    if (reload < 1)
        reload = 1;

    /* Load calculated reload value */
    IWDG->RLR = reload;

    /* Reload watchdog counter */
    IWDG->KR = 0xAAAA;

    /* Start Independent Watchdog */
    IWDG->KR = 0xCCCC;
}

/**
 * @brief  Refresh (Feed/Kick) Independent Watchdog.
 * @note
 *         Must be called before watchdog timeout.
 *         Otherwise MCU will reset automatically.
 *
 * @retval None
 */
void IWDG_Refresh(void)
{
    /* Reload watchdog counter */
    IWDG->KR = 0xAAAA;
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
