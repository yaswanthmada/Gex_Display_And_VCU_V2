/*
 * FLASH.c
 *
 *  Created on: Aug 24, 2026
 *      Author: MadaYaswanth
 */


#include"FLASH.h"
#include <stddef.h>
inline void Flash_Wait_Busy(void)
{
    while (FLASH->SR & FLASH_SR_BSY);
}
void Flash_Unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}
void Flash_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}
void Flash_Erase_Page(uint32_t page_addr)
{
	Flash_Wait_Busy();
    FLASH->CR |= FLASH_CR_PER;          /* Enable Page Erase     */
    FLASH->AR = page_addr;              /* Target page address   */
    FLASH->CR |= FLASH_CR_STRT;         /* Start Erase operation */
    Flash_Wait_Busy();
    FLASH->CR &= ~FLASH_CR_PER;         /* Disable Page Erase    */
}

void Flash_Write_Half_Word(uint32_t address, uint16_t data)
{
	Flash_Wait_Busy();
    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint16_t*)address = data;
    Flash_Wait_Busy();
    FLASH->CR &= ~FLASH_CR_PG;
}
