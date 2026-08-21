/*
 * DECODE_BATTERY_MANAGEMENT_SYSTEM.h
 *
 *  Created on: Aug 10, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_DECODE_BATTERY_MANAGEMENT_SYSTEM_H_
#define INC_DECODE_BATTERY_MANAGEMENT_SYSTEM_H_

#include"CONFIGURATION_FILES.h"
#include"MCP2515.h"
#include"SYSTICK.h"
#include"TASK_TIMER.h"
#include"UART.h"
#include"DWIN_DISPLAY.h"
#include<string.h>
#ifdef JBD_BMS

#define JBD_BMS_ID0 0x100
#define JBD_BMS_ID1 0x101
#define JBD_BMS_ID2 0x102
#define JBD_BMS_ID3 0x103
#define JBD_BMS_ID4 0x104
#define JBD_BMS_ID5 0x105
#define JBD_BMS_ID6 0x106
#define JBD_BMS_ID7 0x107
#define JBD_BMS_ID8 0x108
#define JBD_BMS_ID9 0x109
#define JBD_BMS_ID10 0x10A
#define JBD_BMS_ID11 0x10B
#define JBD_BMS_ID12 0x10C
#define JBD_BMS_ID13 0x10D
#define JBD_BMS_ID14 0x10E
#define JBD_BMS_ID15 0x10F
#define JBD_BMS_ID16 0x110

#define BMS_FAULT_NAME_LEN    20
#define BMS_FAULT_COUNT_JBD_BMS      16


typedef struct
{
	float Total_Voltage;
	float Charge_and_Discharge_Current;
	float Remaining_Capacity;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x100_t;

typedef struct{
	float Full_Capacity;
	uint16_t  No_Cycles;
	uint16_t RSOC;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x101_t;

typedef struct{
	bool Equlization_Low_byte[33];
	uint16_t Protection_Status;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x102_t;


typedef struct{
	uint16_t FET_Control_Status;
	short int Production_Date;
	uint16_t Software_Version;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x103_t;


typedef struct{
	uint8_t Number_Of_Battery_Strings;
	uint8_t NTC_Number_Probes;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x104_t;


typedef struct{
	float NTC1;
	float NTC2;
	float NTC3;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x105_t;


typedef struct{
	float NTC4;
	float NTC5;
	float NTC6;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x106_t;


typedef struct{
	float CELL1;
	float  CELL2;
	float CELL3;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x107_t;

typedef struct{
	float CELL4;
	float  CELL5;
	float CELL6;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x108_t;

typedef struct{
	float CELL7;
	float CELL8;
	float CELL9;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x109_t;

typedef struct{
	float CELL10;
	float  CELL11;
	float CELL12;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x10A_t;

typedef struct{
	float CELL13;
	float  CELL14;
	float CELL15;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x10B_t;

typedef struct{
	float CELL16;
	float  CELL17;
	float CELL18;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x10C_t;

typedef struct{
	float CELL19;
	float  CELL20;
	float CELL21;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x10D_t;

typedef struct{
	float CELL22;
	float  CELL23;
	float CELL24;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x10E_t;

typedef struct{
	float CELL25;
	float  CELL26;
	float CELL27;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x10F_t;

typedef struct{
	float CELL28;
	float  CELL29;
	float CELL30;
    uint32_t Current_Received_Time;
    uint32_t Last_Received_Time;
}JBD_BMS_0x110_t;

#endif

typedef struct{
#ifdef JBD_BMS
	JBD_BMS_0x100_t JBD_BMS_0x100;
	JBD_BMS_0x101_t JBD_BMS_0x101;
	JBD_BMS_0x102_t JBD_BMS_0x102;
	JBD_BMS_0x103_t JBD_BMS_0x103;
	JBD_BMS_0x104_t JBD_BMS_0x104;
	JBD_BMS_0x105_t JBD_BMS_0x105;
	JBD_BMS_0x106_t JBD_BMS_0x106;
	JBD_BMS_0x107_t JBD_BMS_0x107;
	JBD_BMS_0x108_t JBD_BMS_0x108;
	JBD_BMS_0x109_t JBD_BMS_0x109;
	JBD_BMS_0x10A_t JBD_BMS_0x10A;
	JBD_BMS_0x10B_t JBD_BMS_0x10B;
	JBD_BMS_0x10C_t JBD_BMS_0x10C;
	JBD_BMS_0x10D_t JBD_BMS_0x10D;
	JBD_BMS_0x10E_t JBD_BMS_0x10E;
	JBD_BMS_0x10F_t JBD_BMS_0x10F;
	JBD_BMS_0x110_t JBD_BMS_0x110;
#endif
    /* Communication status */
    bool bms_comm_ok;
}BMS_MSG_t;

bool Process_Bms_Messages(CAN_Message_t *);
bool Enable_Bms_Print_Task();

#endif /* INC_DECODE_BATTERY_MANAGEMENT_SYSTEM_H_ */
