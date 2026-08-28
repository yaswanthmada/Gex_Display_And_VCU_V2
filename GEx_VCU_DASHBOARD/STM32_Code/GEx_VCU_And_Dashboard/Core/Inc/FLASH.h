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
#define FLASH_LAST_PAGE_ADDR  0x0801FC00UL //Last page of Flash
#define WDT_MAGIC_KEY         0xA5A5U

typedef struct
{
    uint16_t wdt_flag;    /* Magic Key (0xA5A5 = Watchdog Reset Logged) */
    uint16_t reset_count; /* Cumulative watchdog reset count             */
} Wdt_Log_t;

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
void Wdt_Log_Check_And_Process(void);

/*******************************************************************************
 * Function Name : WdtLog_Read
 * Description   : Reads the stored watchdog reset log structure directly
 *                 from Flash memory and validates the magic key.
 * Scope         : Global (Public API)
 * Parameters    : log_out - Pointer to WdtLog_t structure to store output data
 * Return Value  : true    - Valid watchdog log found (0xA5A5 key present)
 *                 false   - Log invalid, null pointer, or flash erased
 ******************************************************************************/
bool Wdt_Log_Read(Wdt_Log_t *log_out);

/*******************************************************************************
 * Function Name : WdtLog_ClearFlash
 * Description   : Unlocks and erases Page 127 of Flash memory, resetting the
 *                 stored watchdog log and cumulative reset counter.
 * Scope         : Global (Public API)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Wdt_Log_Clear_Flash(void);

#endif /* INC_FLASH_H_ */
