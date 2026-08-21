/*
 * DWIN_DISPLAY.h
 *
 *  Created on: Aug 10, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_DWIN_DISPLAY_H_
#define INC_DWIN_DISPLAY_H_

#include"DWIN_DEFINES.h"
#include"UART.h"
#include"main.h"
#include<string.h>
#include"TASK_TIMER.h"
typedef struct
{
uint8_t Soc;
float Battery_Pack_voltage;
float Battery_Current;
float Cells[24];
uint8_t Avg_Temp;
uint8_t Bms_Fault;
uint8_t Bms_Fault_Count;
char Bms_Active_Fault[16][BMS_FAULT_NAMES_LENGTH];
}BMS_Data_t;
typedef struct
{
	float Mcu_Pack_voltage;
	float Mcu_Current;
	uint8_t Motor_Temp;
	uint8_t Mcu_Pcb_Temp;
	uint8_t Mcu_Fault;
	uint32_t Odo_Meter;
	uint32_t Trip_Value;
	uint8_t Mcu_Fault_Count;
	uint8_t Speed;
	uint8_t Vehicle_Mode;
	char Mcu_Active_Fault[22][MCU_FAULT_NAMES_LENGTH];
	bool Brake_Signal;
	bool Forward;
	bool Reverse;
	bool Neutral;
	bool Echo_Boost;

}MCU_Data_t;
typedef struct
{
	bool Battery_Low_12V;
    float  Voltage_12v;
}ADC_Data_t;
typedef struct
{
bool Hand_Brake;
bool Head_Light;
bool Charge_Ack;
bool Brake_Fluid;
bool Right_Ind;
bool Left_Ind;
bool Mppt_On_Off;
}IO_Data_t;
typedef struct
{
bool Is_Watch_Dog_Reset;
bool Is_System_Hard_Fault;
bool Is_Bms_Can_Ok;
bool Is_Mcu_Can_Ok;
bool Is_Can_Ok;
}System_Faults_t;
typedef struct
{
    BMS_Data_t Bms_Data;
    MCU_Data_t Mcu_Data;
    ADC_Data_t Adc_Data;
    IO_Data_t  IO_Data;
    System_Faults_t System_Data;
    bool Is_Ready;
}GEx_Display_t;
/*******************************************************************************
 * Function Name : Display_Update_All
 * Description   : High-level routine that first refreshes display data structures
 *                 and then pushes the values to the DWIN display screen.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/

void Display_Update_All();
/*******************************************************************************
 * Function Name : Get_Bms_Data
 * Description   : Collects the latest decoded BMS parameters and copies them
 *                 into the application-level BMS data structure, including
 *                 battery temperature, current, pack voltage, state of charge,
 *                 cell voltages, and active BMS fault information.
 * Scope         : Global
 * Parameters    : Bms_Data - Pointer to the application BMS data structure
 * Return Value  : None
 ******************************************************************************/
void Get_Bms_Data(BMS_Data_t*);
/*******************************************************************************
 * Function Name : Get_Mcu_Data
 * Description   : Collects the latest decoded Motor Controller parameters and
 *                 copies them into the application-level MCU data structure,
 *                 including motor speed, pack voltage, current, motor and
 *                 controller temperatures, fault status, active faults, and
 *                 brake signal.
 * Scope         : Global
 * Parameters    : Mcu_Data - Pointer to the application MCU data structure
 * Return Value  : None
 ******************************************************************************/

void Get_Mcu_Data(MCU_Data_t*);
/*******************************************************************************
 * Function Name : Get_IO_Data
 * Description   : Retrieves the latest GPIO input status and copies the
 *                 vehicle input states into the IO data structure.
 * Scope         : Public / Interface API
 * Parameters    : IO_Data - Pointer to the IO data structure to be updated
 * Return Value  : None
 ******************************************************************************/
void Get_IO_Data(IO_Data_t*);

/*******************************************************************************
 * Function Name : Get_Adc_Data
 * Description   : Populates an external data structure with the latest calculated
 *                 12V battery voltage.
 * Scope         : Public / Driver API
 * Parameters    : Adc_Data - Pointer to the ADC_Data_t structure to populate
 * Return Value  : None
 * ***************************************************************************/
void Get_Adc_Data(ADC_Data_t*);


void Get_System_Data(System_Faults_t*);
/*******************************************************************************
 * Function Name : Enable_Display_Print_Task
 * Description   : Public wrapper function to initialize and enable the periodic
 *                 display print task.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : bool (true = Success, false = Failure)
 ******************************************************************************/
bool Enable_Display_Print_Task();

#endif /* INC_DWIN_DISPLAY_H_ */
