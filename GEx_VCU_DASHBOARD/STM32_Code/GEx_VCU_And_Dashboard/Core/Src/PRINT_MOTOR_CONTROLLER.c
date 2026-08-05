/*
 * PRINT_MOTOR_CONTROLLER.c
 *
 *  Created on: Aug 5, 2026
 *      Author: MadaYaswanth
 */

#include"CONFIGURATION_FILES.h"
#include"PROCESS_MOTOR_CONTROLLER.h"
extern void Uart_Printf(const char *format, ...);
#ifdef MCU_NANJING_QUARK
extern Mcu_Msgs_t Mcu_Msgs;

void Print_MCU_Msg_0x011(const MCU_Msg_0x011_t* msg)
{
	uint32_t Elapsed_Time= HAL_GetTick() - msg->last_rx_time;
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

void Print_MCU_Msg_0x012(const MCU_Msg_0x012_t* msg)
{
	uint32_t Elapsed_Time= HAL_GetTick() - msg->last_rx_time;
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

void Print_MCU_Msg_0x013(const MCU_Msg_0x013_t* msg)
{
	uint32_t Elapsed_Time= HAL_GetTick() - msg->last_rx_time;
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

void Print_MCU_Msg_0x014(const MCU_Msg_0x014_t* msg)
{
	uint32_t Elapsed_Time= HAL_GetTick() - msg->last_rx_time;
    Uart_Printf("\r\n=================== MCU MSG 0x014 ===================\r\n");
    Uart_Printf("  Last Rx Time           : %lu ms\r\n", msg->last_rx_time);
    Uart_Printf("  Elapsed Time           : %lu ms\r\n", Elapsed_Time);
    Uart_Printf("  MCU Code               : 0x%02X\r\n", msg->Mcu_Code);
    Uart_Printf("  Hardware Version       : v%u\r\n", msg->Mcu_Hardware_Version);
    Uart_Printf("  Software Version       : v%u\r\n", msg->Mcu_Software_Version);
    Uart_Printf("  Small Version          : v%u\r\n", msg->Mcuc_Small_Version);
    Uart_Printf("=====================================================\r\n");
}
#endif
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
