/*
 * SPI.c
 *
 *  Created on: Aug 3, 2026
 *      Author: MadaYaswanth
 */


#include"SPI.h"


/*******************************************************************************
 * Function Name : SPI2_Init
 * Description   : Configures SPI2 peripheral and GPIO pins PB12-PB15.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (1 = Success,0=Failure)
 ******************************************************************************/
static bool SPI2_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    GPIOB->CRH &= ~GPIO_CRH_MODE12;
    GPIOB->CRH &= ~GPIO_CRH_CNF12;
    GPIOB->CRH |= GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1;
    GPIOB->CRH &= ~GPIO_CRH_MODE13;
    GPIOB->CRH &= ~GPIO_CRH_CNF13;
    GPIOB->CRH |= GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1;
    GPIOB->CRH |= GPIO_CRH_CNF13_1;
    GPIOB->CRH &= ~GPIO_CRH_MODE14;
    GPIOB->CRH &= ~GPIO_CRH_CNF14;
    GPIOB->CRH |= GPIO_CRH_CNF14_0;
    GPIOB->CRH &= ~GPIO_CRH_MODE15;
    GPIOB->CRH &= ~GPIO_CRH_CNF15;
    GPIOB->CRH |= GPIO_CRH_MODE15_0 | GPIO_CRH_MODE15_1;
    GPIOB->CRH |= GPIO_CRH_CNF15_1;
    GPIOB->BSRR = GPIO_BSRR_BS12;
    SPI2->CR1 = 0;
    SPI2->CR1 |= SPI_CR1_MSTR;
    SPI2->CR1 |= SPI_CR1_BR_1;
    SPI2->CR1 |= SPI_CR1_SSM;
    SPI2->CR1 |= SPI_CR1_SSI;
    SPI2->CR1 |= SPI_CR1_SPE;
	if ((SPI2->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
	{
		return false;
	}

	return true;
}

/*******************************************************************************
 * Function Name : SPI2_DeInit
 * Description   : Safely de-initializes SPI2 by waiting for active transfers,
 *                 disabling the peripheral, pulsing a hardware block reset,
 *                 and gating the clock off.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void SPI2_DeInit(void)
{
    SPI2->CR1 &= ~SPI_CR1_SPE;
    RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
}
void SPI2_Chip_Select(uint8_t enable)
{
    if (enable)
    {
        GPIOB->BRR = GPIO_BRR_BR12;
    }
    else
    {
        GPIOB->BSRR = GPIO_BSRR_BS12;
    }
}
uint8_t SPI2_TransmitReceiveByte(uint8_t txByte)
{
    uint32_t timeout = 100000;
    while (!(SPI2->SR & SPI_SR_TXE))
    {
        if (--timeout == 0) return 0xFF;
    }
    SPI2->DR = txByte;
    timeout = 100000;
    while (!(SPI2->SR & SPI_SR_RXNE))
    {
        if (--timeout == 0) return 0xFF;
    }
    return (uint8_t)SPI2->DR;
}

uint8_t SPI2_Tx_Buffer(const uint8_t *pBuffer, uint16_t length)
{
    if (pBuffer == NULL || length == 0) return 0;

    for (uint16_t i = 0; i < length; i++)
    {
        SPI2_TransmitReceiveByte(pBuffer[i]);
    }
    return 1;
}

uint8_t SPI2_RxBuffer(uint8_t *pBuffer, uint16_t length)
{
    if (pBuffer == NULL || length == 0) return 0;

    for (uint16_t i = 0; i < length; i++)
    {
        pBuffer[i] = SPI2_TransmitReceiveByte(0xFF); /* Send dummy byte to clock out data */
    }
    return 1;
}
bool Serial_Peripheral_Interface_Init(void)
{

	return (bool)SPI2_Init();
}
