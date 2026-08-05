/*
 * SPI.h
 *
 *  Created on: Aug 3, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include"main.h"


bool  Serial_Peripheral_Interface_Init();
void    SPI2_DeInit(void);
void    SPI2_Chip_Select(uint8_t enable);

uint8_t SPI2_TransmitReceiveByte(uint8_t txByte);
uint8_t SPI2_TxBuffer(const uint8_t *pBuffer, uint16_t length);
uint8_t SPI2_RxBuffer(uint8_t *pBuffer, uint16_t length);

#endif /* INC_SPI_H_ */
