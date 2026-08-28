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
#define DRIVE_MODE        0X1009
#define REVERSE_MODE      0X1010
#define NEUTRAL_MODE      0X1011
#define ECHO_BOOST        0x1012
#define READY             0x1013
#define CAR_ROAD          0X1015

//Will update according to the display VP address
#define CELL_1  0x8000
#define CELL_2 0x8004
#define CELL_3 0x8008
#define CELL_4 0x8012
#define CELL_5 0x8016
#define CELL_6 0x8020
#define CELL_7 0x8024
#define CELL_8 0x8028
#define CELL_9 0x8032
#define CELL_10 0x8036
#define CELL_11 0x8040
#define CELL_12 0x8044
#define CELL_13 0x8048
#define CELL_14 0x8052
#define CELL_15 0x8056
#define CELL_16 0x8060
#define CELL_17 0x8064
#define CELL_18 0x8068
#define CELL_19 0x8072
#define CELL_20 0x8076
#define CELL_21 0x8080
#define CELL_22 0x8084
#define CELL_23 0x8088
#define CELL_24 0x8092

#define VEHICLE_SPEED		0X1500
#define ODO                 0x1502
#define TRIP       0x1506
#define MCU_TEMP   0x1538
#define MOTOR_TEMP 0x1536
#define BAT_PACK_TEMP    0x1540
#define SOC        0X1516

#define BATTERY_PACK_VOL 0x8096
#define BATTERY_PACK_CUR 0x8108
#define BATTERY_12VOL    0x8112
#define MCU_CUR          0x8100
#define MCU_VOL          0x8104
#define MCU_FAULT_COUNT  0x1520
#define BMS_FAULT_COUNT  0x1522
#define IS_BMS_OK        0x1524
#define IS_MCU_OK        0x1526
#define IS_CAN_OK        0x1528
#define IS_WDTR          0x1530
#define IS_HARD_FAULT    0x1532
#define BATTERY_12V_VOL  0x1536

#define DISPLAY_MAX_FAULTS   4
#define MCU_FAULT_1   0x6000
#define MCU_FAULT_2   0x6020
#define MCU_FAULT_3   0x6040
#define MCU_FAULT_4	  0x6060

#define BMS_FAULT_1   0x6500
#define BMS_FAULT_2   0x6520
#define BMS_FAULT_3   0x6540
#define BMS_FAULT_4   0x6560



#endif /* INC_DWIN_DEFINES_H_ */
