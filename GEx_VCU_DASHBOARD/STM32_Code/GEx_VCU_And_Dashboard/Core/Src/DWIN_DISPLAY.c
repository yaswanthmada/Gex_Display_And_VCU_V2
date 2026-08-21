/*
 * DWIN_DISPLAY.c
 *
 *  Created on: Aug 10, 2026
 *      Author: MadaYaswanth
 */


#include"DWIN_DISPLAY.h"
static int status_print_task_id=-1;
GEx_Display_t GEx_Display={0} ;
// static int16_t page_offset  = 0; Now not using , required when want to change from one screen to another
/*******************************************************************************
 * Function Name : Send_Cmd
 * Description   : Constructs a DWIN display command frame with headers, length,
 *                 command byte, target memory address, and payload data.
 * Scope         : Static (Private to this file)
 * Parameters    : cmd      - DWIN command identifier
 *                 address  - Target variable pointer (VP) address
 *                 data     - Pointer to payload data bytes
 *                 data_len - Length of payload data in bytes
 * Return Value  : None
 ******************************************************************************/
static void Send_Cmd(uint8_t cmd, uint16_t address,
                     const uint8_t *data, uint8_t data_len)
{
    uint8_t frame[20];
    uint8_t idx = 0;
    frame[idx++] = DWIN_HEADER_1;
    frame[idx++] = DWIN_HEADER_2;
    frame[idx++] = (uint8_t)(1u + 2u + data_len);   /* LEN */
    frame[idx++] = cmd;
    frame[idx++] = (uint8_t)(address >> 8);
    frame[idx++] = (uint8_t)(address & 0xFFu);
    for (uint8_t i = 0; i < data_len; i++)
    {
        frame[idx++] = data[i];
    }
    Send_On_Display_Uart(frame, idx);
}
/*******************************************************************************
 * Function Name : DWIN_Write_VP
 * Description   : Writes a 16-bit integer value to a specified Variable Pointer (VP)
 *                 address on the DWIN display.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Target Variable Pointer address
 *                 value   - 16-bit unsigned integer value to write
 * Return Value  : None
 ******************************************************************************/
static void DWIN_Write_VP(uint16_t address, uint16_t value)
{
    uint8_t d[2] = { (uint8_t)(value >> 8), (uint8_t)(value & 0xFFu) };
    Send_Cmd(CMD_WRITE_VP, address, d, 2u);
}
static void DWIN_Write_Bool(uint16_t address,
                            bool value)
{
    DWIN_Write_VP(address,
                  value ? 1u : 0u);
}
/*******************************************************************************
 * Function Name : DWIN_Write_VP32
 * Description   : Writes a 32-bit integer value to a specified Variable Pointer (VP)
 *                 address on the DWIN display.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Target Variable Pointer address
 *                 value   - 32-bit unsigned integer value to write
 * Return Value  : None
 ******************************************************************************/
static void DWIN_Write_VP32(uint16_t address, uint32_t value)
{
    uint8_t d[4] = {
        (uint8_t)((value >> 24) & 0xFFu),
        (uint8_t)((value >> 16) & 0xFFu),
        (uint8_t)((value >>  8) & 0xFFu),
        (uint8_t)( value        & 0xFFu)
    };
    Send_Cmd(CMD_WRITE_VP, address, d, 4u);
}
/*******************************************************************************
 * Function Name : DWIN_Write_VPFloat
 * Description   : Writes a single-precision floating-point value to a specified
 *                 Variable Pointer (VP) address by casting it via a union.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Target Variable Pointer address
 *                 value   - Float value to write
 * Return Value  : None
 ******************************************************************************/
static void DWIN_Write_VPFloat(uint16_t address, float value)
{
    union { float f; uint32_t u; } v;
    v.f = value;
    DWIN_Write_VP32(address, v.u);
}
/*******************************************************************************
 * Function Name : Update_Dwin_Display
 * Description   : Synchronizes all vehicle telemetry, sensor inputs, and state
 *                 variables with their corresponding DWIN UI variable pointer addresses.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/

static void Update_Dwin_Display(void)
{

    DWIN_Write_Bool(
        HEAD_LIGHTS,
        GEx_Display.IO_Data.Head_Light);


    DWIN_Write_Bool(
        LEFT_INDICATOR,
        GEx_Display.IO_Data.Left_Ind);


    DWIN_Write_Bool(
        PEDDAL_BRAKE,
        GEx_Display.Mcu_Data.Brake_Signal);


    DWIN_Write_Bool(
        HAND_BRAKE,
        GEx_Display.IO_Data.Hand_Brake);


    DWIN_Write_Bool(
        BRAKE_FLUID,
        GEx_Display.IO_Data.Brake_Fluid);


    DWIN_Write_Bool(
        BATTERY_FAULT_12V,
        GEx_Display.Adc_Data.Battery_Low_12V);


    DWIN_Write_Bool(
        CHARGER_DETECTION,
        GEx_Display.IO_Data.Charge_Ack);


    DWIN_Write_Bool(
        RIGHT_INDICATOR,
        GEx_Display.IO_Data.Right_Ind);


    DWIN_Write_Bool(
        DRIVE_MODE,
        GEx_Display.Mcu_Data.Forward);


    DWIN_Write_Bool(
        REVERSE_MODE,
        GEx_Display.Mcu_Data.Reverse);


    DWIN_Write_Bool(
        NEUTRAL_MODE,
        GEx_Display.Mcu_Data.Neutral);


    DWIN_Write_Bool(
        ECHO_BOOST,
        GEx_Display.Mcu_Data.Echo_Boost);

    DWIN_Write_Bool(
    		READY,
        GEx_Display.Is_Ready);


    DWIN_Write_VP(
        VEHICLE_SPEED,
        GEx_Display.Mcu_Data.Speed);
    if(GEx_Display.Mcu_Data.Speed==0)
    {
        DWIN_Write_Bool(
        		CAR_ROAD,
            0);
    }
    else
    {
    	static bool state=true;
        DWIN_Write_Bool(
        		CAR_ROAD,
            state);
        state=!state;
    }

    DWIN_Write_VP(
        SOC,
        GEx_Display.Bms_Data.Soc);

    DWIN_Write_VP32(
        ODO,
        GEx_Display.Mcu_Data.Odo_Meter);
    DWIN_Write_VP32(
        TRIP,
        GEx_Display.Mcu_Data.Trip_Value);

    DWIN_Write_VP(
        MCU_TEMP,
        GEx_Display.Mcu_Data.Mcu_Pcb_Temp);


    DWIN_Write_VP(
        MOTOR_TEMP,
        GEx_Display.Mcu_Data.Motor_Temp);



    DWIN_Write_VP(
        BAT_PACK_TEMP,
        GEx_Display.Bms_Data.Avg_Temp);

    DWIN_Write_VPFloat(
        BATTERY_PACK_VOL,
        GEx_Display.Bms_Data.Battery_Pack_voltage);


    DWIN_Write_VPFloat(
        BATTERY_PACK_CUR,
        GEx_Display.Bms_Data.Battery_Current);

    DWIN_Write_VPFloat(
        BATTERY_12V_VOL,
        GEx_Display.Adc_Data.Voltage_12v);

    DWIN_Write_VPFloat(
    		MCU_CUR,
		GEx_Display.Mcu_Data.Mcu_Current);

    DWIN_Write_VPFloat(
    		MCU_VOL,
		GEx_Display.Mcu_Data.Mcu_Pack_voltage);


    DWIN_Write_VP(
        MCU_FAULT_COUNT,
        GEx_Display.Mcu_Data.Mcu_Fault_Count);


    DWIN_Write_VP(
        BMS_FAULT_COUNT,
        GEx_Display.Bms_Data.Bms_Fault_Count);



    DWIN_Write_Bool(
        IS_BMS_OK,
        GEx_Display.System_Data.Is_Bms_Can_Ok);


    DWIN_Write_Bool(
        IS_MCU_OK,
        GEx_Display.System_Data.Is_Mcu_Can_Ok);


    DWIN_Write_Bool(
        IS_CAN_OK,
        GEx_Display.System_Data.Is_Can_Ok);


    DWIN_Write_Bool(
        IS_WDTR,
        GEx_Display.System_Data.Is_Watch_Dog_Reset);


    DWIN_Write_Bool(
        IS_HARD_FAULT,
        GEx_Display.System_Data.Is_System_Hard_Fault);

    DWIN_Write_VPFloat(
        CELL_1,
        GEx_Display.Bms_Data.Cells[0]);


    DWIN_Write_VPFloat(
        CELL_2,
        GEx_Display.Bms_Data.Cells[1]);


    DWIN_Write_VPFloat(
        CELL_3,
        GEx_Display.Bms_Data.Cells[2]);


    DWIN_Write_VPFloat(
        CELL_4,
        GEx_Display.Bms_Data.Cells[3]);


    DWIN_Write_VPFloat(
        CELL_5,
        GEx_Display.Bms_Data.Cells[4]);


    DWIN_Write_VPFloat(
        CELL_6,
        GEx_Display.Bms_Data.Cells[5]);


    DWIN_Write_VPFloat(
        CELL_7,
        GEx_Display.Bms_Data.Cells[6]);


    DWIN_Write_VPFloat(
        CELL_8,
        GEx_Display.Bms_Data.Cells[7]);


    DWIN_Write_VPFloat(
        CELL_9,
        GEx_Display.Bms_Data.Cells[8]);


    DWIN_Write_VPFloat(
        CELL_10,
        GEx_Display.Bms_Data.Cells[9]);


    DWIN_Write_VPFloat(
        CELL_11,
        GEx_Display.Bms_Data.Cells[10]);


    DWIN_Write_VPFloat(
        CELL_12,
        GEx_Display.Bms_Data.Cells[11]);


    DWIN_Write_VPFloat(
        CELL_13,
        GEx_Display.Bms_Data.Cells[12]);


    DWIN_Write_VPFloat(
        CELL_14,
        GEx_Display.Bms_Data.Cells[13]);


    DWIN_Write_VPFloat(
        CELL_15,
        GEx_Display.Bms_Data.Cells[14]);


    DWIN_Write_VPFloat(
        CELL_16,
        GEx_Display.Bms_Data.Cells[15]);

    DWIN_Write_VPFloat(
    		CELL_17,
        GEx_Display.Bms_Data.Cells[16]);

    DWIN_Write_VPFloat(
    		CELL_18,
        GEx_Display.Bms_Data.Cells[17]);

    DWIN_Write_VPFloat(
    		CELL_19,
        GEx_Display.Bms_Data.Cells[18]);

    DWIN_Write_VPFloat(
    		CELL_20,
        GEx_Display.Bms_Data.Cells[19]);

    DWIN_Write_VPFloat(
    		CELL_21,
        GEx_Display.Bms_Data.Cells[20]);

    DWIN_Write_VPFloat(
    		CELL_22,
        GEx_Display.Bms_Data.Cells[21]);

    DWIN_Write_VPFloat(
    		CELL_23,
        GEx_Display.Bms_Data.Cells[22]);

    DWIN_Write_VPFloat(
    		CELL_24,
        GEx_Display.Bms_Data.Cells[23]);

}

/*******************************************************************************
 * Function Name : Update_Display_strucutres
 * Description   : Fetches the latest data structures from BMS, MCU, IO, and ADC subsystems.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Update_Display_strucutres()
{
	Get_Bms_Data(&GEx_Display.Bms_Data);
	Get_Mcu_Data(&GEx_Display.Mcu_Data);
	Get_IO_Data(&GEx_Display.IO_Data);
	Uart_Printf("ADC start");
	Get_Adc_Data(&GEx_Display.Adc_Data);
//	Get_System_Data(&GEx_Display.System_Data); // need to update once all code done
}
void DWIN_Display_Test(void)
{
//    static uint8_t soc = 0;
//
//    static uint8_t speed = 0;
//
//    static uint8_t motor_temp = 30;
//
//    static uint8_t mcu_temp = 25;
//
//    static uint8_t avg_temp = 25;
//
//    static float pack_voltage = 48.00f;
//
//    static float pack_current = 0.00f;
//
//    static float cell_voltage = 3.20f;
//
//    static float voltage_12v = 11.50f;
//
//    static uint32_t odo = 10000;
//
//    static uint32_t trip = 0;
//
//    static uint8_t mode = 0;
//
//    static bool toggle = false;
//
//    uint8_t i;
//
//
//    soc++;
//
//    if (soc >100)
//    {
//        soc = 0;
//    }
//
//
//    speed++;
//
//    if (speed >= 120)
//    {
//        speed = 0;
//    }
//
//    pack_voltage += 0.10f;
//
//    if (pack_voltage >= 54.00f)
//    {
//        pack_voltage = 48.00f;
//    }
//    pack_current += 0.50f;
//
//    if (pack_current >= 50.00f)
//    {
//        pack_current = 0.00f;
//    }
//
//    voltage_12v += 0.05f;
//
//    if (voltage_12v >= 14.00f)
//    {
//        voltage_12v = 11.50f;
//    }
//
//    motor_temp++;
//
//    if (motor_temp >= 100)
//    {
//        motor_temp = 30;
//    }
//
//
//    mcu_temp++;
//
//    if (mcu_temp >= 80)
//    {
//        mcu_temp = 25;
//    }
//
//    avg_temp++;
//
//    if (avg_temp >= 60)
//    {
//        avg_temp = 25;
//    }
//
//
//    cell_voltage += 0.01f;
//
//    if (cell_voltage >= 4.20f)
//    {
//        cell_voltage = 3.20f;
//    }
//
//
//    odo++;
//
//    trip++;
//
//    if (trip >= 9999)
//    {
//        trip = 0;
//    }
//
//    toggle = !toggle;
//
//
//    mode++;
//
//    if (mode >= 3)
//    {
//        mode = 0;
//    }
//
//
//    GEx_Display.Bms_Data.Soc =
//        soc;
//
//
//    GEx_Display.Bms_Data.Battery_Pack_voltage =
//        pack_voltage;
//
//
//    GEx_Display.Bms_Data.Battery_Current =
//        pack_current;
//
//
//    GEx_Display.Bms_Data.Avg_Temp =
//        avg_temp;
//
//
//    GEx_Display.Bms_Data.Bms_Fault =
//        toggle;
//
//
//    GEx_Display.Bms_Data.Bms_Fault_Count =
//        toggle ? 1 : 0;
//
//    for (i = 0; i < 24; i++)
//    {
//        GEx_Display.Bms_Data.Cells[i] =
//            cell_voltage + ((float)i * 0.01f);
//    }
//
//
//    GEx_Display.Mcu_Data.Mcu_Pack_voltage =
//        pack_voltage;
//
//
//    GEx_Display.Mcu_Data.Mcu_Current =
//        pack_current;
//
//
//    GEx_Display.Mcu_Data.Motor_Temp =
//        motor_temp;
//
//
//    GEx_Display.Mcu_Data.Mcu_Pcb_Temp =
//        mcu_temp;
//
//
//    GEx_Display.Mcu_Data.Mcu_Fault =
//        toggle;
//
//
//    GEx_Display.Mcu_Data.Odo_Meter =
//        odo;
//
//
//    GEx_Display.Mcu_Data.Trip_Value =
//        trip;
//
//
//    GEx_Display.Mcu_Data.Mcu_Fault_Count =
//        toggle ? 1 : 0;
//
//
//    GEx_Display.Mcu_Data.Speed =
//        speed;
//
//
//    if (mode == 0)
//    {
//        GEx_Display.Mcu_Data.Forward = true;
//
//        GEx_Display.Mcu_Data.Neutral = false;
//
//        GEx_Display.Mcu_Data.Reverse = false;
//    }
//    else if (mode == 1)
//    {
//        GEx_Display.Mcu_Data.Forward = false;
//
//        GEx_Display.Mcu_Data.Neutral = true;
//
//        GEx_Display.Mcu_Data.Reverse = false;
//    }
//    else
//    {
//        GEx_Display.Mcu_Data.Forward = false;
//
//        GEx_Display.Mcu_Data.Neutral = false;
//
//        GEx_Display.Mcu_Data.Reverse = true;
//    }
//
//
//    GEx_Display.Mcu_Data.Brake_Signal =
//        toggle;
//
//
//    GEx_Display.Mcu_Data.Echo_Boost =
//        toggle;
//
//    GEx_Display.Adc_Data.Battery_Low_12V =toggle;
//
//
//
//    GEx_Display.Adc_Data.Voltage_12v =
//        voltage_12v;
//
//    GEx_Display.IO_Data.Hand_Brake =
//        toggle;
//
//
//    GEx_Display.IO_Data.Head_Light =
//        !toggle;
//
//
//    GEx_Display.IO_Data.Charge_Ack =
//        toggle;
//
//
//    GEx_Display.IO_Data.Brake_Fluid =
//        !toggle;
//
//
//    GEx_Display.IO_Data.Right_Ind =
//        toggle;
//
//
//    GEx_Display.IO_Data.Left_Ind =
//        !toggle;
//
//
//    GEx_Display.IO_Data.Mppt_On_Off =
//        toggle;
//
//    GEx_Display.System_Data.Is_Watch_Dog_Reset =
//    		!toggle;
//
//
//    GEx_Display.System_Data.Is_System_Hard_Fault =
//    		!toggle;
//
//
//    GEx_Display.System_Data.Is_Bms_Can_Ok =
//        !toggle;
//
//
//    GEx_Display.System_Data.Is_Mcu_Can_Ok =
//    		!toggle;
//
//
//    GEx_Display.System_Data.Is_Can_Ok =
//        !toggle;
//    GEx_Display.Is_Ready=!toggle;

}
void Display_Update_All()
{
 Update_Display_strucutres();
//	DWIN_Display_Test();
 Update_Dwin_Display();
}
/*******************************************************************************
 * Function Name : Print_Display_Data
 * Description   : Placeholder callback function for printing display debug information.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Print_Display_Data(void)
{
    uint8_t i;
    uint32_t value;

    Uart_Printf("\r\n");
    Uart_Printf("=====================================================\r\n");
    Uart_Printf("                  DISPLAY DATA                       \r\n");
    Uart_Printf("=====================================================\r\n");

    /* -------------------- IO STATUS -------------------- */

    Uart_Printf("Head Light             : %s\r\n",
                GEx_Display.IO_Data.Head_Light ? "ON" : "OFF");

    Uart_Printf("Left Indicator         : %s\r\n",
                GEx_Display.IO_Data.Left_Ind ? "ON" : "OFF");

    Uart_Printf("Right Indicator        : %s\r\n",
                GEx_Display.IO_Data.Right_Ind ? "ON" : "OFF");

    Uart_Printf("Pedal Brake            : %s\r\n",
                GEx_Display.Mcu_Data.Brake_Signal ? "ACTIVE" : "RELEASED");

    Uart_Printf("Hand Brake             : %s\r\n",
                GEx_Display.IO_Data.Hand_Brake ? "ACTIVE" : "RELEASED");

    Uart_Printf("Brake Fluid            : %s\r\n",
                GEx_Display.IO_Data.Brake_Fluid ? "OK" : "LOW");

    Uart_Printf("Charger Detection      : %s\r\n",
                GEx_Display.IO_Data.Charge_Ack ? "DETECTED" : "NOT DETECTED");

    Uart_Printf("MPPT                   : %s\r\n",
                GEx_Display.IO_Data.Mppt_On_Off ? "ON" : "OFF");


    /* -------------------- VEHICLE -------------------- */

    Uart_Printf("\r\n---------------- VEHICLE ----------------------------\r\n");

    Uart_Printf("Ready                  : %s\r\n",
                GEx_Display.Is_Ready ? "YES" : "NO");

    Uart_Printf("Vehicle Speed          : %u km/h\r\n",
                GEx_Display.Mcu_Data.Speed);

    Uart_Printf("ODO                    : %lu\r\n",
                (unsigned long)GEx_Display.Mcu_Data.Odo_Meter);

    Uart_Printf("Trip                   : %lu\r\n",
                (unsigned long)GEx_Display.Mcu_Data.Trip_Value);

    Uart_Printf("Forward                : %s\r\n",
                GEx_Display.Mcu_Data.Forward ? "ON" : "OFF");

    Uart_Printf("Reverse                : %s\r\n",
                GEx_Display.Mcu_Data.Reverse ? "ON" : "OFF");

    Uart_Printf("Neutral                : %s\r\n",
                GEx_Display.Mcu_Data.Neutral ? "ON" : "OFF");

    Uart_Printf("Eco/Boost              : %s\r\n",
                GEx_Display.Mcu_Data.Echo_Boost ? "ON" : "OFF");


    /* -------------------- BMS -------------------- */

    Uart_Printf("\r\n---------------- BMS -------------------------------\r\n");

    Uart_Printf("SOC                    : %u %%\r\n",
                GEx_Display.Bms_Data.Soc);

    /*
     * Battery Pack Voltage
     * Example: 48.52 V
     */
    value = (uint32_t)(GEx_Display.Bms_Data.Battery_Pack_voltage * 100.0f);

    Uart_Printf("Pack Voltage           : %lu.%02lu V\r\n",
                (unsigned long)(value / 100U),
                (unsigned long)(value % 100U));

    /*
     * Battery Current
     * Example: 12.35 A
     */
    value = (uint32_t)(GEx_Display.Bms_Data.Battery_Current * 100.0f);

    Uart_Printf("Pack Current           : %lu.%02lu A\r\n",
                (unsigned long)(value / 100U),
                (unsigned long)(value % 100U));

    Uart_Printf("Average Battery Temp   : %u C\r\n",
                GEx_Display.Bms_Data.Avg_Temp);

    Uart_Printf("BMS Fault              : %s\r\n",
                GEx_Display.Bms_Data.Bms_Fault ? "YES" : "NO");

    Uart_Printf("BMS Fault Count        : %u\r\n",
                GEx_Display.Bms_Data.Bms_Fault_Count);


    /* -------------------- MCU -------------------- */

    Uart_Printf("\r\n---------------- MCU -------------------------------\r\n");

    value = (uint32_t)(GEx_Display.Mcu_Data.Mcu_Pack_voltage * 100.0f);

    Uart_Printf("MCU Pack Voltage       : %lu.%02lu V\r\n",
                (unsigned long)(value / 100U),
                (unsigned long)(value % 100U));

    value = (uint32_t)(GEx_Display.Mcu_Data.Mcu_Current * 100.0f);

    Uart_Printf("MCU Current            : %lu.%02lu A\r\n",
                (unsigned long)(value / 100U),
                (unsigned long)(value % 100U));

    Uart_Printf("MCU PCB Temp           : %u C\r\n",
                GEx_Display.Mcu_Data.Mcu_Pcb_Temp);

    Uart_Printf("Motor Temp             : %u C\r\n",
                GEx_Display.Mcu_Data.Motor_Temp);

    Uart_Printf("MCU Fault              : %s\r\n",
                GEx_Display.Mcu_Data.Mcu_Fault ? "YES" : "NO");

    Uart_Printf("MCU Fault Count        : %u\r\n",
                GEx_Display.Mcu_Data.Mcu_Fault_Count);


    /* -------------------- 12V SYSTEM -------------------- */

    Uart_Printf("\r\n---------------- 12V SYSTEM ------------------------\r\n");

    value = (uint32_t)(GEx_Display.Adc_Data.Voltage_12v * 100.0f);

    Uart_Printf("12V Voltage            : %lu.%02lu V\r\n",
                (unsigned long)(value / 100U),
                (unsigned long)(value % 100U));

    Uart_Printf("12V Battery Status     : %s\r\n",
                GEx_Display.Adc_Data.Battery_Low_12V ?
                "LOW" : "OK");


    /* -------------------- SYSTEM STATUS -------------------- */

    Uart_Printf("\r\n---------------- SYSTEM STATUS ---------------------\r\n");

    Uart_Printf("BMS CAN                : %s\r\n",
                GEx_Display.System_Data.Is_Bms_Can_Ok ?
                "OK" : "ERROR");

    Uart_Printf("MCU CAN                : %s\r\n",
                GEx_Display.System_Data.Is_Mcu_Can_Ok ?
                "OK" : "ERROR");

    Uart_Printf("General CAN            : %s\r\n",
                GEx_Display.System_Data.Is_Can_Ok ?
                "OK" : "ERROR");

    Uart_Printf("Watchdog Reset         : %s\r\n",
                GEx_Display.System_Data.Is_Watch_Dog_Reset ?
                "YES" : "NO");

    Uart_Printf("System Hard Fault      : %s\r\n",
                GEx_Display.System_Data.Is_System_Hard_Fault ?
                "YES" : "NO");


    /* -------------------- CELL VOLTAGES -------------------- */

    Uart_Printf("\r\n---------------- CELL VOLTAGES --------------------\r\n");

    for (i = 0U; i < 24U; i++)
    {
        /*
         * Example:
         * 3.425 V -> 3425
         */
        value = (uint32_t)(GEx_Display.Bms_Data.Cells[i] * 1000.0f);

        Uart_Printf("Cell %02u               : %lu.%03lu V\r\n",
                    (unsigned int)(i + 1U),
                    (unsigned long)(value / 1000U),
                    (unsigned long)(value % 1000U));
    }


    Uart_Printf("=====================================================\r\n");
    Uart_Printf("                 END DISPLAY DATA                    \r\n");
    Uart_Printf("=====================================================\r\n");
}
/*******************************************************************************
 * Function Name : Task_Id_For_Dispaly_Print
 * Description   : Registers the periodic display print task with the task timer manager
 *                 if it hasn't been registered yet.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = Task registered or already active, false = Registration failed)
 ******************************************************************************/
static bool Task_Id_For_Dispaly_Print(void)
{
    if (status_print_task_id < 0)
    {
        status_print_task_id = Task_Timer_Register(3000, Print_Display_Data);
        if (status_print_task_id < 0)
        {
        	return false;
        }
    }
    return true;
}
bool Enable_Display_Print_Task()
{
	return Task_Id_For_Dispaly_Print();
}
/*
 Now Not using future we will use
 */


/*******************************************************************************
 * Function Name : DWIN_Set_Text
 * Description   : Writes a text string to a specified Variable Pointer (VP)
 *                 address, capped at a maximum length of 200 bytes.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Target Variable Pointer address
 *                 text    - Pointer to null-terminated character string
 * Return Value  : None
 ******************************************************************************
static void DWIN_Set_Text(uint16_t address, const char *text)
{
    uint8_t len = (uint8_t)strlen(text);
    if (len > 200u) len = 200u;
    Send_Cmd(CMD_WRITE_VP, address, (const uint8_t *)text, len);
}*/
/*******************************************************************************
 * Function Name : DWIN_ReadVP
 * Description   : Sends a read request command for a specified number of words
 *                 starting from a given Variable Pointer address.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Target Variable Pointer address
 *                 wordLen - Number of words to read
 * Return Value  : None
 ******************************************************************************
static void DWIN_ReadVP(uint16_t address, uint8_t wordLen)
{
    Send_Cmd(CMD_READ_VP, address, &wordLen, 1u);
}*/

/*******************************************************************************
 * Function Name : DWIN_SetBaudRate
 * Description   : Configures the communication baud rate of the DWIN display
 *                 by calculating the required register divisor value.
 * Scope         : Static (Private to this file)
 * Parameters    : newBaud - Desired communication baud rate (e.g., 115200)
 * Return Value  : None
 ******************************************************************************
static void DWIN_Set_BaudRate(uint32_t newBaud)
{
    uint16_t bv = (uint16_t)(3225600UL / newBaud);
    uint8_t d[4] = { 0x5Au, 0x00u, (uint8_t)(bv >> 8), (uint8_t)(bv & 0xFFu) };
    Send_Cmd(CMD_WRITE_VP, ADDR_UART2_CFG, d, 4u);
}*/
/*******************************************************************************
 * Function Name : DWIN_Set_RTC
 * Description   : Sets the real-time clock (RTC) parameters on the DWIN display.
 * Scope         : Static (Private to this file)
 * Parameters    : year, month, day, weekDay, hour, minute, second - Date & time components
 * Return Value  : None
 ******************************************************************************
static void DWIN_Set_RTC(uint8_t year, uint8_t month, uint8_t day,
                 uint8_t weekDay, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t d[9] = { 0x5Au, 0xA5u, year, month, day, weekDay, hour, minute, second };
    Send_Cmd(CMD_WRITE_VP, ADDR_RTC_SET, d, 9u);
}*/
/*******************************************************************************
 * Function Name : DWIN_Beep
 * Description   : Triggers the display's onboard buzzer for a specified duration.
 * Scope         : Static (Private to this file)
 * Parameters    : duration_10ms - Buzzer activation duration in increments of 10ms
 * Return Value  : None
 ******************************************************************************
static void DWIN_Beep(uint8_t duration_10ms)
{
    DWIN_Write_VP(ADDR_BUZZER, (uint16_t)duration_10ms);
}*/
/*******************************************************************************
 * Function Name : DWIN_Set_Brightness
 * Description   : Adjusts the display backlight brightness level (0 to 100%).
 * Scope         : Static (Private to this file)
 * Parameters    : level - Brightness percentage value
 * Return Value  : None
 ******************************************************************************
static void DWIN_Set_Brightness(uint8_t level)
{
    if (level > 100u) level = 100u;
    uint8_t d[2] = {  0x00u, level };
    Send_Cmd(CMD_WRITE_VP, ADDR_BRIGHTNESS, d, 2u);
}*/
/*******************************************************************************
 * Function Name : DWIN_System_Reset
 * Description   : Sends a hardware system reset command to reboot the DWIN display.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************
static void DWIN_System_Reset(void)
{
    uint8_t d[4] = { 0x55u, 0xAAu, 0x5Au, 0xA5u };
    Send_Cmd(CMD_WRITE_VP, ADDR_SYS_RESET, d, 4u);
}*/
/*******************************************************************************
 * Function Name : DWIN_Set_Page
 * Description   : Switches the active display page to the requested page ID,
 *                 adjusted by the global page offset.
 * Scope         : Static (Private to this file)
 * Parameters    : pageID - Target page number to switch to
 * Return Value  : None
 ******************************************************************************
static void DWIN_Set_Page(uint16_t pageID)
{
    uint16_t pid = (uint16_t)((int16_t)pageID + page_offset);
    uint8_t d[4] = { 0x5Au, 0x01u, (uint8_t)(pid >> 8), (uint8_t)(pid & 0xFFu) };
    Send_Cmd(CMD_WRITE_VP, ADDR_PAGE_SWITCH, d, 4u);
}*/
/*******************************************************************************
 * Function Name : DWIN_Get_Page
 * Description   : Requests the current active page ID from the display.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************
static void DWIN_Get_Page(void)
{
	DWIN_ReadVP(ADDR_PIC_NOW, 1u);
}*/

