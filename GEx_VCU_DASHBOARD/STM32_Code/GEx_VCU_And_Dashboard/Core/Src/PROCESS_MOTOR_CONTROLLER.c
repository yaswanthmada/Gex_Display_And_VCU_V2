/*
 * PROCESS_MOTOR_CONTROLLER.C
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */


#include"PROCESS_MOTOR_CONTROLLER.h"
#include"CONFIGURATION_FILES.h"


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
void Decode_MCU_0x011(const uint8_t* Data, MCU_Msg_0x011_t* Message)
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

void Decode_MCU_0x012(const uint8_t* Data, MCU_Msg_0x012_t* Message)
{
	Message->last_rx_time         = Get_Tick_Ms();
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

void Decode_MCU_0x013(const uint8_t* Data, MCU_Msg_0x013_t* Message)
{
	Message->last_rx_time         = Get_Tick_Ms();
    Message->Motor_Temp         = (int16_t) ((int8_t) Extract_Raw_signal_Intel(Data, 0, 8) - 40);
    Message->Controller_Temp    = (int16_t) ((int8_t) Extract_Raw_signal_Intel(Data, 8, 8) - 40);
    Message->Torque_Request     = ((float)  Extract_Raw_signal_Intel(Data, 16, 16) * 0.25f) - 511.0f;
    Message->Motor_Torque       = ((float)  Extract_Raw_signal_Intel(Data, 32, 16) * 0.25f) - 511.0f;
    Message->Motor_Output_Power = (uint16_t)Extract_Raw_signal_Intel(Data, 48, 16);
}

void Decode_MCU_0x014(const uint8_t* Data, MCU_Msg_0x014_t* Message)
{
	Message->last_rx_time         = Get_Tick_Ms();
    Message->Mcu_Code             = (uint8_t)Extract_Raw_signal_Intel(Data, 0, 8);
    Message->Mcu_Hardware_Version = (uint8_t)Extract_Raw_signal_Intel(Data, 8, 8);
    Message->Mcu_Software_Version = (uint8_t)Extract_Raw_signal_Intel(Data, 16, 8);
    Message->Mcu_Small_Version   = (uint8_t)Extract_Raw_signal_Intel(Data, 32, 8);
}
#endif
