/*
 * DWIN_DISPLAY.c
 *
 *  Created on: Aug 10, 2026
 *      Author: MadaYaswanth
 */


#include"DWIN_DISPLAY.h"
static int status_print_task_id=-1;
GEx_Display_t GEx_Display ;
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
    uint8_t frame[100];
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
/*******************************************************************************
 * Function Name : Update_Dwin_Display
 * Description   : Synchronizes all vehicle telemetry, sensor inputs, and state
 *                 variables with their corresponding DWIN UI variable pointer addresses.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Update_Dwin_Display()
{
    DWIN_Write_VP(HEAD_LIGHTS,GEx_Display.IO_Data.Head_Light );
    DWIN_Write_VP(LEFT_INDICATOR,GEx_Display.IO_Data.Right_Ind);
    DWIN_Write_VP(PEDDAL_BRAKE,GEx_Display.Mcu_Data.Brake_Signal);
    DWIN_Write_VP(HAND_BRAKE,GEx_Display.Mcu_Data.Brake_Signal);
    DWIN_Write_VP(BRAKE_FLUID,  GEx_Display.IO_Data.Brake_Fluid);
    DWIN_Write_VP(BATTERY_FAULT_12V,  GEx_Display.Adc_Data.Battery_Low_12V);
    DWIN_Write_VP(CHARGER_DETECTION, GEx_Display.IO_Data.Charge_Ack);
    DWIN_Write_VP(RIGHT_INDICATOR,GEx_Display.IO_Data.Right_Ind);
    DWIN_Write_VP(VEHICLE_SPEED,GEx_Display.Mcu_Data.Speed);

    DWIN_Write_VP(TRIP,GEx_Display.Mcu_Data.Trip_Value);

    DWIN_Write_VP(MCU_TEMP,GEx_Display.Mcu_Data.Mcu_Pcb_Temp);
    DWIN_Write_VP(MOTOR_TEMP,GEx_Display.Mcu_Data.Motor_Temp);

    DWIN_Write_VP(SOC,GEx_Display.Bms_Data.Soc);

    DWIN_Write_VP(BATTERY_PACK_VOL,GEx_Display.Bms_Data.Battery_Pack_voltage);
    DWIN_Write_VP(BATTERY_PACK_CUR,GEx_Display.Bms_Data.Battery_Current);

    DWIN_Write_VP(DRIVE_MODE,GEx_Display.Mcu_Data.Forward);
    DWIN_Write_VP(REVERSE_MODE,GEx_Display.Mcu_Data.Reverse);
    DWIN_Write_VP(NEUTRAL_MODE,GEx_Display.Mcu_Data.Neutral);
    DWIN_Write_VP(ECHO_BOOST,GEx_Display.Mcu_Data.Echo_Boost);



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
void Display_Update_All()
{
 Update_Display_strucutres();
 Update_Dwin_Display();
}
/*******************************************************************************
 * Function Name : Print_Display_Data
 * Description   : Placeholder callback function for printing display debug information.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Print_Display_Data()
{
	Uart_Printf("\r\n=================== DISPLAY DATA ===================\r\n");
	    Uart_Printf("  Head Light             : %s\r\n", GEx_Display.IO_Data.Head_Light ? "On" : "Off");
	    Uart_Printf("  Left Indicator         : %s\r\n", GEx_Display.IO_Data.Right_Ind ? "On" : "Off");
	    Uart_Printf("  Pedal Brake            : %s\r\n", GEx_Display.Mcu_Data.Brake_Signal ? "Active" : "Released");
	    Uart_Printf("  Hand Brake             : %s\r\n", GEx_Display.Mcu_Data.Brake_Signal ? "Active" : "Released");
	    Uart_Printf("  Brake Fluid            : %s\r\n", GEx_Display.IO_Data.Brake_Fluid ? "OK" : "Low");
	    Uart_Printf("  12V Battery Fault      : %s\r\n", GEx_Display.Adc_Data.Battery_Low_12V ? "Low" : "OK");
	    Uart_Printf("  Charger Detection      : %s\r\n", GEx_Display.IO_Data.Charge_Ack ? "Detected" : "Not Detected");
	    Uart_Printf("  Right Indicator        : %s\r\n", GEx_Display.IO_Data.Right_Ind ? "On" : "Off");
	    Uart_Printf("  Vehicle Speed          : %u\r\n", GEx_Display.Mcu_Data.Speed);
	    Uart_Printf("  Trip Value             : %lu\r\n", (unsigned long)GEx_Display.Mcu_Data.Trip_Value);
	    Uart_Printf("  MCU PCB Temp           : %u C\r\n", GEx_Display.Mcu_Data.Mcu_Pcb_Temp);
	    Uart_Printf("  Motor Temp             : %u C\r\n", GEx_Display.Mcu_Data.Motor_Temp);
	    Uart_Printf("  Battery SOC            : %u %%\r\n", GEx_Display.Bms_Data.Soc);
	    Uart_Printf("  Battery Pack Voltage   : %.2f V\r\n", (float)GEx_Display.Bms_Data.Battery_Pack_voltage);
	    Uart_Printf("  Battery Current        : %.2f A\r\n", (float)GEx_Display.Bms_Data.Battery_Current);
	    Uart_Printf("  Forward Mode           : %s\r\n", GEx_Display.Mcu_Data.Forward ? "On" : "Off");
	    Uart_Printf("  Reverse Mode           : %s\r\n", GEx_Display.Mcu_Data.Reverse ? "On" : "Off");
	    Uart_Printf("  Neutral Mode           : %s\r\n", GEx_Display.Mcu_Data.Neutral ? "On" : "Off");
	    Uart_Printf("  Eco/Boost Mode         : %s\r\n", GEx_Display.Mcu_Data.Echo_Boost ? "On" : "Off");
	    Uart_Printf("  General CAN Status     : %s\r\n", GEx_Display.System_Data.Is_Can_Ok ? "OK" : "Error");
	    Uart_Printf("  BMS CAN Status         : %s\r\n", GEx_Display.System_Data.Is_Bms_Can_Ok ? "OK" : "Error");
	    Uart_Printf("  MCU CAN Status         : %s\r\n", GEx_Display.System_Data.Is_Mcu_Can_Ok ? "OK" : "Error");
	    Uart_Printf("  Cell Temp [0]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[0]);
	    Uart_Printf("  Cell Temp [1]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[1]);
	    Uart_Printf("  Cell Temp [2]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[2]);
	    Uart_Printf("  Cell Temp [3]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[3]);
	    Uart_Printf("  Cell Temp [4]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[4]);
	    Uart_Printf("  Cell Temp [5]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[5]);
	    Uart_Printf("  Cell Temp [6]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[6]);
	    Uart_Printf("  Cell Temp [7]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[7]);
	    Uart_Printf("  Cell Temp [8]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[8]);
	    Uart_Printf("  Cell Temp [9]          : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[9]);
	    Uart_Printf("  Cell Temp [10]         : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[10]);
	    Uart_Printf("  Cell Temp [11]         : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[11]);
	    Uart_Printf("  Cell Temp [12]         : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[12]);
	    Uart_Printf("  Cell Temp [13]         : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[13]);
	    Uart_Printf("  Cell Temp [14]         : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[14]);
	    Uart_Printf("  Cell Temp [15]         : %.2f C\r\n", (float)GEx_Display.Bms_Data.Cells[15]);
	    Uart_Printf("  Average Battery Temp   : %u C\r\n", GEx_Display.Bms_Data.Avg_Temp);
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
 * Function Name : DWIN_Write_VP32
 * Description   : Writes a 32-bit integer value to a specified Variable Pointer (VP)
 *                 address on the DWIN display.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Target Variable Pointer address
 *                 value   - 32-bit unsigned integer value to write
 * Return Value  : None
 ******************************************************************************
static void DWIN_Write_VP32(uint16_t address, uint32_t value)
{
    uint8_t d[4] = {
        (uint8_t)((value >> 24) & 0xFFu),
        (uint8_t)((value >> 16) & 0xFFu),
        (uint8_t)((value >>  8) & 0xFFu),
        (uint8_t)( value        & 0xFFu)
    };
    Send_Cmd(CMD_WRITE_VP, address, d, 4u);
}*/
/*******************************************************************************
 * Function Name : DWIN_Write_VPFloat
 * Description   : Writes a single-precision floating-point value to a specified
 *                 Variable Pointer (VP) address by casting it via a union.
 * Scope         : Static (Private to this file)
 * Parameters    : address - Target Variable Pointer address
 *                 value   - Float value to write
 * Return Value  : None
 ******************************************************************************
static void DWIN_Write_VPFloat(uint16_t address, float value)
{
    union { float f; uint32_t u; } v;
    v.f = value;
    DWIN_Write_VP32(address, v.u);
}*/
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

