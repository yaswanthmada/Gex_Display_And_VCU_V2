/*
 * MCP2515.h
 *
 *  Created on: Aug 3, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_MCP2515_H_
#define INC_MCP2515_H_

#include"main.h"

#include "MCP2515_DEF.h"

/* ========================================================================= */
/*                         A - Z FUNCTION DECLARATIONS                      */
/* ========================================================================= */
bool    MCP2515_AbortTransmission(MCP2515_TxBuffer_t txBufferMask);

void    MCP2515_BitModify(uint8_t regAddr, uint8_t mask, uint8_t value);

void    MCP2515_ClearInterruptFlags(uint8_t flagsMask);
void    MCP2515_ClearRxBuffer(uint8_t bufferNum);

void    MCP2515_DeInit(void);

void    MCP2515_EnableInterrupts(uint8_t interruptMask);


void    MCP2515_FlushTxBuffer(MCP2515_TxBuffer_t txBufferNum);

uint8_t MCP2515_GetErrorFlags(void);
MCP2515_Mode_t MCP2515_GetMode(void);
uint8_t MCP2515_GetRxErrorCount(void);
uint8_t MCP2515_GetTxErrorCount(void);
bool MCP2515_EchoMessage(CAN_Message_t *rxMsg);

bool    Micro_Chip_Product_2515_Init();
bool    MCP2515_IsBufferAvailable(void);
bool    MCP2515_IsMessageAvailable(void);

bool    MCP2515_LoadTxBuffer(MCP2515_TxBuffer_t bufferIdx, const CAN_Message_t *msg);

void    MCP2515_ModifyBit(uint8_t regAddr, uint8_t mask, uint8_t value);

bool    MCP2515_NormalMode(void);

void    MCP2515_OnInterruptServiceRoutine(void);

uint8_t MCP2515_PollStatus(void);

uint8_t MCP2515_ReadRegister(uint8_t regAddr);
void    MCP2515_ReadRegisters(uint8_t startAddr, uint8_t *pData, uint8_t len);
bool    MCP2515_ReadRxBuffer(uint8_t rxBufferCommand, CAN_Message_t *msg);
uint8_t MCP2515_ReadStatus(void);
bool    MCP2515_ReceiveMessage(CAN_Message_t *msg);
void    MCP2515_RequestToSend(MCP2515_TxBuffer_t txBufferMask);

bool    MCP2515_SendMessage(const CAN_Message_t *msg);
bool    MCP2515_SetFilter(MCP2515_Filter_t filterNum, bool isExtended, uint32_t filterValue);
bool    MCP2515_SetMask(MCP2515_Mask_t maskNum, bool isExtended, uint32_t maskValue);
bool    MCP2515_Set_Mode(MCP2515_Mode_t mode);
void    MCP2515_SetOneShotMode(bool enable);
void    MCP2515_SetPriority(MCP2515_TxBuffer_t txBufferNum, uint8_t priority);
bool    MCP2515_Sleep(void);
bool    MCP2515_WakeUp(void);
void    MCP2515_Write_Register(uint8_t regAddr, uint8_t value);
void    MCP2515_WriteRegisters(uint8_t startAddr, const uint8_t *pData, uint8_t len);
#endif /* INC_MCP2515_H_ */
