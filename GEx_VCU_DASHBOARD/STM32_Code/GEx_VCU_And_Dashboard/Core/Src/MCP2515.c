/*
 * MCP2515.c
 *
 *  Created on: Aug 3, 2026
 *      Author: MadaYaswanth
 */

#include"MCP2515.h"
#include"MCP2515_REG.h"
#include"DECODE_MOTOR_CONTROLLER.h"

#include"SPI.h"
/*******************************************************************************
 * Function Name : MCP2515_Read_Register
 * Description   : Reads a single byte from a specified MCP2515 register via SPI.
 * Scope         : Static (Private to this file)
 * Parameters    : regAddr - Address of the register to read
 * Return Value  : uint8_t (Data byte read from the register)
 ******************************************************************************/
static uint8_t MCP2515_Read_Register(uint8_t regAddr)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_READ);
    SPI2_Transmit_Receive_Byte(regAddr);
    uint8_t val = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    SPI2_Chip_Select(0);
    return val;
}
/*******************************************************************************
 * Function Name : MCP2515_Get_Mode
 * Description   : Reads the CANSTAT status register and extracts the current
 *                 operating mode of the MCP2515 CAN controller.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : MCP2515_Mode_t (Current mode: CONFIG, NORMAL, SLEEP, etc.)
 ******************************************************************************/
static MCP2515_Mode_t MCP2515_Get_Mode(void)
{
    return (MCP2515_Mode_t)(MCP2515_Read_Register(MCP_REG_CANSTAT) & MCP_MASK_MODE);
}
/*******************************************************************************
 * Function Name : MCP2515_Bit_Modify
 * Description   : Modifies specific bits of an MCP2515 register using the Bit Modify instruction.
 * Scope         : Static (Private to this file)
 * Parameters    : Reg_Addr - Target register address
 *                 Mask     - Bit mask specifying which bits to modify (1 = modify, 0 = ignore)
 *                 Value    - Value to write to the masked bits
 * Return Value  : None
 ******************************************************************************/
static void MCP2515_Bit_Modify(uint8_t Reg_Addr, uint8_t Mask, uint8_t Value)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_BITMOD);
    SPI2_Transmit_Receive_Byte(Reg_Addr);
    SPI2_Transmit_Receive_Byte(Mask);
    SPI2_Transmit_Receive_Byte(Value);
    SPI2_Chip_Select(0);
}
/*******************************************************************************
 * Function Name : MCP2515_Clear_Rx_Buffer
 * Description   : Clears the receive interrupt flag for a specified RX buffer.
 * Scope         : Static (Private to this file)
 * Parameters    : bufferNum - Buffer identifier (0 for RXB0, 1 for RXB1)
 * Return Value  : None
 ******************************************************************************/
static void MCP2515_Clear_Rx_Buffer(uint8_t bufferNum)
{
    if (bufferNum == 0)
    {
    	MCP2515_Bit_Modify(MCP_REG_CANINTF, MCP_BIT_RX0IF, 0x00);
    }
    else if (bufferNum == 1)
    {
    	MCP2515_Bit_Modify(MCP_REG_CANINTF, MCP_BIT_RX1IF, 0x00);
    }
}
/*******************************************************************************
 * Function Name : MCP2515_Write_Register
 * Description   : Writes a single byte value to a specified MCP2515 register via SPI.
 * Scope         : Static (Private to this file)
 * Parameters    : Reg_Addr - Target register address
 *                 Value    - Data byte to write to the register
 * Return Value  : None
 ******************************************************************************/
static void MCP2515_Write_Register(uint8_t Reg_Addr, uint8_t Value)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_WRITE);
    SPI2_Transmit_Receive_Byte(Reg_Addr);
    SPI2_Transmit_Receive_Byte(Value);
    SPI2_Chip_Select(0);
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
static bool MCP2515_Set_Mode(MCP2515_Mode_t mode)
{
    MCP2515_Bit_Modify(MCP_REG_CANCTRL, MCP_MASK_MODE, mode);
    for (uint32_t timeout = 0; timeout < 10000; timeout++)
    {
        if (MCP2515_Get_Mode() == mode)
        {
        	return true;
        }
    }
    return false;
}

/*******************************************************************************
 * Function Name : MCP2515_Read_Rx_Buffer
 * Description   : Reads an entire CAN message structure directly from an RX buffer
 *                 via SPI command, handling both standard and extended identifiers.
 * Scope         : Static (Private to this file)
 * Parameters    : Rx_Buffer_Command - SPI read instruction command for the specific RX buffer
 *                 Message           - Pointer to the CAN_Message_t structure to populate
 * Return Value  : bool (true always)
 ******************************************************************************/
static bool MCP2515_Read_Rx_Buffer(uint8_t Rx_Buffer_Command, CAN_Message_t *Message)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(Rx_Buffer_Command);
    uint8_t sidh = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t sidl = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t eid8 = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t eid0 = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    uint8_t dlc  = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    Message-> isExtended = (sidl & MCP_BIT_EXIDE) != 0;
    if (Message->isExtended)
    {
        Message->id = ((uint32_t)sidh << 21) | (((uint32_t)sidl & 0xE0) << 13) |
                  (((uint32_t)sidl & 0x03) << 16) | ((uint32_t)eid8 << 8) | eid0;
    }
    else
    {
        Message->id = ((uint32_t)sidh << 3) | (sidl >> 5);
    }
    Message->dlc = dlc & MCP_MASK_DLC;
    Message->isRTR = (dlc & MCP_BIT_RTR) != 0;
    for (uint8_t i = 0; i < Message->dlc; i++)
    {
        Message->data[i] = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    }
    SPI2_Chip_Select(0);
    return true;
}
/*******************************************************************************
 * Function Name : MCP2515_Receive_Message
 * Description   : Checks for pending received messages across buffers, reads them,
 *                 and clears the corresponding interrupt flags.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the CAN_Message_t structure to store the received message
 * Return Value  : bool (true = Message successfully read, false = No messages pending)
 ******************************************************************************/
static bool MCP2515_Receive_Message(CAN_Message_t *Message)
{
    uint8_t Flags = MCP2515_Read_Register(MCP_REG_CANINTF);

    if (Flags & MCP_BIT_RX0IF)
    {
        MCP2515_Read_Rx_Buffer(MCP_CMD_READ_RX_BUF0, Message);
        MCP2515_Clear_Rx_Buffer(0);
        return true;
    }
    else if (Flags & MCP_BIT_RX1IF)
    {
        MCP2515_Read_Rx_Buffer(MCP_CMD_READ_RX_BUF1, Message);
        MCP2515_Clear_Rx_Buffer(1);
        return true;
    }

    return false;
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
/*******************************************************************************
 * Function Name : MCP2515_Set_Filter
 * Description   : Configures a specified receive acceptance filter (RXF0-RXF5)
 *                 for standard or extended CAN identifiers, temporarily switching
 *                 to configuration mode.
 * Scope         : Static (Private to this file)
 * Parameters    : Filter_Num  - Target filter index (MCP2515_Filter_t)
 *                 Is_Extended - Flag indicating whether the filter is extended (true) or standard (false)
 *                 filterValue - Identifier filter value to write
 * Return Value  : bool (true = Filter configured successfully, false = Invalid filter number)
 ******************************************************************************/
static bool MCP2515_Set_Filter(MCP2515_Filter_t Filter_Num, bool Is_Extended, uint32_t filterValue)
{
    MCP2515_Mode_t Saved_Mode = MCP2515_Get_Mode();
    MCP2515_Set_Mode(MCP_MODE_CONFIG);
    uint8_t Base_Addr;
    switch (Filter_Num)
    {
        case MCP_FILTER_RXF0: Base_Addr = MCP_REG_RXF0SIDH; break;
        case MCP_FILTER_RXF1: Base_Addr = MCP_REG_RXF1SIDH; break;
        case MCP_FILTER_RXF2: Base_Addr = MCP_REG_RXF2SIDH; break;
        case MCP_FILTER_RXF3: Base_Addr = MCP_REG_RXF3SIDH; break;
        case MCP_FILTER_RXF4: Base_Addr = MCP_REG_RXF4SIDH; break;
        case MCP_FILTER_RXF5: Base_Addr = MCP_REG_RXF5SIDH; break;
        default: return false;
    }

    if (Is_Extended)
    {
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDH, (uint8_t)(filterValue >> 21));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDL, (uint8_t)(((filterValue >> 13) & 0xE0) | MCP_BIT_EXIDE | ((filterValue >> 16) & 0x03)));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID8, (uint8_t)(filterValue >> 8));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID0, (uint8_t)(filterValue));
    }
    else
    {
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDH, (uint8_t)(filterValue >> 3));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDL, (uint8_t)((filterValue & 0x07) << 5));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID8, 0x00);
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID0, 0x00);
    }

    MCP2515_Set_Mode(Saved_Mode);
    return true;
}
/*******************************************************************************
 * Function Name : MCP2515_Set_Mask
 * Description   : Configures a specified acceptance mask (RXM0 or RXM1)
 *                 for standard or extended CAN identifiers, temporarily switching
 *                 to configuration mode.
 * Scope         : Static (Private to this file)
 * Parameters    : Mask_Num    - Target mask index (MCP2515_Mask_t)
 *                 Is_Extended - Flag indicating whether the mask is extended (true) or standard (false)
 *                 Mask_Value  - Acceptance mask value to write
 * Return Value  : bool (true always upon execution)
 ******************************************************************************/
static bool MCP2515_Set_Mask(MCP2515_Mask_t Mask_Num, bool Is_Extended, uint32_t Mask_Value)
{
    MCP2515_Mode_t Saved_Mode = MCP2515_Get_Mode();
    MCP2515_Set_Mode(MCP_MODE_CONFIG);

    uint8_t Base_Addr = (Mask_Num == MCP_MASK_RXM0) ? MCP_REG_RXM0SIDH : MCP_REG_RXM1SIDH;

    if (Is_Extended)
    {
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDH, (uint8_t)(Mask_Value >> 21));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDL, (uint8_t)(((Mask_Value >> 13) & 0xE0) | MCP_BIT_EXIDE | ((Mask_Value >> 16) & 0x03)));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID8, (uint8_t)(Mask_Value >> 8));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID0, (uint8_t)(Mask_Value));
    }
    else
    {
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDH, (uint8_t)(Mask_Value >> 3));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDL, (uint8_t)((Mask_Value & 0x07) << 5));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID8, 0x00);
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID0, 0x00);
    }

    MCP2515_Set_Mode(Saved_Mode);
    return true;
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
 * Function Name : MCP2515_Apply_Id_Filters
 * Description   : Configures the MCP2515 hardware filters dynamically using arrays
 *                 of standard and extended IDs. Because the controller only has
 *                 6 hardware filters total, the combined count cannot exceed 6.
 *                 Unused filter slots are automatically padded with a dummy ID
 *                 to prevent unwanted traffic from slipping through.
 * Scope         : Static (Private to this file)
 * Parameters    : std_ids     - Pointer to an array of standard 11-bit CAN IDs
 *                 std_count   - Number of standard IDs in the array
 *                 ext_ids     - Pointer to an array of extended 29-bit CAN IDs
 *                 ext_count   - Number of extended IDs in the array
 * Return Value  : bool (true if applied successfully, false if total IDs exceed 6)
 ******************************************************************************/
static bool MCP2515_Set_Filters(const uint16_t *std_ids, uint8_t std_count, const uint32_t *ext_ids, uint8_t ext_count)
{
    if ((std_count + ext_count) > 6) {
        return false;
    }

    MCP2515_Mode_t saved_mode = MCP2515_Get_Mode();
    MCP2515_Set_Mode(MCP_MODE_CONFIG);

    MCP2515_Set_Mask(MCP_MASK_RXM0, false, 0x7FF);
    MCP2515_Set_Mask(MCP_MASK_RXM1, false, 0x7FF);

    uint8_t filter_idx = 0;

    for (uint8_t i = 0; i < std_count; i++) {
        MCP2515_Set_Filter((MCP2515_Filter_t)filter_idx++, false, std_ids[i]);
    }

    for (uint8_t i = 0; i < ext_count; i++) {
        MCP2515_Set_Filter((MCP2515_Filter_t)filter_idx++, true, ext_ids[i]);
    }

    uint16_t dummy_std = (std_count > 0) ? std_ids[0] : 0x000;
    for (; filter_idx < 6; filter_idx++) {
        MCP2515_Set_Filter((MCP2515_Filter_t)filter_idx, false, dummy_std);
    }

    MCP2515_Set_Mode(saved_mode);
    return true;
}
/*******************************************************************************
 * Function Name : MCP2515_Is_Message_Available
 * Description   : Checks if a new CAN message has been received in either
 *                 receive buffer.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = Message waiting in RXB0 or RXB1, false = No pending messages)
 ******************************************************************************/
static bool MCP2515_Is_Message_Available(void)
{
    uint8_t Flags = MCP2515_Read_Register(MCP_REG_CANINTF);
    return (Flags & MCP_MASK_RXIF) != 0;
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
static bool MCP2515_Init(MCP2515_BaudRate_t Baud, MCP2515_Clock_t Clock)
{
    MCP2515_Reset();
    if (MCP2515_Get_Mode() != MCP_MODE_CONFIG)
	{
	 return false;
	}
    if (!MCP2515_Set_Bitrate(Baud, Clock))
    {
    	return false;
    }
    MCP2515_Write_Register(MCP_REG_RXB0CTRL, MCP_MASK_RXB0_RXM | MCP_BIT_RX0_BUKT);
    MCP2515_Write_Register(MCP_REG_RXB1CTRL, MCP_MASK_RXB1_RXM);
    return MCP2515_Set_Mode(MCP_MODE_NORMAL);
}
bool Micro_Chip_Product_2515_Init(void)
{
	return MCP2515_Init(CAN_SPEED_250KBPS, MCP_CLOCK_8MHZ);
}
bool MCP2515_Can_Collect_Frame(CAN_Message_t *Frame)
{
	if(MCP2515_Is_Message_Available())
	{
		return MCP2515_Receive_Message(Frame);
	}
	return false;
}
bool Configure_Mcp2515_Can_Ids()
{
	uint16_t Std_Id[]={
#ifdef MCU_NANJING_QUARK
 MCU_MSG_0x011_ID  ,
 MCU_MSG_0x012_ID  ,
 MCU_MSG_0x013_ID  ,
MCU_MSG_0x014_ID
#endif
	};
	return MCP2515_Set_Filters(Std_Id,sizeof(Std_Id)/sizeof(Std_Id[0]),NULL,0);
}


/*
 Now not using required for future purpose
 * */

/*******************************************************************************
 * Function Name : MCP2515_Sleep
 * Description   : Puts the MCP2515 into Sleep operating mode for power saving.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = Mode change successful, false = Mode change failed)
 ******************************************************************************
static bool MCP2515_Sleep(void)
{
    return MCP2515_Set_Mode(MCP_MODE_SLEEP);
}*/
/*******************************************************************************
 * Function Name : MCP2515_DeInit
 * Description   : De-initializes the MCP2515 by putting it into sleep mode
 *                 and shutting down the SPI2 interface.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************
static void MCP2515_DeInit(void)
{
    MCP2515_Sleep();
    SPI2_DeInit();
}*/

/*******************************************************************************
 * Function Name : MCP2515_Get_Error_Flags
 * Description   : Retrieves the error flag register (EFLG) contents from the MCP2515.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (Error flag register value)
 ******************************************************************************
static uint8_t MCP2515_Get_Error_Flags(void)
{
    return MCP2515_Read_Register(MCP_REG_EFLG);
}*/
/*******************************************************************************
 * Function Name : MCP2515_Get_Rx_Error_Count
 * Description   : Retrieves the Receive Error Counter (REC) register value.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (Current value of the REC register)
 ******************************************************************************
static uint8_t MCP2515_Get_Rx_Error_Count(void)
{
    return MCP2515_Read_Register(MCP_REG_REC);
}*/
/*******************************************************************************
 * Function Name : MCP2515_Get_Tx_Error_Count
 * Description   : Retrieves the Transmit Error Counter (TEC) register value.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (Current value of the TEC register)
 ******************************************************************************
static uint8_t MCP2515_Get_Tx_Error_Count(void)
{
    return MCP2515_Read_Register(MCP_REG_TEC);
}*/
/*******************************************************************************
 * Function Name : MCP2515_Is_Buffer_Available
 * Description   : Checks if at least one transmit buffer is available and ready
 *                 for loading a new message.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = At least one TX buffer is free, false = All TX buffers pending)
 ******************************************************************************
static bool MCP2515_Is_Buffer_Available(void)
{
    uint8_t Ctrl0 = MCP2515_Read_Register(MCP_REG_TXB0CTRL);
    uint8_t Ctrl1 = MCP2515_Read_Register(MCP_REG_TXB1CTRL);
    uint8_t Ctrl2 = MCP2515_Read_Register(MCP_REG_TXB2CTRL);

    return (!(Ctrl0 & MCP_BIT_TXREQ) || !(Ctrl1 & MCP_BIT_TXREQ) || !(Ctrl2 & MCP_BIT_TXREQ));
}*/
/*******************************************************************************
 * Function Name : MCP2515_Load_Tx_Buffer
 * Description   : Formats and loads a CAN message into the specified transmit buffer
 *                 registers (Standard or Extended frame support).
 * Scope         : Static (Private to this file)
 * Parameters    : Buffer_Idx - Target transmit buffer index (MCP2515_TxBuffer_t)
 *                 Message    - Pointer to the CAN message structure to load (CAN_Message_t*)
 * Return Value  : bool (true = Buffer successfully loaded, false = Validation failed)
 ******************************************************************************
static bool MCP2515_Load_Tx_Buffer(MCP2515_TxBuffer_t Buffer_Idx, const CAN_Message_t *Message)
{
    if (Message == NULL || Message->dlc > 8)
    {
    	return false;
    }

    uint8_t Base_Addr = (Buffer_Idx == MCP_TXB0) ? MCP_REG_TXB0SIDH :
                       (Buffer_Idx == MCP_TXB1) ? MCP_REG_TXB1SIDH : MCP_REG_TXB2SIDH;

    if (Message->isExtended)
    {
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDH, (uint8_t)(Message->id >> 21));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDL, (uint8_t)(((Message->id >> 13) & 0xE0) | MCP_BIT_EXIDE | ((Message->id >> 16) & 0x03)));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID8, (uint8_t)(Message->id >> 8));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID0, (uint8_t)(Message->id));
    }
    else
    {
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDH, (uint8_t)(Message->id >> 3));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_SIDL, (uint8_t)((Message->id & 0x07) << 5));
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID8, 0x00);
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_EID0, 0x00);
    }

    uint8_t Dlc_Reg = (Message->dlc & MCP_MASK_DLC) | (Message->isRTR ? MCP_BIT_RTR : 0x00);
    MCP2515_Write_Register(Base_Addr + MCP_OFFSET_DLC, Dlc_Reg);

    for (uint8_t i = 0; i < Message->dlc; i++)
    {
        MCP2515_Write_Register(Base_Addr + MCP_OFFSET_DATA + i, Message->data[i]);
    }

    return true;
}*/
/*******************************************************************************
 * Function Name : MCP2515_Modify_Bit
 * Description   : Wrapper function to modify specific bits of an MCP2515 register.
 * Scope         : Static (Private to this file)
 * Parameters    : Reg_Addr - Target register address
 *                 Mask     - Bit mask to apply
 *                 Value    - Value to write
 * Return Value  : None
 ******************************************************************************
static void MCP2515_Modify_Bit(uint8_t Reg_Addr, uint8_t Mask, uint8_t Value)
{
    MCP2515_Bit_Modify(Reg_Addr, Mask, Value);
}*/
/*******************************************************************************
 * Function Name : MCP2515_Normal_Mode
 * Description   : Sets the MCP2515 operating mode to Normal Operation.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = Mode change successful, false = Mode change failed)
 ******************************************************************************
static bool MCP2515_Normal_Mode(void)
{
    return MCP2515_Set_Mode(MCP_MODE_NORMAL);
}*/
/*******************************************************************************
 * Function Name : MCP2515_Read_Registers
 * Description   : Reads a sequence of contiguous registers starting from a base address.
 * Scope         : Static (Private to this file)
 * Parameters    : startAddr - Starting register address
 *                 pData     - Pointer to the buffer where received bytes will be stored
 *                 len       - Number of bytes to read
 * Return Value  : None
 ******************************************************************************
static void MCP2515_Read_Registers(uint8_t startAddr, uint8_t *pData, uint8_t len)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_READ);
    SPI2_Transmit_Receive_Byte(startAddr);
    for (uint8_t i = 0; i < len; i++)
    {
        pData[i] = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    }
    SPI2_Chip_Select(0);
}*/
/*******************************************************************************
 * Function Name : MCP2515_Read_Status
 * Description   : Reads the status instruction output from the MCP2515.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (Status flags byte)
 ******************************************************************************
static uint8_t MCP2515_Read_Status(void)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_READ_STATUS);
    uint8_t status = SPI2_Transmit_Receive_Byte(MCP_SPI_DUMMY);
    SPI2_Chip_Select(0);
    return status;
}*/

/*******************************************************************************
 * Function Name : MCP2515_Poll_Status
 * Description   : Polls the general status byte of the MCP2515.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (Status byte containing filter and transmit flags)
 ******************************************************************************
static uint8_t MCP2515_Poll_Status(void)
{
    return MCP2515_Read_Status();
}*/
/*******************************************************************************
 * Function Name : MCP2515_Request_To_Send
 * Description   : Triggers a Request-to-Send (RTS) command for one or more transmit buffers.
 * Scope         : Static (Private to this file)
 * Parameters    : txBufferMask - Bitmask representing which buffer(s) to request sending for (MCP2515_TxBuffer_t)
 * Return Value  : None
 ******************************************************************************
static void MCP2515_Request_To_Send(MCP2515_TxBuffer_t txBufferMask)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_RTS | (txBufferMask & MCP_TXB_ALL));
    SPI2_Chip_Select(0);
}
*/
/*******************************************************************************
 * Function Name : MCP2515_Send_Message
 * Description   : Automatically finds an available transmit buffer, loads the message,
 *                 and triggers transmission via Request-to-Send.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the CAN message structure to send (CAN_Message_t*)
 * Return Value  : bool (true = Message loaded and sent, false = All buffers busy or loading failed)
 ******************************************************************************
static bool MCP2515_Send_Message(const CAN_Message_t *Message)
{
    MCP2515_TxBuffer_t targetBuffer;
    uint8_t ctrl0 = MCP2515_Read_Register(MCP_REG_TXB0CTRL);

    if (!(ctrl0 & MCP_BIT_TXREQ))
    {
    	targetBuffer = MCP_TXB0;
    }
    else
    {
        uint8_t ctrl1 = MCP2515_Read_Register(MCP_REG_TXB1CTRL);
        if (!(ctrl1 & MCP_BIT_TXREQ)) targetBuffer = MCP_TXB1;
        else
        {
            uint8_t ctrl2 = MCP2515_Read_Register(MCP_REG_TXB2CTRL);
            if (!(ctrl2 & MCP_BIT_TXREQ)) targetBuffer = MCP_TXB2;
            else return false;
        }
    }

    if (!MCP2515_Load_Tx_Buffer(targetBuffer, Message)) return false;
    MCP2515_Request_To_Send(targetBuffer);
    return true;
}*/
/*******************************************************************************
 * Function Name : MCP2515_Write_Register
 * Description   : Writes a single byte of data to a specified register address
 *                 in the MCP2515 CAN controller via the SPI bus.
 * Scope         : Only in this .c file
 * Parameters    : regAddr - Register memory address to write to (uint8_t)
 *                 value   - Data byte to be written to the register (uint8_t)
 * Return Value  : None
 ******************************************************************************
static void MCP2515_Write_Registers(uint8_t startAddr, const uint8_t *pData, uint8_t len)
{
    SPI2_Chip_Select(1);
    SPI2_Transmit_Receive_Byte(MCP_CMD_WRITE);
    SPI2_Transmit_Receive_Byte(startAddr);
    for (uint8_t i = 0; i < len; i++)
    {
        SPI2_Transmit_Receive_Byte(pData[i]);
    }
    SPI2_Chip_Select(0);
}*/
