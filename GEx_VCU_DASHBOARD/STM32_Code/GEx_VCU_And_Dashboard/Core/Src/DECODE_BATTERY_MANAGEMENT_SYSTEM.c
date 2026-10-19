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
const char Bms_Fault_Names[16][40] =
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
    Message->CELL14 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL15 = (float)((Data[4] << 8) | Data[5])/1000.f;
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
    Message->CELL17 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL18 = (float)((Data[4] << 8) | Data[5])/1000.f;
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
static void Decode_CAN_0x10D(const uint8_t *Data,JBD_BMS_0x10D_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL19 = (float)((Data[0] << 8) | Data[1])/1000.f;
    Message->CELL20 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL21 = (float)((Data[4] << 8) | Data[5])/1000.f;
}/*******************************************************************************
 * Function Name : Decode_CAN_0x10C
 * Description   : Decodes BMS CAN message 0x10C and updates the voltage value
 *                 of battery cell 16 along with the message reception
 *                 timestamps.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x10C BMS message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_CAN_0x10E(const uint8_t *Data,JBD_BMS_0x10E_t *Message)
{
	Message->Last_Received_Time= Message->Current_Received_Time;
    Message->Current_Received_Time=HAL_GetTick();
    Message->CELL22 = (float)((Data[0] << 8) | Data[1])/1000.f;
    Message->CELL23 = (float)((Data[2] << 8) | Data[3])/1000.f;
    Message->CELL24 = (float)((Data[4] << 8) | Data[5])/1000.f;
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
	Uart_Printf("------------ID=0X100-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("Voltage :%f\n", Message->Total_Voltage);
	    Uart_Printf("Current :%f\n", Message->Charge_and_Discharge_Current);
	    Uart_Printf("RSOC :%f\n", Message->Remaining_Capacity);
	    Uart_Printf("------------------------------------\n");
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
	Uart_Printf("------------ID=0X105-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("NTC1 :%.2f\n", Message->NTC1);
	    Uart_Printf("NTC2 :%.2f\n", Message->NTC2);
	    Uart_Printf("NTC3 :%.2f\n", Message->NTC3);
	    Uart_Printf("------------------------------------\n");
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
	Uart_Printf("------------ID=0X106-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("NTC4 :%.2f\n", Message->NTC4);
	    Uart_Printf("NTC5 :%.2f\n", Message->NTC5);
	    Uart_Printf("NTC6 :%.2f\n", Message->NTC6);
	    Uart_Printf("------------------------------------\n");

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
	Uart_Printf("\n------------ID=0X107-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("CELL1 :%.2f\n", Message->CELL1);
	    Uart_Printf("CELL2 :%.2f\n", Message->CELL2);
	    Uart_Printf("CELL3 :%.2f\n", Message->CELL3);
	    Uart_Printf("------------------------------------\n");
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
	Uart_Printf("\n------------ID=0X108-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("CELL4 :%.2f\n", Message->CELL4);
	    Uart_Printf("CELL5 :%.2f\n", Message->CELL5);
	    Uart_Printf("CELL6 :%.2f\n", Message->CELL6);
	    Uart_Printf("------------------------------------\n");
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
	Uart_Printf("------------ID=0X109-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("CELL7 :%.2f\n", Message->CELL7);
	    Uart_Printf("CELL8 :%.2f\n", Message->CELL8);
	    Uart_Printf("CELL9 :%.2f\n", Message->CELL9);
	    Uart_Printf("------------------------------------\n");

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
	Uart_Printf("\n------------ID=0X10A-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("CELL10 :%.2f\n", Message->CELL10);
	    Uart_Printf("CELL11 :%.2f\n", Message->CELL11);
	    Uart_Printf("CELL12 :%.2f\n", Message->CELL12);
	    Uart_Printf("------------------------------------\n");

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
	Uart_Printf("\n------------ID=0X10B-------------\n");
	    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
	    Uart_Printf("CELL13 :%.2f\n", Message->CELL13);
	    Uart_Printf("CELL14 :%.2f\n", Message->CELL14);
	    Uart_Printf("CELL15 :%.2f\n", Message->CELL15);
	    Uart_Printf("------------------------------------\n");

	}
/*******************************************************************************
- Function Name : Print_Jbd_Bms_0x10C_t
- Description   : Prints the decoded BMS CAN 0x10C message information,
-                 including reception timestamps and cell 16 to cell 18
-                 voltage values, over UART using separate print calls.
- Scope         : Static (Private to this file)
- Parameters    : Message - Pointer to the decoded 0x10C BMS message structure
- Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x10C_t(JBD_BMS_0x10C_t *Message)
{
    Uart_Printf("\n------------ID=0X10C-------------\n");
    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
    Uart_Printf("CELL16 :%.2f\n", Message->CELL16);
    Uart_Printf("CELL17 :%.2f\n", Message->CELL17);
    Uart_Printf("CELL18 :%.2f\n", Message->CELL18);
    Uart_Printf("------------------------------------\n");
}

/*******************************************************************************
- Function Name : Print_Jbd_Bms_0x10D_t
- Description   : Prints the decoded BMS CAN 0x10D message information,
-                 including reception timestamps and cell 19 to cell 21
-                 voltage values, over UART using separate print calls.
- Scope         : Static (Private to this file)
- Parameters    : Message - Pointer to the decoded 0x10D BMS message structure
- Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x10D_t(JBD_BMS_0x10D_t *Message)
{
    Uart_Printf("\n------------ID=0X10D-------------\n");
    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
    Uart_Printf("CELL19 :%.2f\n", Message->CELL19);
    Uart_Printf("CELL20 :%.2f\n", Message->CELL20);
    Uart_Printf("CELL21 :%.2f\n", Message->CELL21);
    Uart_Printf("------------------------------------\n");
}

/*******************************************************************************
- Function Name : Print_Jbd_Bms_0x10E_t
- Description   : Prints the decoded BMS CAN 0x10E message information,
-                 including reception timestamps and cell 22 to cell 24
-                 voltage values, over UART using separate print calls.
- Scope         : Static (Private to this file)
- Parameters    : Message - Pointer to the decoded 0x10E BMS message structure
- Return Value  : None
 ******************************************************************************/
static void Print_Jbd_Bms_0x10E_t(JBD_BMS_0x10E_t *Message)
{
    Uart_Printf("\n------------ID=0X10E-------------\n");
    Uart_Printf("Last_RX :%d  Current_Rx :%d \n", Message->Last_Received_Time, Message->Current_Received_Time);
    Uart_Printf("CELL22 :%.2f\n", Message->CELL22);
    Uart_Printf("CELL23 :%.2f\n", Message->CELL23);
    Uart_Printf("CELL24 :%.2f\n", Message->CELL24);
    Uart_Printf("------------------------------------\n");
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
		Print_Jbd_Bms_0x10D_t(&Bms_Frames.JBD_BMS_0x10D);
		Print_Jbd_Bms_0x10E_t(&Bms_Frames.JBD_BMS_0x10E);

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
	 case JBD_BMS_ID13:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x10D(frame->data,&Bms_Frames.JBD_BMS_0x10D);
	     return true;
	 case JBD_BMS_ID14:
		 Bms_Frames.bms_comm_ok=true;
		 Decode_CAN_0x10E(frame->data,&Bms_Frames.JBD_BMS_0x10E);
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
        status_print_task_id = Task_Timer_Register(000, Print_Bms_Messages);
        if (status_print_task_id < 0)
        {
        	return false;
        }
    }
    return true;
}
/* ======================================================================
 * range_soc_simple.c
 * Simple EV range from SoC only.
 *
 * Input : SoC (0.0 to 100.0 %) and a flag saying if the SoC is valid.
 * Output: range in km.
 *
 *   battery total energy (Wh) = Ah x Volts
 *   energy left (Wh)          = total x SoC - reserve
 *   range (km)                = energy left / WH_PER_KM
 *
 * It is an estimate. It does not change with driving style, load or weather.
 * ====================================================================== */

/* Total energy of the battery in Wh.  100 Ah x 1 = 100 Ah ; 3.2 V x 23 = 73.6 V ;
 * 100 Ah x 73.6 V = 7360 Wh                                                      */
static float BatteryTotal_Wh(void)
{
    float total_ah = CELL_CAPACITY_AH * (float)CELLS_IN_PARALLEL;
    float volts    = CELL_VOLTAGE_V   * (float)CELLS_IN_SERIES;
    return total_ah * volts;
}

/* Energy left in Wh, after taking away the reserve */
float BatteryLeft_Wh(float soc_percent)
{
    float total   = BatteryTotal_Wh();
    float reserve = total * RESERVE_PERCENT / 100.0f;
    float left    = (total * soc_percent / 100.0f) - reserve;

    return (left > 0.0f) ? left : 0.0f;
}

/* Range in km as a float, for example 70.6 */
float Range_FromSoc_km(float soc_percent, uint8_t soc_is_valid)
{
    float wh_per_km = WH_PER_KM;

    if ((soc_is_valid == 0u) || (soc_percent < 0.0f) || (soc_percent > 100.0f)) {
        return 0.0f;                        /* show dashes in this case */
    }
    if (wh_per_km < MIN_WH_PER_KM) {
        wh_per_km = MIN_WH_PER_KM;
    }
    return BatteryLeft_Wh(soc_percent) / wh_per_km;
}

/* Range for the display in 0.1 km units: 706 means 70.6 km. Rounded DOWN. */
uint16_t Range_FromSoc_x10(float soc_percent, uint8_t soc_is_valid)
{
    float r = Range_FromSoc_km(soc_percent, soc_is_valid) * 10.0f;

    if (r > 9999.0f) {
        r = 9999.0f;
    }
    return (uint16_t)r;                     /* cutting the decimals rounds down */
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
    if (Bms_Data == NULL)
    {
        return;
    }
    Bms_Data->Range=0;
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


   Bms_Data->Cells[15]=Bms_Frames.JBD_BMS_0x10C.CELL16;
   Bms_Data->Cells[16]=Bms_Frames.JBD_BMS_0x10C.CELL17;
   Bms_Data->Cells[17]=Bms_Frames.JBD_BMS_0x10C.CELL18;
   Bms_Data->Cells[18]=Bms_Frames.JBD_BMS_0x10D.CELL19;
   Bms_Data->Cells[19]=Bms_Frames.JBD_BMS_0x10D.CELL20;
   Bms_Data->Cells[20]=Bms_Frames.JBD_BMS_0x10D.CELL21;
   Bms_Data->Cells[21]=Bms_Frames.JBD_BMS_0x10E.CELL22;
   Bms_Data->Cells[22]=Bms_Frames.JBD_BMS_0x10E.CELL23;
   Bms_Data->Cells[23]=Bms_Frames.JBD_BMS_0x10E.CELL24;




   uint8_t fault_count = 0;
   if(Bms_Frames.JBD_BMS_0x102.Protection_Status>0)
   {
	      fault_count = 0;
	      for (uint8_t i = 0; i < BMS_FAULT_COUNT_JBD_BMS; i++)
	      {
	          Bms_Data->Bms_Active_Fault[i][0] = '\0';
	      }
	      if (Bms_Frames.JBD_BMS_0x102.Protection_Status > 0)
	      {
	    	  uint16_t max_fault_names = sizeof(Bms_Fault_Names) / sizeof(Bms_Fault_Names[0]);

	          for (uint8_t i = 0; i < 16 && i < BMS_FAULT_COUNT_JBD_BMS && i < max_fault_names; i++)
	          {
	              if (Bms_Frames.JBD_BMS_0x102.Protection_Status & (1U << i))
	              {
	                  if (fault_count < BMS_FAULT_COUNT_JBD_BMS)
	                  {
	                      uint8_t dest_size = sizeof(Bms_Data->Bms_Active_Fault[0]);
	                      memcpy(Bms_Data->Bms_Active_Fault[fault_count],
	                             Bms_Fault_Names[i],
	                             sizeof(Bms_Data->Bms_Active_Fault[0]));

	                      Bms_Data->Bms_Active_Fault[fault_count][dest_size - 1] = '\0';

	                      fault_count++;
	                  }
	              }
	          }
	      }
	      Bms_Data->Bms_Fault_Count = fault_count;
	      Bms_Data->Bms_Fault = (fault_count > 0);
   }
#endif
}
