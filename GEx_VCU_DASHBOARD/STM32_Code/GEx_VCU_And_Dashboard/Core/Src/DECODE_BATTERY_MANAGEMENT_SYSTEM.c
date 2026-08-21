/*
 * DECODE_BATTERY_MANAGEMENT_SYSTEM.c
 *
 *  Created on: Aug 10, 2026
 *      Author: MadaYaswanth
 */


#include"DECODE_BATTERY_MANAGEMENT_SYSTEM.h"

BMS_MSG_t Bms_Frames={0};
static int status_print_task_id=-1;
#ifdef JBD_BMS
uint8_t ntc[5];
uint8_t Avg_Temp=0;
const char Bms_Fault_Names[15][BMS_FAULT_NAME_LEN] =
{
	    "Cell Over Volt",
	    "Cell Under Volt",
	    "Pack Over Volt",
	    "Pack Under Volt",
	    "Charge Over Temp",
	    "Charge Low Temp",
	    "Dischg Over Temp",
	    "Dischg Low Temp",
	    "Charge Over Curr",
	    "Dischg Over Curr",
	    "Short Circuit",
	    "AFE IC Error",
	    "MOS Lock",
	    "Reserved",
	    "Reserved",
	    "Reserved"
};
/*******************************************************************************
 * Function Name : Decode_CAN_0x100
 * Description   : Decodes BMS CAN message 0x100 and updates the battery pack
 *                 voltage, charge/discharge current, remaining capacity, and
 *                 message reception timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x100 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x100(const uint8_t *Data, JBD_BMS_0x100_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->Total_Voltage = (float)((Data[0] << 8) | Data[1]) * 0.01f;
    Message->Charge_and_Discharge_Current =(float)(((Data[2] << 8) | Data[3]))* 0.01f;
    Message->Remaining_Capacity =(float)((Data[4] << 8) | Data[5]) * 0.01f;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x101
 * Description   : Decodes BMS CAN message 0x101 and updates the full battery
 *                 capacity, cycle count, state of charge, and message
 *                 reception timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x101 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x101(const uint8_t *Data, JBD_BMS_0x101_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->Full_Capacity = ((Data[0] << 8) | Data[1]) * 0.01f;
    Message->No_Cycles = (Data[2] << 8) | Data[3];
    Message->RSOC = Data[4]<<8|(Data[5]);
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x103
 * Description   : Decodes BMS CAN message 0x103 and updates the FET control
 *                 status, production date, software version, and message
 *                 reception timestamps.
 * Scope         : Global
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x103 BMS message structure
 * Return Value  : None
 ******************************************************************************/
void Decode_CAN_0x103(const uint8_t *Data, JBD_BMS_0x103_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->FET_Control_Status = (Data[0] << 8) | Data[1];
    Message->Production_Date = (Data[2] << 8) | Data[3];
    Message->Software_Version = (Data[4] << 8) | Data[5];
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x102
 * Description   : Decodes BMS CAN message 0x102 and extracts cell equalization
 *                 status bits and the BMS protection status from the received
 *                 CAN data.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x102 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x102(const uint8_t *Data, JBD_BMS_0x102_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    uint16_t low = (Data[0] << 8) | Data[1];
    for (int i = 0; i < 16; i++) Message->Equlization_Low_byte[i] = (low >> i) & 1;
    uint16_t high = (Data[2] << 8) | Data[3];
    for (int i = 0; i < 17; i++) Message->Equlization_Low_byte[16 + i] = (high >> i) & 1;
    Message->Protection_Status = (Data[4]) | Data[5]<<8;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x104
 * Description   : Decodes BMS CAN message 0x104 and updates the number of
 *                 battery strings, number of NTC temperature probes, and
 *                 message reception timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x104 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x104(const uint8_t *Data, JBD_BMS_0x104_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->Number_Of_Battery_Strings = Data[0];
    Message->NTC_Number_Probes = Data[1];
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x105
 * Description   : Decodes BMS CAN message 0x105 and converts the first three
 *                 NTC temperature values from the received data into degrees
 *                 Celsius while updating the message reception timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x105 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x105(const uint8_t *Data, JBD_BMS_0x105_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    ntc[0]=Message->NTC1 = (float)((Data[0] << 8) | Data[1]) * 0.1f - 273.15f;
    ntc[1]=Message->NTC2 = (float)((Data[2] << 8) | Data[3]) * 0.1f - 273.15f;
    ntc[2]=Message->NTC3 = (float)((Data[4] << 8) | Data[5]) * 0.1f - 273.15f;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x106
 * Description   : Decodes BMS CAN message 0x106 and converts the fourth and
 *                 fifth NTC temperature values into degrees Celsius. It also
 *                 calculates the average temperature of all five NTC probes.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x106 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x106(const uint8_t *Data, JBD_BMS_0x106_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    ntc[3]=Message->NTC4 = (float)((Data[0] << 8) | Data[1]) * 0.1f - 273.15f;
    ntc[4]=Message->NTC5 = (float)( (Data[2] << 8) | Data[3]) * 0.1f - 273.15f;
    Avg_Temp=(uint8_t)((ntc[0]+ntc[1]+ntc[2]+ntc[3]+ntc[4])/5);
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x107
 * Description   : Decodes BMS CAN message 0x107 and updates the voltage values
 *                 of battery cells 1 to 3 along with the message reception
 *                 timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x107 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x107(const uint8_t *Data,JBD_BMS_0x107_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL1 = (float)((Data[0] << 8) | Data[1])/1000.f;
    Message-> CELL2 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL3 = (float)((Data[4] << 8) | Data[5])/1000.f;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x108
 * Description   : Decodes BMS CAN message 0x108 and updates the voltage values
 *                 of battery cells 4 to 6 along with the message reception
 *                 timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x108 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x108(const uint8_t *Data,JBD_BMS_0x108_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL4 = (float)((Data[0] << 8) | Data[1])/1000.f;
    Message->CELL5 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL6 = (float)((Data[4] << 8) | Data[5])/1000.f;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x109
 * Description   : Decodes BMS CAN message 0x109 and updates the voltage values
 *                 of battery cells 7 to 9 along with the message reception
 *                 timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x109 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x109(const uint8_t *Data,JBD_BMS_0x109_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL7 = (float)((Data[0] << 8) | Data[1])/1000.f;
    Message->CELL8 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL9 = (float)((Data[4] << 8) | Data[5])/1000.f;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x10A
 * Description   : Decodes BMS CAN message 0x10A and updates the voltage values
 *                 of battery cells 10 to 12 along with the message reception
 *                 timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x10A BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x10A(const uint8_t *Data,JBD_BMS_0x10A_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL10 = (float)((Data[0] << 8) | Data[1])/1000.f;
    Message->CELL11 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL12 = (float)((Data[4] << 8) | Data[5])/1000.f;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x10B
 * Description   : Decodes BMS CAN message 0x10B and updates the voltage values
 *                 of battery cells 13 to 15 along with the message reception
 *                 timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x10B BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x10B(const uint8_t *Data,JBD_BMS_0x10B_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL13 = (float)((Data[0] << 8) | Data[1])/1000.f;
    Message->CELL14 = ((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL15 = ((Data[4] << 8) | Data[5])/1000.f;
}
/*******************************************************************************
 * Function Name : Decode_CAN_0x10C
 * Description   : Decodes BMS CAN message 0x10C and updates the voltage value
 *                 of battery cell 16 along with the message reception
 *                 timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x10C BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x10C(const uint8_t *Data,JBD_BMS_0x10C_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL16 = (float)((Data[0] << 8) | Data[1])/1000.f;
}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x100_t
 * Description   : Prints the decoded BMS CAN 0x100 message information,
 *                 including reception timestamps, battery voltage, current,
 *                 and remaining capacity, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x100 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x100_t(JBD_BMS_0x100_t * Message)
{
	Uart_Printf("------------ID=0X100-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"Voltage :%d\n"
			"Current :%d\n"
			"RSOC :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->Total_Voltage
			,(int)Message->Charge_and_Discharge_Current
			,(int)Message->Remaining_Capacity);
	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x105_t
 * Description   : Prints the decoded BMS CAN 0x105 message information,
 *                 including reception timestamps and NTC1 to NTC3 temperature
 *                 values, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x105 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x105_t(JBD_BMS_0x105_t *Message)
{
	Uart_Printf("------------ID=0X105-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"NTC1 :%d\n"
			"NTC2 :%d\n"
			"NTC3 :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->NTC1
			,(int)Message->NTC2
			,(int)Message->NTC3);

	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x106_t
 * Description   : Prints the decoded BMS CAN 0x106 message information,
 *                 including reception timestamps and NTC4 to NTC5 temperature
 *                 values, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x106 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x106_t(JBD_BMS_0x106_t *Message)
{
	Uart_Printf("------------ID=0X106-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"NTC4 :%d\n"
			"NTC5 :%d\n"
			"NTC6 :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->NTC4
			,(int)Message->NTC5
			,(int)Message->NTC6);

	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x107_t
 * Description   : Prints the decoded BMS CAN 0x107 message information,
 *                 including reception timestamps and cell 1 to cell 3
 *                 voltage values, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x107 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x107_t(JBD_BMS_0x107_t *Message)
{
	Uart_Printf("------------ID=0X107-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"CELL1 :%d\n"
			"CELL2 :%d\n"
			"CELL3 :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->CELL1
			,(int)Message->CELL2
			,(int)Message->CELL3);

	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x108_t
 * Description   : Prints the decoded BMS CAN 0x108 message information,
 *                 including reception timestamps and cell 4 to cell 6
 *                 voltage values, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x108 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x108_t(JBD_BMS_0x108_t *Message)
{
	Uart_Printf("------------ID=0X108-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"CELL4 :%d\n"
			"CELL5 :%d\n"
			"CELL6 :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->CELL4
			,(int)Message->CELL5
			,(int)Message->CELL6);
	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x109_t
 * Description   : Prints the decoded BMS CAN 0x109 message information,
 *                 including reception timestamps and cell 7 to cell 9
 *                 voltage values, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x109 BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x109_t(JBD_BMS_0x109_t *Message)
{
	Uart_Printf("------------ID=0X109-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"CELL7 :%d\n"
			"CELL8 :%d\n"
			"CELL9 :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->CELL7
			,(int)Message->CELL8
			,(int)Message->CELL9);

	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x10A_t
 * Description   : Prints the decoded BMS CAN 0x10A message information,
 *                 including reception timestamps and cell 10 to cell 12
 *                 voltage values, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x10A BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x10A_t(JBD_BMS_0x10A_t *Message)
{
	Uart_Printf("------------ID=0X10A-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"CELL10 :%d\n"
			"CELL11:%d\n"
			"CELL12 :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->CELL10
			,(int)Message->CELL11
			,(int)Message->CELL12);

	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x10B_t
 * Description   : Prints the decoded BMS CAN 0x10B message information,
 *                 including reception timestamps and cell 13 to cell 15
 *                 voltage values, over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x10B BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x10B_t(JBD_BMS_0x10B_t *Message)
{
	Uart_Printf("------------ID=0X10B-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"CELL13 :%d\n"
			"CELL14 :%d\n"
			"CELL15 :%d\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->CELL13
			,(int)Message->CELL14
			,(int)Message->CELL15);

	}
/*******************************************************************************
 * Function Name : Print_Jbd_Bms_0x10C_t
 * Description   : Prints the decoded BMS CAN 0x10C message information,
 *                 including reception timestamps and cell 16 voltage over
 *                 UART.
 * Scope         : Static (Private to this file)
 * Parameters    : Message - Pointer to the decoded 0x10C BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x10C_t(JBD_BMS_0x10C_t *Message)
{
	Uart_Printf("------------ID=0X10C-------------\n"
			"Last_RX :%d  Current_Rx :%d \n"
			"CELL16 :%d mV\n"
			"------------------------------------\n"
			,Message->Last_Received_Time
			,Message->Current_Received_Time
			,(int)Message->CELL16);

	}

#endif
/*******************************************************************************
 * Function Name : Print_Bms_Messages
 * Description   : Prints all decoded JBD BMS CAN messages over UART when BMS
 *                 communication is available. Reports a communication status
 *                 message when BMS messages are not received.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Print_Bms_Messages()
{
#ifdef JBD_BMS
	if(Bms_Frames.bms_comm_ok)
	{
		Print_Jbd_Bms_0x100_t(&Bms_Frames.JBD_BMS_0x100);
		Print_Jbd_Bms_0x105_t(&Bms_Frames.JBD_BMS_0x105);
		Print_Jbd_Bms_0x106_t(&Bms_Frames.JBD_BMS_0x106);
		Print_Jbd_Bms_0x107_t(&Bms_Frames.JBD_BMS_0x107);
		Print_Jbd_Bms_0x108_t(&Bms_Frames.JBD_BMS_0x108);
		Print_Jbd_Bms_0x109_t(&Bms_Frames.JBD_BMS_0x109);
		Print_Jbd_Bms_0x10A_t(&Bms_Frames.JBD_BMS_0x10A);
		Print_Jbd_Bms_0x10B_t(&Bms_Frames.JBD_BMS_0x10B);
		Print_Jbd_Bms_0x10C_t(&Bms_Frames.JBD_BMS_0x10C);

	}
	else
	{
		Uart_Printf("[INFO] :BMS Messages Not Received \n");
	}
#endif
}

/*******************************************************************************
 * Function Name : Process_Bms_Messages
 * Description   : Identifies received BMS CAN messages based on their CAN ID,
 *                 updates the BMS communication status, decodes the associated
 *                 message data, and reports whether the message was processed.
 * Scope         : Global
 * Parameters    : frame - Pointer to the received CAN message
 * Return Value  : true if the CAN message ID is recognized and processed;
 *                 false if the CAN message ID is not recognized.
 ******************************************************************************/
bool Process_Bms_Messages(CAN_Message_t *frame)
{
	 switch(frame->id)
	 {
#ifdef JBD_BMS
	 case JBD_BMS_ID0:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x100(frame->data,&Bms_Frames.JBD_BMS_0x100);
		 return true;
	 case JBD_BMS_ID1:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x101(frame->data,&Bms_Frames.JBD_BMS_0x101);
		 return true;
	 case JBD_BMS_ID2:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x102(frame->data,&Bms_Frames.JBD_BMS_0x102);
		 return true;
	 case JBD_BMS_ID3:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x103(frame->data,&Bms_Frames.JBD_BMS_0x103);
		 return true;
	 case JBD_BMS_ID4:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x104(frame->data,&Bms_Frames.JBD_BMS_0x104);
		 return true;
	 case JBD_BMS_ID5:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x105(frame->data,&Bms_Frames.JBD_BMS_0x105);
		 return true;
	 case JBD_BMS_ID6:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x106(frame->data,&Bms_Frames.JBD_BMS_0x106);
		 return true;
	 case JBD_BMS_ID7:
		 Decode_CAN_0x107(frame->data,&Bms_Frames.JBD_BMS_0x107);
		 return true;
	 case JBD_BMS_ID8:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x108(frame->data,&Bms_Frames.JBD_BMS_0x108);
		 return true;
	 case JBD_BMS_ID9:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x109(frame->data,&Bms_Frames.JBD_BMS_0x109);
		 return true;
	 case JBD_BMS_ID10:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x10A(frame->data,&Bms_Frames.JBD_BMS_0x10A);
		 return true;
	 case JBD_BMS_ID11:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x10B(frame->data,&Bms_Frames.JBD_BMS_0x10B);
		 return true;
	 case JBD_BMS_ID12:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x10C(frame->data,&Bms_Frames.JBD_BMS_0x10C);
	     return true;
#endif
	 default :break;
	 }
	 return false;
}
/*******************************************************************************
 * Function Name : Task_Id_For_Bms_Print
 * Description   : Registers the periodic BMS status printing task when it has
 *                 not already been registered and returns the task registration
 *                 status.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : true if the BMS print task is registered successfully;
 *                 false if task registration fails.
 ******************************************************************************/
static bool Task_Id_For_Bms_Print(void)
{
    if (status_print_task_id < 0)
    {
        status_print_task_id = Task_Timer_Register(3000, Print_Bms_Messages);
        if (status_print_task_id < 0)
        {
        	return false;
        }
    }
    return true;
}
/*******************************************************************************
 * Function Name : Enable_Bms_Print_Task
 * Description   : Enables the periodic BMS message printing task by registering
 *                 the BMS print task when required.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : true if the BMS print task is registered successfully;
 *                 false if task registration fails.
 ******************************************************************************/
bool Enable_Bms_Print_Task()
{
	return Task_Id_For_Bms_Print();
}
void Get_Bms_Data(BMS_Data_t* Bms_Data)
{
#ifdef JBD_BMS
	Bms_Data->Avg_Temp=Avg_Temp;
	Bms_Data->Battery_Current=Bms_Frames.JBD_BMS_0x100.Charge_and_Discharge_Current;
	Bms_Data->Battery_Pack_voltage=Bms_Frames.JBD_BMS_0x100.Total_Voltage;
	if( Bms_Frames.JBD_BMS_0x102.Protection_Status>0)
	{
		Bms_Data->Bms_Fault=true;
	}
	else
	{
		Bms_Data->Bms_Fault=false;
	}
   Bms_Data->Soc=Bms_Frames.JBD_BMS_0x101.RSOC;
   Bms_Data->Cells[0]=Bms_Frames.JBD_BMS_0x107.CELL1;
   Bms_Data->Cells[1]=Bms_Frames.JBD_BMS_0x107.CELL2;
   Bms_Data->Cells[2]=Bms_Frames.JBD_BMS_0x107.CELL3;
   Bms_Data->Cells[3]=Bms_Frames.JBD_BMS_0x108.CELL4;
   Bms_Data->Cells[4]=Bms_Frames.JBD_BMS_0x108.CELL5;
   Bms_Data->Cells[5]=Bms_Frames.JBD_BMS_0x108.CELL6;
   Bms_Data->Cells[6]=Bms_Frames.JBD_BMS_0x109.CELL7;
   Bms_Data->Cells[7]=Bms_Frames.JBD_BMS_0x109.CELL8;
   Bms_Data->Cells[8]=Bms_Frames.JBD_BMS_0x109.CELL9;
   Bms_Data->Cells[9]=Bms_Frames.JBD_BMS_0x10A.CELL10;
   Bms_Data->Cells[10]=Bms_Frames.JBD_BMS_0x10A.CELL11;
   Bms_Data->Cells[11]=Bms_Frames.JBD_BMS_0x10A.CELL12;
   Bms_Data->Cells[12]=Bms_Frames.JBD_BMS_0x10B.CELL13;
   Bms_Data->Cells[13]=Bms_Frames.JBD_BMS_0x10B.CELL14;
   Bms_Data->Cells[14]=Bms_Frames.JBD_BMS_0x10B.CELL15;
//   uint8_t fault_count = 0;
//   if(Bms_Frames.JBD_BMS_0x102.Protection_Status>0)
//   {
//	      fault_count = 0;
//	      for (uint8_t i = 0; i < 16; i++)
//	      {
//	          Bms_Data->Bms_Active_Fault[i][0] = '\0';
//	      }
//	      if (Bms_Frames.JBD_BMS_0x102.Protection_Status > 0)
//	      {
//	          uint16_t max_fault_names = sizeof(Bms_Fault_Names) / sizeof(Bms_Fault_Names[0]);
//
//	          for (uint8_t i = 0; i < 16 && i < 16 && i < max_fault_names; i++)
//	          {
//	              if (Bms_Frames.JBD_BMS_0x102.Protection_Status & (1U << i))
//	              {
//	                  if (fault_count < 16)
//	                  {
//	                      uint8_t dest_size = sizeof(Bms_Data->Bms_Active_Fault[0]);
//	                      strncpy(Bms_Data->Bms_Active_Fault[fault_count], Bms_Fault_Names[i], dest_size - 1);
//	                      Bms_Data->Bms_Active_Fault[fault_count][dest_size - 1] = '\0';
//
//	                      fault_count++;
//	                  }
//	              }
//	          }
//	      }
//	      Bms_Data->Bms_Fault_Count = fault_count;
//	      Bms_Data->Bms_Fault = (fault_count > 0);
//   }
#endif
}
