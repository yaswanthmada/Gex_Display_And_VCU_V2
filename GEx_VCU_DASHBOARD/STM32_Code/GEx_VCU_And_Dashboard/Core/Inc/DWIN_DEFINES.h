/*
 * DWIN_DEFINES.h
 *
 *  Created on: Aug 11, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_DWIN_DEFINES_H_
#define INC_DWIN_DEFINES_H_

#include<stdbool.h>
#include<stdint.h>

#define DWIN_HEADER_1           0x5A
#define DWIN_HEADER_2           0xA5

#define CMD_WRITE_VP            0x82
#define CMD_READ_VP             0x83
#define CMD_WRITE_CURVE         0x84

#define ADDR_SYS_RESET          0x0004
#define ADDR_NOR_FLASH_RW       0x0008
#define ADDR_UART2_CFG          0x000C
#define ADDR_VERSION            0x000F
#define ADDR_RTC_NOW            0x0010
#define ADDR_PIC_NOW            0x0014
#define ADDR_SYS_CONFIG         0x0080
#define ADDR_BRIGHTNESS         0x0082
#define ADDR_PAGE_SWITCH        0x0084
#define ADDR_PWM0_SET           0x0086
#define ADDR_RTC_SET            0x009C
#define ADDR_BUZZER             0x00A0
#define ADDR_ICL_SWITCH         0x00DE
#define ADDR_PAGE_STACK         0x00E8

#define COLOR_BLACK             0x0000
#define COLOR_WHITE             0xFFFF
#define COLOR_RED               0xF800
#define COLOR_GREEN             0x07E0
#define COLOR_BLUE              0x001F
#define COLOR_YELLOW            0xFFE0
#define COLOR_CYAN              0x07FF
#define COLOR_MAGENTA           0xF81F

#define HEAD_LIGHTS       0x1000
#define LEFT_INDICATOR    0x1001
#define PEDDAL_BRAKE      0x1002
#define HAND_BRAKE        0x1003
#define BRAKE_FLUID       0x1004
#define BATTERY_FAULT_12V 0x1005
#define CHARGER_DETECTION 0x1006
#define RIGHT_INDICATOR   0x1007
#define DRIVE_MODE      0X1009
#define REVERSE_MODE     0X1010
#define NEUTRAL_MODE     0X1011
#define ECHO_BOOST 0x1012
#define READY 0x1013
#define CAR_ROAD    0X1015

//Will update according to the display VP address
#define BATTERY_PACK_TEMP 0
#define BATTERY_PACK_VOL 0
#define BATTERY_PACK_CUR 0
#define CELL_1  0
#define CELL_2 0
#define CELL_3 0
#define CELL_4 0
#define CELL_5 0
#define CELL_6 0
#define CELL_7 0
#define CELL_8 0
#define CELL_9 0
#define CELL_10 0
#define CELL_11 0
#define CELL_12 0
#define CELL_13 0
#define CELL_14 0
#define CELL_15 0

#define MCU_TEMP 0
#define MOTOR_TEMP 0
#define VEHICLE_SPEED 0

#define SPEED		0X2002
#define TEMP_VAL    0x2000

#define SOC    0X1016


#define ODO 0
#define TRIP 0
#define BMS_FAULT_NAMES_LENGTH 40
#define MCU_FAULT_NAMES_LENGTH 40

#define CAN_COMM 0
#define BMS_CAN_COMM 0
#define MCU_CAN_COMM 0

#define DWIN_RGB(r,g,b) \
    ((uint16_t)(((uint16_t)((r)&0x1F)<<11)|((uint16_t)((g)&0x3F)<<5)|((uint16_t)((b)&0x1F))))




#endif /* INC_DWIN_DEFINES_H_ */
