/*
 * CAN.c
 *
 *  Created on: Aug 6, 2026
 *      Author: MadaYaswanth
 */

#include"main.h"
#include"RCC.h"
#include"CAN.h"
/*******************************************************************************
 * Function Name : CAN_Filter_Init
 * Description   : Configures hardware acceptance filters on CAN1 for Standard
 *                 (11-bit) and Extended (29-bit) CAN IDs, routing standard
 *                 frames to FIFO0 and extended frames to FIFO1.
 * Scope         : Static (Private to this file)
 * Parameters    : stdID    - Array of standard 11-bit CAN IDs to accept
 *                 stdCount - Number of standard CAN IDs in array
 *                 extID    - Array of extended 29-bit CAN IDs to accept
 *                 extCount - Number of extended CAN IDs in array
 * Return Value  : None
 ******************************************************************************/
static void CAN_Filter_Init(const uint16_t stdID[], uint8_t stdCount,
                     const uint32_t extID[], uint8_t extCount)
{
    uint8_t i;
    CAN1->FMR |= CAN_FMR_FINIT;
    if (stdCount == 0)
    {
    	CAN1->FA1R &= ~(1U << 0);
    	CAN1->FS1R |= (1U << 0);
    	CAN1->FM1R &= ~(1U << 0);
    	CAN1->FFA1R &= ~(1U << 0);
    	CAN1->sFilterRegister[0].FR1 = 0x00000000;
    	CAN1->sFilterRegister[0].FR2 = (1U << 2);
    	CAN1->FA1R |= (1U << 0);
    }
    if (extCount == 0)
    {
    	CAN1->FA1R &= ~(1U << 1);
    	CAN1->FS1R |= (1U << 1);
    	CAN1->FM1R &= ~(1U << 1);
    	CAN1->FFA1R |= (1U << 1);
    	CAN1->sFilterRegister[1].FR1 = (1U << 2);
    	CAN1->sFilterRegister[1].FR2 = (1U << 2);
    	CAN1->FA1R |= (1U << 1);
    }
    for (i = 0; i < stdCount; i++)
    {
        CAN1->FA1R &= ~(1U << i);
        CAN1->FS1R |= (1U << i);
        CAN1->FM1R |= (1U << i);
        CAN1->FFA1R &= ~(1U << i);
        CAN1->sFilterRegister[i].FR1 = (stdID[i] << 21);
        CAN1->sFilterRegister[i].FR2 = 0;
        CAN1->FA1R |= (1U << i);
    }
    for (i = 0; i < extCount; i++)
    {
        uint8_t filter = stdCount + i;
        CAN1->FA1R &= ~(1U << filter);
        CAN1->FS1R |= (1U << filter);
        CAN1->FM1R |= (1U << filter);
        CAN1->FFA1R |= (1U << filter);
        CAN1->sFilterRegister[filter].FR1 = (extID[i] << 3) | (1U << 2);
        CAN1->sFilterRegister[filter].FR2 = 0;
        CAN1->FA1R |= (1U << filter);
    }
    CAN1->FMR &= ~CAN_FMR_FINIT;
}
/*******************************************************************************
 * Function Name : CAN1_Set_Bit_Timing
 * Description   : Configures the CAN1 Bit Timing Register (BTR) for desired
 *                 baud rates (250/500 Kbps) based on the current APB1 peripheral
 *                 clock frequency (36 MHz or 8 MHz).
 * Scope         : Static (Private to this file)
 * Parameters    : Baud_Value - Target CAN bus baud rate in Kbps (e.g., 250, 500)
 * Return Value  : bool (true = Bitrate configured, false = Unsupported frequency/baud)
 ******************************************************************************/
static bool CAN1_Set_Bit_Timing(uint16_t Baud_Value)
{
	if(Baud_Value==0)
	{
		return false;
	}
	switch(Get_APB1_freq())
	{
		case CLOCK_36MHZ:
		{
			switch(Baud_Value)
			{
			case CAN1_250_BAUD_RATE:
			{
			    CAN1->BTR =0x001c0008;
				return true;
			}
			case CAN1_500_BAUD_RATE:
			{
			    CAN1->BTR =0x001e0003;
			    return true;
			}
			}
			break;
		}
		case CLOCK_8MHZ:
		{
			switch(Baud_Value)
			{
			case CAN1_250_BAUD_RATE:
			{
			    CAN1->BTR = 0x001c0001;
			    return true;
			}
			case CAN1_500_BAUD_RATE:
			{
			    CAN1->BTR =0x001c0000;
			    return true;
			}
			}
			break;
		}
	}
	CAN1->BTR=0x00000000U;
	return false;
}
/*******************************************************************************
 * Function Name : CAN1_Init
 * Description   : Initializes CAN1 peripheral clocks, configures remapped GPIO
 *                 pins (PB8/PB9), sets up Master Control Register flags,
 *                 applies 500 Kbps bit timing, initializes default filters,
 *                 and transitions CAN1 to Normal operational mode.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = Initialization successful, false = Config/Clock failed)
 ******************************************************************************/
static bool CAN1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
    AFIO->MAPR &= ~AFIO_MAPR_CAN_REMAP;
    AFIO->MAPR |= AFIO_MAPR_CAN_REMAP_REMAP2;
    GPIOB->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
    GPIOB->CRH |= GPIO_CRH_CNF8_1;
    GPIOB->ODR |= GPIO_ODR_ODR8;
    GPIOB->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOB->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_MODE9_1;
    GPIOB->CRH |= GPIO_CRH_CNF9_1;
    CAN1->MCR &= ~CAN_MCR_SLEEP;
    CAN1->MCR |= CAN_MCR_INRQ;
    while ((CAN1->MSR & CAN_MSR_INAK) == 0);
    CAN1->MCR &= ~(CAN_MCR_TTCM |
                   CAN_MCR_ABOM |
                   CAN_MCR_AWUM |
                   CAN_MCR_NART |
                   CAN_MCR_RFLM |
                   CAN_MCR_TXFP);
    if(!CAN1_Set_Bit_Timing(CAN1_500_BAUD_RATE))
    {
    	return false;
    }
    CAN1->MCR &= ~CAN_MCR_INRQ;
    while (CAN1->MSR & CAN_MSR_INAK);
    CAN_Filter_Init(NULL,0,NULL,0);
    if (!(RCC->APB1ENR & RCC_APB1ENR_CAN1EN))
    {
    	return false;
    }
    return true;
}
/*******************************************************************************
 * Function Name : CAN1_Tx_Frame
 * Description   : Loads a CAN message frame (Standard/Extended ID, DLC, Data)
 *                 into a specified transmit mailbox register (sTxMailBox) and
 *                 requests transmission by setting the TXRQ bit.
 * Scope         : Static (Private to this file)
 * Parameters    : frame    - Pointer to CAN frame structure containing ID, DLC,
 *                            IDE flag, and data payload (const CAN_Message_t*)
 *                 mail_box - Index of the target transmit mailbox (0, 1, or 2)
 * Return Value  : None
 ******************************************************************************/
static void CAN1_Tx_Frame(const CAN_Message_t *frame,uint8_t mail_box )
{
	CAN1->sTxMailBox[mail_box].TIR = 0x00000000;
	if(frame->isExtended==0)
	{
		CAN1->sTxMailBox[mail_box].TIR |= (frame->id<<21);

	}
	else
	{
		CAN1->sTxMailBox[mail_box].TIR |= (frame->id<<3)|(1<<2);
	}
	CAN1->sTxMailBox[mail_box].TDTR &= ~(0xF<<0);
	CAN1->sTxMailBox[mail_box].TDTR |= (frame->dlc<<0);
	CAN1->sTxMailBox[mail_box].TDLR = 0x00000000;
	CAN1->sTxMailBox[mail_box].TDHR = 0x00000000;
	 for(uint8_t i=0;i<frame->dlc;i++)
	 {
		 if(i<4)
		 {
			 CAN1->sTxMailBox[mail_box].TDLR|=(frame->data[i]<<(i*8));
		 }
		 else
		 {
			 CAN1->sTxMailBox[mail_box].TDHR|=(frame->data[i]<<((i-4)*8));
		 }
	 }
	CAN1->sTxMailBox[mail_box].TIR |= (1<<0);
}
/*******************************************************************************
 * Function Name : CAN1_Rx_Frame
 * Description   : Extracts a received CAN message (ID, RTR flag, DLC, and data
 *                 payload) from the specified FIFO hardware mailbox and releases
 *                 the FIFO output mailbox slot.
 * Scope         : Static (Private to this file)
 * Parameters    : Rx_Frame    - Pointer to CAN frame structure to populate with
 *                               received message data (CAN_Message_t*)
 *                 Fifo_Number - Hardware receive FIFO index (0 or 1)
 * Return Value  : None
 ******************************************************************************/
static void CAN1_Rx_Frame(CAN_Message_t *Rx_Frame,uint8_t Fifo_Number)
{
	  if (Rx_Frame->isExtended == 0)
	  {
	    Rx_Frame->id = (uint32_t)0x000007FF & (CAN1->sFIFOMailBox[Fifo_Number].RIR >> 21);
	  }
	  else
	  {
	    Rx_Frame->id = (uint32_t)0x1FFFFFFF & (CAN1->sFIFOMailBox[Fifo_Number].RIR >> 3);
	  }
	  Rx_Frame->isRTR = (uint8_t)0x02 & CAN1->sFIFOMailBox[Fifo_Number].RIR;
	  Rx_Frame->dlc = (uint8_t)0x0F & CAN1->sFIFOMailBox[Fifo_Number].RDTR;
	  for(uint8_t i=0;i< Rx_Frame->dlc;i++){
		  if(i<4){
			  Rx_Frame->data[i] = (CAN1->sFIFOMailBox[Fifo_Number].RDLR>>(i*8));
		  }else{
			  Rx_Frame->data[i] = (CAN1->sFIFOMailBox[Fifo_Number].RDHR>>(((i-4)*8)));

		  }
	  }
	  if (Fifo_Number == 0)
	  {
		  CAN1->RF0R |= CAN_RF0R_RFOM0;
	  }
	  else if (Fifo_Number == 1)
	  {
		  CAN1->RF1R |= CAN_RF1R_RFOM1;
	  }
}
/*******************************************************************************
 * Function Name : CAN_Get_Free_Tx_Mailbox
 * Description   : Checks the Transmit Status Register (TSR) to locate the first
 *                 available hardware transmit mailbox (0, 1, or 2).
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (0, 1, or 2 if a mailbox is free; 3 if all are busy)
 ******************************************************************************/
static uint8_t CAN_Get_Free_Tx_Mailbox(void){
    uint32_t tsr_value = CAN1->TSR;

    if (tsr_value & CAN_TSR_TME0) {
        return 0; // Mailbox 0 is free
    }
    if (tsr_value & CAN_TSR_TME1) {
        return 1; // Mailbox 1 is free
    }
    if (tsr_value & CAN_TSR_TME2) {
        return 2; // Mailbox 2 is free
    }
    return 3; // No mailboxes are free
}
/*******************************************************************************
 * Function Name : CAN_Message_Pending
 * Description   : Checks receive FIFOs (FIFO0 and FIFO1) to determine if any
 *                 incoming CAN message frames are pending in hardware.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (0 for FIFO0 pending, 1 for FIFO1 pending, 3 if empty)
 ******************************************************************************/
static uint8_t CAN_Message_Pending(void)
{
    if ((CAN1->RF0R & CAN_RF0R_FMP0) != 0U)
    {
        return 0;
    }

    if ((CAN1->RF1R & CAN_RF1R_FMP1) != 0U)
    {
        return 1;
    }
    return 3;
}
bool Can_Send_Frame(CAN_Message_t *Frame)
{
	uint8_t Mail_Box=CAN_Get_Free_Tx_Mailbox();
	if(Mail_Box!=3)
	{
		CAN1_Tx_Frame(Frame,Mail_Box);
		return true;
	}
   return false;
}
bool Can_Collect_Frame(CAN_Message_t *Frame)
{
	uint8_t Mail_Box=CAN_Message_Pending();
	if(Mail_Box!=3)
	{
		CAN1_Rx_Frame(Frame,Mail_Box);
		return true;
	}
	return false;
}


bool Controller_Area_Network_Init()
{
	return CAN1_Init();
}
