/*
 * MCP2515.c
 *
 *  Created on: Aug 3, 2026
 *      Author: MadaYaswanth
 */

#include"MCP2515.h"
#include"MCP2515_REG.h"
#include"SPI.h"
/*******************************************************************************
 * Function Name : MCP2515_Get_Mode
 * Description   : Reads the CANSTAT status register and extracts the current
 *                 operating mode of the MCP2515 CAN controller.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : MCP2515_Mode_t (Current mode: CONFIG, NORMAL, SLEEP, etc.)
 ******************************************************************************/
MCP2515_Mode_t MCP2515_Get_Mode(void)
{
    return (MCP2515_Mode_t)(MCP2515_ReadRegister(MCP_REG_CANSTAT) & MCP_MASK_MODE);
}
bool MCP2515_AbortTransmission(MCP2515_TxBuffer_t txBufferMask)
{
    if (txBufferMask & MCP_TXB0) MCP2515_BitModify(MCP_REG_TXB0CTRL, MCP_BIT_TXREQ, 0x00);
    if (txBufferMask & MCP_TXB1) MCP2515_BitModify(MCP_REG_TXB1CTRL, MCP_BIT_TXREQ, 0x00);
    if (txBufferMask & MCP_TXB2) MCP2515_BitModify(MCP_REG_TXB2CTRL, MCP_BIT_TXREQ, 0x00);
    return true;
}

/* B */
void MCP2515_BitModify(uint8_t regAddr, uint8_t mask, uint8_t value)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_BITMOD);
    SPI2_Transmit_Receive_Byte(regAddr);
    SPI2_Transmit_Receive_Byte(mask);
    SPI2_Transmit_Receive_Byte(value);
    SPI2_Chip_Select(0);
}

/* C */
void MCP2515_ClearInterruptFlags(uint8_t flagsMask)
{
    MCP2515_BitModify(MCP_REG_CANINTF, flagsMask, 0x00);
}

void MCP2515_ClearRxBuffer(uint8_t bufferNum)
{
    if (bufferNum == 0)      MCP2515_BitModify(MCP_REG_CANINTF, MCP_BIT_RX0IF, 0x00);
    else if (bufferNum == 1) MCP2515_BitModify(MCP_REG_CANINTF, MCP_BIT_RX1IF, 0x00);
}

/* D */
void MCP2515_DeInit(void)
{
    MCP2515_Sleep();
    SPI2_DeInit();
}

/* E */
void MCP2515_EnableInterrupts(uint8_t interruptMask)
{
    MCP2515_Write_Register(MCP_REG_CANINTE, interruptMask);
}

/* F */
void MCP2515_FlushTxBuffer(MCP2515_TxBuffer_t txBufferNum)
{
    uint8_t addr = (txBufferNum == MCP_TXB0) ? MCP_REG_TXB0SIDH :
                   (txBufferNum == MCP_TXB1) ? MCP_REG_TXB1SIDH : MCP_REG_TXB2SIDH;
    for (uint8_t i = 0; i < 13; i++)
    {
        MCP2515_Write_Register(addr + i, 0x00);
    }
}

/* G */
uint8_t MCP2515_GetErrorFlags(void)
{
    return MCP2515_ReadRegister(MCP_REG_EFLG);
}

uint8_t MCP2515_GetRxErrorCount(void)
{
    return MCP2515_ReadRegister(MCP_REG_REC);
}

uint8_t MCP2515_GetTxErrorCount(void)
{
    return MCP2515_ReadRegister(MCP_REG_TEC);
}

bool MCP2515_IsBufferAvailable(void)
{
    uint8_t ctrl0 = MCP2515_ReadRegister(MCP_REG_TXB0CTRL);
    uint8_t ctrl1 = MCP2515_ReadRegister(MCP_REG_TXB1CTRL);
    uint8_t ctrl2 = MCP2515_ReadRegister(MCP_REG_TXB2CTRL);

    return (!(ctrl0 & MCP_BIT_TXREQ) || !(ctrl1 & MCP_BIT_TXREQ) || !(ctrl2 & MCP_BIT_TXREQ));
}

bool MCP2515_IsMessageAvailable(void)
{
    uint8_t flags = MCP2515_ReadRegister(MCP_REG_CANINTF);
    return (flags & MCP_MASK_RXIF) != 0;
}

bool MCP2515_LoadTxBuffer(MCP2515_TxBuffer_t bufferIdx, const CAN_Message_t *msg)
{
    if (msg == NULL || msg->dlc > 8) return false;

    uint8_t baseAddr = (bufferIdx == MCP_TXB0) ? MCP_REG_TXB0SIDH :
                       (bufferIdx == MCP_TXB1) ? MCP_REG_TXB1SIDH : MCP_REG_TXB2SIDH;

    if (msg->isExtended)
    {
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDH, (uint8_t)(msg->id >> 21));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDL, (uint8_t)(((msg->id >> 13) & 0xE0) | MCP_BIT_EXIDE | ((msg->id >> 16) & 0x03)));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID8, (uint8_t)(msg->id >> 8));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID0, (uint8_t)(msg->id));
    }
    else
    {
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDH, (uint8_t)(msg->id >> 3));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDL, (uint8_t)((msg->id & 0x07) << 5));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID8, 0x00);
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID0, 0x00);
    }

    uint8_t dlcReg = (msg->dlc & MCP_MASK_DLC) | (msg->isRTR ? MCP_BIT_RTR : 0x00);
    MCP2515_Write_Register(baseAddr + MCP_OFFSET_DLC, dlcReg);

    for (uint8_t i = 0; i < msg->dlc; i++)
    {
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_DATA + i, msg->data[i]);
    }

    return true;
}

void MCP2515_ModifyBit(uint8_t regAddr, uint8_t mask, uint8_t value)
{
    MCP2515_BitModify(regAddr, mask, value);
}

bool MCP2515_NormalMode(void)
{
    return MCP2515_Set_Mode(MCP_MODE_NORMAL);
}

void MCP2515_OnInterruptServiceRoutine(void)
{
    uint8_t flags = MCP2515_ReadRegister(MCP_REG_CANINTF);

    if (flags & MCP_BIT_RX0IF) { /* Handle RX0 Interrupt */ }
    if (flags & MCP_BIT_RX1IF) { /* Handle RX1 Interrupt */ }
    if (flags & MCP_BIT_TX0IF) { /* Handle TX0 Interrupt */ }
    if (flags & MCP_BIT_ERRIF) { /* Handle ERRIF Interrupt */ }

    MCP2515_ClearInterruptFlags(flags);
}

uint8_t MCP2515_PollStatus(void)
{
    return MCP2515_ReadStatus();
}

uint8_t MCP2515_ReadRegister(uint8_t regAddr)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_READ);
    SPI2_Transmit_Receive_Byte(regAddr);
    uint8_t val = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    SPI2_Chip_Select(0);
    return val;
}

void MCP2515_ReadRegisters(uint8_t startAddr, uint8_t *pData, uint8_t len)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_READ);
    SPI2_Transmit_Receive_Byte(startAddr);
    for (uint8_t i = 0; i < len; i++)
    {
        pData[i] = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    }
    SPI2_Chip_Select(0);
}

bool MCP2515_ReadRxBuffer(uint8_t rxBufferCommand, CAN_Message_t *msg)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(rxBufferCommand);

    uint8_t sidh = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t sidl = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t eid8 = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t eid0 = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t dlc  = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);

    msg->isExtended = (sidl & MCP_BIT_EXIDE) != 0;
    if (msg->isExtended)
    {
        msg->id = ((uint32_t)sidh << 21) | (((uint32_t)sidl & 0xE0) << 13) |
                  (((uint32_t)sidl & 0x03) << 16) | ((uint32_t)eid8 << 8) | eid0;
    }
    else
    {
        msg->id = ((uint32_t)sidh << 3) | (sidl >> 5);
    }

    msg->dlc = dlc & MCP_MASK_DLC;
    msg->isRTR = (dlc & MCP_BIT_RTR) != 0;

    for (uint8_t i = 0; i < msg->dlc; i++)
    {
        msg->data[i] = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    }

    SPI2_Chip_Select(0);
    return true;
}

uint8_t MCP2515_ReadStatus(void)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_READ_STATUS);
    uint8_t status = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    SPI2_Chip_Select(0);
    return status;
}

bool MCP2515_ReceiveMessage(CAN_Message_t *msg)
{
    uint8_t flags = MCP2515_ReadRegister(MCP_REG_CANINTF);

    if (flags & MCP_BIT_RX0IF)
    {
        MCP2515_ReadRxBuffer(MCP_CMD_READ_RX_BUF0, msg);
        MCP2515_ClearRxBuffer(0);
        return true;
    }
    else if (flags & MCP_BIT_RX1IF)
    {
        MCP2515_ReadRxBuffer(MCP_CMD_READ_RX_BUF1, msg);
        MCP2515_ClearRxBuffer(1);
        return true;
    }

    return false;
}

void MCP2515_RequestToSend(MCP2515_TxBuffer_t txBufferMask)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_RTS | (txBufferMask & MCP_TXB_ALL));
    SPI2_Chip_Select(0);
}


/* S */
bool MCP2515_SendMessage(const CAN_Message_t *msg)
{
    MCP2515_TxBuffer_t targetBuffer;
    uint8_t ctrl0 = MCP2515_ReadRegister(MCP_REG_TXB0CTRL);

    if (!(ctrl0 & MCP_BIT_TXREQ)) targetBuffer = MCP_TXB0;
    else
    {
        uint8_t ctrl1 = MCP2515_ReadRegister(MCP_REG_TXB1CTRL);
        if (!(ctrl1 & MCP_BIT_TXREQ)) targetBuffer = MCP_TXB1;
        else
        {
            uint8_t ctrl2 = MCP2515_ReadRegister(MCP_REG_TXB2CTRL);
            if (!(ctrl2 & MCP_BIT_TXREQ)) targetBuffer = MCP_TXB2;
            else return false;
        }
    }

    if (!MCP2515_LoadTxBuffer(targetBuffer, msg)) return false;
    MCP2515_RequestToSend(targetBuffer);
    return true;
}
/*******************************************************************************
 * Function Name : MCP2515_Set_Bitrate
 * Description   : Configures the CAN bus bit timing registers (CNF1, CNF2, CNF3)
 *                 for the specified baud rate based on the external oscillator
 *                 frequency.
 * Scope         : Static (Private to this file)
 * Parameters    : baudRate - Target CAN communication baud rate (MCP2515_BaudRate_t)
 *                 oscFreq  - Oscillator crystal frequency on module (MCP2515_Clock_t)
 * Return Value  : bool (true = Bitrate configured successfully, false = Invalid parameters)
 ******************************************************************************/
static bool MCP2515_Set_Bitrate(MCP2515_BaudRate_t baudRate, MCP2515_Clock_t oscFreq)
{
    if (oscFreq == MCP_CLOCK_8MHZ)
    {
        switch (baudRate)
        {
            case CAN_SPEED_125KBPS:
                MCP2515_Write_Register(MCP_REG_CNF1, 0x01);
                MCP2515_Write_Register(MCP_REG_CNF2, 0xB1);
                MCP2515_Write_Register(MCP_REG_CNF3, 0x85);
                return true;
            case CAN_SPEED_250KBPS:
                MCP2515_Write_Register(MCP_REG_CNF1, 0x00);
                MCP2515_Write_Register(MCP_REG_CNF2, 0xB1);
                MCP2515_Write_Register(MCP_REG_CNF3, 0x85);
                return true;
            case CAN_SPEED_500KBPS:
                MCP2515_Write_Register(MCP_REG_CNF1, 0x00);
                MCP2515_Write_Register(MCP_REG_CNF2, 0x90);
                MCP2515_Write_Register(MCP_REG_CNF3, 0x82);
                return true;
            default: return false;
        }
    }
    return false;
}

bool MCP2515_SetFilter(MCP2515_Filter_t filterNum, bool isExtended, uint32_t filterValue)
{
    MCP2515_Mode_t savedMode = MCP2515_Get_Mode();
    MCP2515_Set_Mode(MCP_MODE_CONFIG);

    uint8_t baseAddr;
    switch (filterNum)
    {
        case MCP_FILTER_RXF0: baseAddr = MCP_REG_RXF0SIDH; break;
        case MCP_FILTER_RXF1: baseAddr = MCP_REG_RXF1SIDH; break;
        case MCP_FILTER_RXF2: baseAddr = MCP_REG_RXF2SIDH; break;
        case MCP_FILTER_RXF3: baseAddr = MCP_REG_RXF3SIDH; break;
        case MCP_FILTER_RXF4: baseAddr = MCP_REG_RXF4SIDH; break;
        case MCP_FILTER_RXF5: baseAddr = MCP_REG_RXF5SIDH; break;
        default: return false;
    }

    if (isExtended)
    {
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDH, (uint8_t)(filterValue >> 21));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDL, (uint8_t)(((filterValue >> 13) & 0xE0) | MCP_BIT_EXIDE | ((filterValue >> 16) & 0x03)));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID8, (uint8_t)(filterValue >> 8));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID0, (uint8_t)(filterValue));
    }
    else
    {
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDH, (uint8_t)(filterValue >> 3));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDL, (uint8_t)((filterValue & 0x07) << 5));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID8, 0x00);
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID0, 0x00);
    }

    MCP2515_Set_Mode(savedMode);
    return true;
}

bool MCP2515_SetMask(MCP2515_Mask_t maskNum, bool isExtended, uint32_t maskValue)
{
    MCP2515_Mode_t savedMode = MCP2515_Get_Mode();
    MCP2515_Set_Mode(MCP_MODE_CONFIG);

    uint8_t baseAddr = (maskNum == MCP_MASK_RXM0) ? MCP_REG_RXM0SIDH : MCP_REG_RXM1SIDH;

    if (isExtended)
    {
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDH, (uint8_t)(maskValue >> 21));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDL, (uint8_t)(((maskValue >> 13) & 0xE0) | MCP_BIT_EXIDE | ((maskValue >> 16) & 0x03)));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID8, (uint8_t)(maskValue >> 8));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID0, (uint8_t)(maskValue));
    }
    else
    {
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDH, (uint8_t)(maskValue >> 3));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_SIDL, (uint8_t)((maskValue & 0x07) << 5));
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID8, 0x00);
        MCP2515_Write_Register(baseAddr + MCP_OFFSET_EID0, 0x00);
    }

    MCP2515_Set_Mode(savedMode);
    return true;
}
/*******************************************************************************
 * Function Name : MCP2515_Set_Mode
 * Description   : Requests a change to the MCP2515 operating mode via bit
 *                 modification of CANCTRL register and polls CANSTAT until the
 *                 mode switch completes or times out.
 * Scope         : Global / Public
 * Parameters    : mode - Target operating mode to set (MCP2515_Mode_t)
 * Return Value  : bool (true = Mode successfully updated, false = Operation timed out)
 ******************************************************************************/
bool MCP2515_Set_Mode(MCP2515_Mode_t mode)
{
    MCP2515_BitModify(MCP_REG_CANCTRL, MCP_MASK_MODE, mode);
    for (uint32_t timeout = 0; timeout < 10000; timeout++)
    {
        if (MCP2515_Get_Mode() == mode) return true;
    }
    return false;
}

void MCP2515_SetOneShotMode(bool enable)
{
    MCP2515_BitModify(MCP_REG_CANCTRL, MCP_BIT_OSM, enable ? MCP_BIT_OSM : 0x00);
}

void MCP2515_SetPriority(MCP2515_TxBuffer_t txBufferNum, uint8_t priority)
{
    uint8_t addr = (txBufferNum == MCP_TXB0) ? MCP_REG_TXB0CTRL :
                   (txBufferNum == MCP_TXB1) ? MCP_REG_TXB1CTRL : MCP_REG_TXB2CTRL;
    MCP2515_BitModify(addr, MCP_MASK_TXPRI, priority & MCP_MASK_TXPRI);
}

bool MCP2515_Sleep(void)
{
    return MCP2515_Set_Mode(MCP_MODE_SLEEP);
}

/* W */
bool MCP2515_WakeUp(void)
{
    MCP2515_BitModify(MCP_REG_CANINTF, MCP_BIT_WAKIF, 0x00);
    return MCP2515_Set_Mode(MCP_MODE_NORMAL);
}

void MCP2515_Write_Register(uint8_t regAddr, uint8_t value)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_WRITE);
    SPI2_Transmit_Receive_Byte(regAddr);
    SPI2_Transmit_Receive_Byte(value);
    SPI2_Chip_Select(0);
}
/*******************************************************************************
 * Function Name : MCP2515_Write_Register
 * Description   : Writes a single byte of data to a specified register address
 *                 in the MCP2515 CAN controller via the SPI bus.
 * Scope         : Global / Public
 * Parameters    : regAddr - Register memory address to write to (uint8_t)
 *                 value   - Data byte to be written to the register (uint8_t)
 * Return Value  : None
 ******************************************************************************/
void MCP2515_Write_Registers(uint8_t startAddr, const uint8_t *pData, uint8_t len)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_WRITE);
    SPI2_Transmit_Receive_Byte(startAddr);
    for (uint8_t i = 0; i < len; i++)
    {
        SPI2_Transmit_Receive_Byte(pData[i]);
    }
    SPI2_Chip_Select(0);
}
bool MCP2515_EchoMessage(CAN_Message_t *rxMsg)
{

    // 1. Check if there is a message in the RX buffer and read it
    if (MCP2515_ReceiveMessage(rxMsg))
    {
    	return true;
    }

    return false; // No message received
}


/*******************************************************************************
 * Function Name : MCP2515_Reset
 * Description   : Sends an SPI software reset command to the MCP2515 CAN
 *                 controller to restore hardware registers to default states
 *                 and enters Configuration Mode, followed by a blocking delay
 *                 for internal startup.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void MCP2515_Reset(void)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_RESET);
    SPI2_Chip_Select(0);
    for (volatile int i = 0; i < 5000; i++);
}
/*******************************************************************************
 * Function Name : MCP2515_Init
 * Description   : Resets, configures bit timing, sets up receive buffers,
 *                 and transitions the MCP2515 to Normal operating mode.
 * Scope         : Only in this .c file
 * Parameters    : baud  - Desired CAN bus bit rate (MCP2515_BaudRate_t)
 *                 clock - Frequency of the external crystal/oscillator (MCP2515_Clock_t)
 * Return Value  : bool (true = Initialization successful, false = Failed)
 ******************************************************************************/
static bool MCP2515_Init(MCP2515_BaudRate_t baud, MCP2515_Clock_t clock)
{
    MCP2515_Reset();
    if (MCP2515_Get_Mode() != MCP_MODE_CONFIG)
	{
	 return false;
	}
    if (!MCP2515_Set_Bitrate(baud, clock))
    {
    	return false;
    }
    MCP2515_Write_Register(MCP_REG_RXB0CTRL, MCP_MASK_RXB0_RXM | MCP_BIT_RX0_BUKT);
    MCP2515_Write_Register(MCP_REG_RXB1CTRL, MCP_MASK_RXB1_RXM);
    return MCP2515_Set_Mode(MCP_MODE_NORMAL);
}
/*******************************************************************************
 * Function Name : Micro_Chip_Product_2515_Init
 * Description   : Wrapper function that initializes the MCP2515 CAN controller
 *                 with default application settings (250 Kbps, 8 MHz clock).
 * Scope         : Global
 * Parameters    : None
 * Return Value  : bool (true = Initialization successful, false = Failed)
 ******************************************************************************/
bool Micro_Chip_Product_2515_Init(void)
{
	return MCP2515_Init(CAN_SPEED_250KBPS, MCP_CLOCK_8MHZ);
}
