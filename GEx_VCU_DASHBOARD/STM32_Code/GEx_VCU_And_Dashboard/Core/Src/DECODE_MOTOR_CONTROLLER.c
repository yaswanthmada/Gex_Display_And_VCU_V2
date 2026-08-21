/*
 * DECODE_MOTOR_CONTROLLER.c
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */



#include"DECODE_MOTOR_CONTROLLER.h"
Mcu_Msgs_t Mcu_Msgs={0};
static int status_print_task_id=-1;
/*******************************************************************************
 * Function Name : Extract_Raw_signal_Intel
 * Description   : Extracts a raw signal value from the received CAN data using
 *                 Intel (little-endian) bit ordering.
 * Scope         : Static (Private to this file)
 * Parameters    : data - Pointer to the received CAN data bytes
 *                 start - Starting bit position of the signal
 *                 len - Length of the signal in bits
 * Return Value  : Extracted raw signal value as a 64-bit unsigned integer
 ******************************************************************************/
static uint64_t Extract_Raw_signal_Intel(const uint8_t *data, int start, int len)
{
if (len <= 0 || len > 64) return 0;
	uint64_t value = 0;
	for (int i = 0; i < len; i++)
	{
	int bit_index = start + i;
	int byte_index = bit_index / 8;
	int bit_in_byte = bit_index % 8;
	uint8_t bit = 0;
	if (byte_index >= 0 && byte_index < 8) bit = (data[byte_index] >> bit_in_byte) & 1U;
	value |= ((uint64_t)bit << i);
 }
return value;

}

#ifdef MCU_NANJING_QUARK
const char Mcu_Fault_Names[22][MCU_FAULT_NAME_LENGTH] =
{
	    "No Fault",
	    "SW Overcurrent",
	    "Motor Overspeed",
	    "DC Bus Overvolt",
	    "KEY Supply Error",
	    "12V Supply Error",
	    "5V Supply Error",
	    "Angle Wire Error",
	    "HW Overcurrent",
	    "Current Loop Error",
	    "DC Bus Undervolt",
	    "Controller Overtemp",
	    "Motor Overtemp",
	    "Current Sensor Error",
	    "Angle Signal Error",
	    "Throttle Range Error",
	    "Throttle Not Reset",
	    "Motor Stall",
	    "BMS Fault",
	    "Comm Disconnected",
	    "Reserved",
	    "Brake Fault"
};
/*******************************************************************************
 * Function Name : Decode_MCU_0x011
 * Description   : Decodes MCU CAN message 0x011 and updates the gear position,
 *                 gear range, ready state, alarm level, motor speed, trouble
 *                 code, low power mode, AC voltage, and AC current.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x011 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_MCU_0x011(const uint8_t* Data, MCU_Msg_0x011_t* Message)
{
   Message->last_rx_time         = Get_Tick_Ms();
   Message->Gear_Position   = (uint8_t) Extract_Raw_signal_Intel(Data, 0, 2);
   Message->High_Or_Low     = (uint8_t) Extract_Raw_signal_Intel(Data, 2, 2);
   Message->Ready           = (uint8_t) Extract_Raw_signal_Intel(Data, 4, 1);
   Message->Mcu_Alarm_Level = (uint8_t) Extract_Raw_signal_Intel(Data, 5, 3);
   Message->Motor_Speed     = (uint16_t)Extract_Raw_signal_Intel(Data, 8, 16);
   Message->Trouble_Code    = (uint8_t) Extract_Raw_signal_Intel(Data, 24, 8);
   Message->Low_Power_Mode  = (uint8_t) Extract_Raw_signal_Intel(Data, 32, 8);
   Message->Ac_Voltage      = (float)   Extract_Raw_signal_Intel(Data, 40, 12) * 0.1f;
   Message->Ac_Current      = (float)   Extract_Raw_signal_Intel(Data, 52, 12) * 0.1f;
}
/*******************************************************************************
 * Function Name : Decode_MCU_0x012
 * Description   : Decodes MCU CAN message 0x012 and updates the DC voltage,
 *                 DC current, drive motor state, brake pedal state, P gear
 *                 state, accelerator pedal signal voltage, hardwire gear state,
 *                 and accelerator pedal opening.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x012 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_MCU_0x012(const uint8_t* Data, MCU_Msg_0x012_t* Message)
{
	Message->last_rx_time             = Get_Tick_Ms();
    Message->Dc_Voltage               = (float)   Extract_Raw_signal_Intel(Data, 0, 16) * 0.1f;
    Message->Dc_Current               = (float)   Extract_Raw_signal_Intel(Data, 16, 16) * 0.1f;
    Message->Drive_Motor_State        = (uint8_t) Extract_Raw_signal_Intel(Data, 32, 4);
    Message->Brake_Pedal_State        = (uint8_t) Extract_Raw_signal_Intel(Data, 36, 4);
    Message->P_Gear_State             = (uint8_t) Extract_Raw_signal_Intel(Data, 40, 1);
    Message->Reserved_1               = (uint8_t) Extract_Raw_signal_Intel(Data, 41, 1);
    Message->Acc_Pedal_Signal_Voltage = (float)   Extract_Raw_signal_Intel(Data, 42, 9) * 0.01f;
    Message->Hardwire_Gear_State      = (uint8_t) Extract_Raw_signal_Intel(Data, 51, 2);
    Message->Reserved_2               = (uint8_t) Extract_Raw_signal_Intel(Data, 53, 3);
    Message->Acc_Pedal_Opening        = (uint8_t) Extract_Raw_signal_Intel(Data, 56, 8);
}
/*******************************************************************************
 * Function Name : Decode_MCU_0x013
 * Description   : Decodes MCU CAN message 0x013 and updates the motor
 *                 temperature, controller temperature, torque request,
 *                 motor torque, and motor output power.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x013 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_MCU_0x013(const uint8_t* Data, MCU_Msg_0x013_t* Message)
{
	Message->last_rx_time         = Get_Tick_Ms();
    Message->Motor_Temp         = (int16_t) ((int8_t) Extract_Raw_signal_Intel(Data, 0, 8) - 40);
    Message->Controller_Temp    = (int16_t) ((int8_t) Extract_Raw_signal_Intel(Data, 8, 8) - 40);
    Message->Torque_Request     = ((float)  Extract_Raw_signal_Intel(Data, 16, 16) * 0.25f) - 511.0f;
    Message->Motor_Torque       = ((float)  Extract_Raw_signal_Intel(Data, 32, 16) * 0.25f) - 511.0f;
    Message->Motor_Output_Power = (uint16_t)Extract_Raw_signal_Intel(Data, 48, 16);
}
/*******************************************************************************
 * Function Name : Decode_MCU_0x014
 * Description   : Decodes MCU CAN message 0x014 and updates the MCU code,
 *                 hardware version, software version, and small version.
 * Scope         : Static (Private to this file)
 * Parameters    : Data - Pointer to the received CAN data bytes
 *                 Message - Pointer to the 0x014 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Decode_MCU_0x014(const uint8_t* Data, MCU_Msg_0x014_t* Message)
{
	Message->last_rx_time         = Get_Tick_Ms();
    Message->Mcu_Code             = (uint8_t)Extract_Raw_signal_Intel(Data, 0, 8);
    Message->Mcu_Hardware_Version = (uint8_t)Extract_Raw_signal_Intel(Data, 8, 8);
    Message->Mcu_Software_Version = (uint8_t)Extract_Raw_signal_Intel(Data, 16, 8);
    Message->Mcu_Small_Version   = (uint8_t)Extract_Raw_signal_Intel(Data, 32, 8);
}
#endif


#ifdef MCU_NANJING_QUARK
/*******************************************************************************
 * Function Name : Print_MCU_Msg_0x011
 * Description   : Prints the decoded MCU CAN message 0x011 information,
 *                 including reception timing, gear position, ready state,
 *                 alarm level, motor speed, trouble code, low power mode,
 *                 AC voltage, and AC current over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : msg - Pointer to the decoded 0x011 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_MCU_Msg_0x011(const MCU_Msg_0x011_t* msg)
{
	uint32_t Elapsed_Time= Get_Tick_Ms() - msg->last_rx_time;
    Uart_Printf("\r\n=================== MCU MSG 0x011 ===================\r\n");
    Uart_Printf("  Last Rx Time           : %lu ms\r\n", msg->last_rx_time);
    Uart_Printf("  Elapsed Time           : %lu ms\r\n", Elapsed_Time);
    Uart_Printf("  Gear Position          : %u\r\n", msg->Gear_Position);
    Uart_Printf("  High / Low Gear        : %u\r\n", msg->High_Or_Low);
    Uart_Printf("  Ready State            : %u\r\n", msg->Ready);
    Uart_Printf("  MCU Alarm Level        : %u\r\n", msg->Mcu_Alarm_Level);
    Uart_Printf("  Motor Speed            : %u RPM\r\n", msg->Motor_Speed);
    Uart_Printf("  Trouble Code           : 0x%02X\r\n", msg->Trouble_Code);
    Uart_Printf("  Low Power Mode         : %u\r\n", msg->Low_Power_Mode);
    Uart_Printf("  AC Voltage             : %.1f V\r\n", msg->Ac_Voltage);
    Uart_Printf("  AC Current             : %.1f A\r\n", msg->Ac_Current);
    Uart_Printf("=====================================================\r\n");
}
/*******************************************************************************
 * Function Name : Print_MCU_Msg_0x012
 * Description   : Prints the decoded MCU CAN message 0x012 information,
 *                 including reception timing, DC voltage, DC current,
 *                 motor state, brake state, P gear state, accelerator signal,
 *                 hardwire gear state, and accelerator pedal opening over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : msg - Pointer to the decoded 0x012 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_MCU_Msg_0x012(const MCU_Msg_0x012_t* msg)
{
	uint32_t Elapsed_Time= Get_Tick_Ms() - msg->last_rx_time;
    Uart_Printf("\r\n=================== MCU MSG 0x012 ===================\r\n");
    Uart_Printf("  Last Rx Time           : %lu ms\r\n", msg->last_rx_time);
    Uart_Printf("  Elapsed Time           : %lu ms\r\n", Elapsed_Time);
    Uart_Printf("  DC Voltage             : %.1f V\r\n", msg->Dc_Voltage);
    Uart_Printf("  DC Current             : %.1f A\r\n", msg->Dc_Current);
    Uart_Printf("  Drive Motor State      : %u\r\n", msg->Drive_Motor_State);
    Uart_Printf("  Brake Pedal State      : %u\r\n", msg->Brake_Pedal_State);
    Uart_Printf("  P Gear State           : %u\r\n", msg->P_Gear_State);
    Uart_Printf("  Acc Pedal Signal Volt  : %.2f V\r\n", msg->Acc_Pedal_Signal_Voltage);
    Uart_Printf("  Hardwire Gear State    : %u\r\n", msg->Hardwire_Gear_State);
    Uart_Printf("  Acc Pedal Opening      : %u %%\r\n", msg->Acc_Pedal_Opening);
    Uart_Printf("=====================================================\r\n");
}
/*******************************************************************************
 * Function Name : Print_MCU_Msg_0x013
 * Description   : Prints the decoded MCU CAN message 0x013 information,
 *                 including reception timing, motor temperature, controller
 *                 temperature, torque request, motor torque, and motor output
 *                 power over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : msg - Pointer to the decoded 0x013 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_MCU_Msg_0x013(const MCU_Msg_0x013_t* msg)
{
	uint32_t Elapsed_Time= Get_Tick_Ms() - msg->last_rx_time;
    Uart_Printf("\r\n=================== MCU MSG 0x013 ===================\r\n");
    Uart_Printf("  Last Rx Time           : %lu ms\r\n", msg->last_rx_time);
    Uart_Printf("  Elapsed Time           : %lu ms\r\n", Elapsed_Time);
    Uart_Printf("  Motor Temp             : %d degC\r\n", msg->Motor_Temp);
    Uart_Printf("  Controller Temp        : %d degC\r\n", msg->Controller_Temp);
    Uart_Printf("  Torque Request         : %.2f Nm\r\n", msg->Torque_Request);
    Uart_Printf("  Motor Torque           : %.2f Nm\r\n", msg->Motor_Torque);
    Uart_Printf("  Motor Output Power     : %u W\r\n", msg->Motor_Output_Power);
    Uart_Printf("=====================================================\r\n");
}
/*******************************************************************************
 * Function Name : Print_MCU_Msg_0x014
 * Description   : Prints the decoded MCU CAN message 0x014 information,
 *                 including reception timing, MCU code, hardware version,
 *                 software version, and small version over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : msg - Pointer to the decoded 0x014 MCU message structure
 * Return Value  : None
 ******************************************************************************/
static void Print_MCU_Msg_0x014(const MCU_Msg_0x014_t* msg)
{
	uint32_t Elapsed_Time= Get_Tick_Ms() - msg->last_rx_time;
    Uart_Printf("\r\n=================== MCU MSG 0x014 ===================\r\n");
    Uart_Printf("  Last Rx Time           : %lu ms\r\n", msg->last_rx_time);
    Uart_Printf("  Elapsed Time           : %lu ms\r\n", Elapsed_Time);
    Uart_Printf("  MCU Code               : 0x%02X\r\n", msg->Mcu_Code);
    Uart_Printf("  Hardware Version       : v%u\r\n", msg->Mcu_Hardware_Version);
    Uart_Printf("  Software Version       : v%u\r\n", msg->Mcu_Software_Version);
    Uart_Printf("  Small Version          : v%u\r\n", msg->Mcu_Small_Version);
    Uart_Printf("=====================================================\r\n");
}
#endif
/*******************************************************************************
 * Function Name : Print_Motor_Messages
 * Description   : Prints all decoded Motor Controller CAN messages over UART
 *                 when Motor Controller communication is available. Reports
 *                 a communication status message when Motor Controller
 *                 messages are not received.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Print_Motor_Messages(void)
{
#ifdef MCU_NANJING_QUARK
    if (Mcu_Msgs.mcu_comm_ok)
    {
        Print_MCU_Msg_0x011(&Mcu_Msgs.MCU_MSG_0x011);
        Print_MCU_Msg_0x012(&Mcu_Msgs.MCU_MSG_0x012);
        Print_MCU_Msg_0x013(&Mcu_Msgs.MCU_MSG_0x013);
        Print_MCU_Msg_0x014(&Mcu_Msgs.MCU_MSG_0x014);
    }
    else
    {
    	Uart_Printf("[INFO] :Motor Messages Not Received \n");
    }
#endif
}
void Process_Motor_Controller_Messages(CAN_Message_t *Frames)
{
	switch(Frames->id)
	{
#ifdef MCU_NANJING_QUARK
	case MCU_MSG_0x011_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x011(Frames->data,&Mcu_Msgs.MCU_MSG_0x011);
		break;
	case MCU_MSG_0x012_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x012(Frames->data,&Mcu_Msgs.MCU_MSG_0x012);
		break;
	case MCU_MSG_0x013_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x013(Frames->data,&Mcu_Msgs.MCU_MSG_0x013);
		break;
	case MCU_MSG_0x014_ID:
		Mcu_Msgs.mcu_comm_ok=true;
		Decode_MCU_0x014(Frames->data,&Mcu_Msgs.MCU_MSG_0x014);
		break;
#endif
	}
}
/*******************************************************************************
 * Function Name : Task_Id_For_Mcu_Print
 * Description   : Registers the periodic Motor Controller message printing
 *                 task when it has not already been registered and returns
 *                 the task registration status.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : true if the Motor Controller print task is registered
 *                 successfully; false if task registration fails.
 ******************************************************************************/
static bool Task_Id_For_Mcu_Print(void)
{
    if (status_print_task_id < 0)
    {
        status_print_task_id = Task_Timer_Register(3000, Print_Motor_Messages);
        if (status_print_task_id < 0)
        {
        	return false;
        }
    }
    return true;
}
/*******************************************************************************
 * Function Name : Enable_Mcu_Print_Task
 * Description   : Enables the periodic Motor Controller message printing task
 *                 by registering the Motor Controller print task when required.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : true if the Motor Controller print task is registered
 *                 successfully; false if task registration fails.
 ******************************************************************************/
bool Enable_Mcu_Print_Task()
{
	return Task_Id_For_Mcu_Print();
}
void Get_Mcu_Data(MCU_Data_t *Mcu_Data)
{
#ifdef MCU_NANJING_QUARK

    Mcu_Data->Speed = Mcu_Msgs.MCU_MSG_0x011.Motor_Speed;
    Mcu_Data->Mcu_Pack_voltage = Mcu_Msgs.MCU_MSG_0x012.Dc_Voltage;
    Mcu_Data->Mcu_Current = Mcu_Msgs.MCU_MSG_0x012.Dc_Current;
    Mcu_Data->Motor_Temp = Mcu_Msgs.MCU_MSG_0x013.Motor_Temp;
    Mcu_Data->Mcu_Pcb_Temp = Mcu_Msgs.MCU_MSG_0x013.Controller_Temp;
    uint32_t trouble_mask = Mcu_Msgs.MCU_MSG_0x011.Trouble_Code;
    if (trouble_mask == 0)
        {
            Mcu_Data->Mcu_Fault = false;
            Mcu_Data->Mcu_Fault_Count = 0;
            for (uint8_t i = 0; i < 22; i++)
            {
                Mcu_Data->Mcu_Active_Fault[i][0] = '\0';
            }
        }
        else
        {
            Mcu_Data->Mcu_Fault = true;
            Mcu_Data->Mcu_Fault_Count = 0;

            for (uint8_t i = 0; i < 22; i++)
            {
                Mcu_Data->Mcu_Active_Fault[i][0] = '\0';
            }
            uint16_t max_fault_names = sizeof(Mcu_Fault_Names) / sizeof(Mcu_Fault_Names[0]);
            for (uint8_t bit = 0; bit < 32 && bit < 22 && bit < max_fault_names; bit++)
            {
                if (trouble_mask & (1UL << bit))
                {
                    if (Mcu_Data->Mcu_Fault_Count < MCU_FAULT_COUNT)
                    {
                        uint8_t dest_size = sizeof(Mcu_Data->Mcu_Active_Fault[0]);
                        strncpy(Mcu_Data->Mcu_Active_Fault[Mcu_Data->Mcu_Fault_Count], Mcu_Fault_Names[bit], dest_size - 1);
                        Mcu_Data->Mcu_Active_Fault[Mcu_Data->Mcu_Fault_Count][dest_size - 1] = '\0';
                        Mcu_Data->Mcu_Fault_Count++;
                    }
                }
            }
        }
    if(Mcu_Msgs.MCU_MSG_0x012.Brake_Pedal_State>0)
    {
        Mcu_Data->Brake_Signal=true;
    }
    else
    {
        Mcu_Data->Brake_Signal=false;
    }
    Uart_Printf("mcu in end\n\r");
#endif
}
