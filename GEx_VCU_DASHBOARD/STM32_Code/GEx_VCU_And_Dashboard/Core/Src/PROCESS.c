/*
 * PROCESS.c
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */


#include"PROCESS.h"
#include"UART.h"
#include"DECODE_MOTOR_CONTROLLER.h"
#include"DECODE_BATTERY_MANAGEMENT_SYSTEM.h"
static uint32_t Bms_Last_Updated,Mcu_Last_Updated;
uint16_t id[]={0x100,0x101,0x102,0x104,0x105,0x106,0x107,0x108,0x109,0x10a,0x10b,0x10C,0x10D,0x10E};
void Print_CAN_Message(const CAN_Message_t *frame)
{
    // Print Identifier and Type
    if (frame->isExtended)
    {
       Uart_Printf("CAN ID: 0x%08lX [Ext] ", (unsigned long)frame->id);
    }
    else
    {
    	Uart_Printf("CAN ID: 0x%03lX [Std] ", (unsigned long)frame->id);
    }

    // Check if it's a Remote Frame or Data Frame
    if (frame->isRTR)
    {
    	Uart_Printf("| Type: RTR | Requested DLC: %u\r\n", frame->dlc);
    }
    else
    {
    	Uart_Printf("| Type: DATA | DLC: %u | Data: ", frame->dlc);

        // Print data bytes in Hex format
        for (uint8_t i = 0; i < frame->dlc && i < 8; i++)
        {
        	Uart_Printf("0x%02X ", frame->data[i]);
        }
        Uart_Printf("%c\n",'\n');
    }
}
uint8_t id_size=sizeof(id)/sizeof(id[0]);
void Process_Can_Messages()
{
	CAN_Message_t rx_msg;
	while(Can_Collect_Frame(&rx_msg))
	{
//		Print_CAN_Message(&rx_msg);
		Bms_Last_Updated=Get_Tick_Ms();
		Process_Bms_Messages(&rx_msg);
	}
	while(MCP2515_Can_Collect_Frame(&rx_msg))
	{
	 Mcu_Last_Updated=Get_Tick_Ms();
//	 		Print_CAN_Message(&rx_msg);
      Process_Motor_Controller_Messages(&rx_msg);
      Can_Send_Frame(&rx_msg);
	}
}
void Request_Bms_Messages()
{
	CAN_Message_t rx_frame;
	   static uint8_t i=0;
	   rx_frame.id=id[i];
	   rx_frame.isExtended=0;
	   rx_frame.dlc=8;
	   rx_frame.isRTR=1;
	   Can_Send_Frame(&rx_frame);
	   i++;
	   if(i>=id_size)
	   {
		   i=0;
	   }
}
bool Get_BMS_Can_Fault()
{
	if(Get_Tick_Ms()-Bms_Last_Updated>2000)
	{
		return true;
	}
	return false;
}
bool Get_MCU_Can_Fault()
{
	if(Get_Tick_Ms()-Mcu_Last_Updated>2000)
	{
		return true;
	}
	return false;
}
void Check_Bms_Mcu_Can_Data(GEx_Display_t* System_Data)
{
	System_Data->System_Data.Is_Bms_Can_Ok=!Get_BMS_Can_Fault();
	System_Data->System_Data.Is_Mcu_Can_Ok=!Get_MCU_Can_Fault();
}
