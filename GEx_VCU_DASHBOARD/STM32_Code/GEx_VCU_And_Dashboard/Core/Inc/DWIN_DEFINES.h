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


#define LEFT_INDICATOR    0x1002  // Decimal: 4102
#define RIGHT_INDICATOR   0x1003  // Decimal: 4103
#define CHARGER_DETECTION 0x1004  // Decimal: 4104
#define BATTERY_FAULT_12V 0x1005  // Decimal: 4105
#define SOLAR_CHRG_DETCT  0x1006
#define HAND_BRAKE        0x1007  // Decimal: 4107
#define BRAKE_FLUID       0x1008  // Decimal: 4108
#define HEAD_LIGHTS       0x1009  // Decimal: 4109
#define PARK_MODE         0x100A  // Decimal: 4110
#define REVERSE_MODE      0x100B  // Decimal: 4111
#define NEUTRAL_MODE      0x100C  // Decimal: 4112
#define DRIVE_MODE        0x100D  // Decimal: 4113
#define ECHO_BOOST        0x100E  // Decimal: 4114

#define IS_MCU_OK         0x1024  // Decimal: 4132
#define IS_BMS_OK         0x1023  // Decimal: 4131
#define IS_CAN_OK         0x1025  // Decimal: 4133
#define IS_HARD_FAULT     0x1026  // Decimal: 4134
#define IS_WDTR           0x1027  // Decimal: 4135
#define CAR_ROAD          0x1028  // Decimal: 4136
#define ODO_TRIP          0x102A  // Decimal: 4140

#define VEHICLE_SPEED_HIGH_VAL     0x2002
#define VEHICLE_SPEED_LOW_VAL      0x2000
#define TRIP_VAL                  0x2004
#define ODO_TRIP_VAL               0x2008
#define SOC_VAL                    0x200C
#define MOTOR_TEMP_VAL             0x200E
#define MCU_TEMP_VAL               0x2012
#define BAT_PACK_TEMP_VAL          0x2014
#define MCU_VOL_VAL                0x2016
#define MCU_CUR_VAL                0x201A
#define BATTERY_PACK_CUR_VAL       0x201E
#define BATTERY_12VOL_VAL          0x2022
#define BATTERY_PACK_VOL_VAL       0x2026

#define MCU_FAULT_COUNT   0x2136
#define BMS_FAULT_COUNT   0x2138

#define CELL_1    0x2040  // Decimal: 8256
#define CELL_2    0x2044  // Decimal: 8260
#define CELL_3    0x2048  // Decimal: 8264
#define CELL_4    0x204C  // Decimal: 8268
#define CELL_5    0x2050  // Decimal: 8272
#define CELL_6    0x2054  // Decimal: 8276
#define CELL_7    0x2058  // Decimal: 8280
#define CELL_8    0x205C  // Decimal: 8284
#define CELL_9    0x2060  // Decimal: 8288
#define CELL_10   0x2064  // Decimal: 8292
#define CELL_11   0x2068  // Decimal: 8296
#define CELL_12   0x206C  // Decimal: 8300
#define CELL_13   0x2070  // Decimal: 8304
#define CELL_14   0x2074  // Decimal: 8308
#define CELL_15   0x2078  // Decimal: 8312
#define CELL_16   0x207C  // Decimal: 8316
#define CELL_17   0x2080  // Decimal: 8320
#define CELL_18   0x2084  // Decimal: 8324
#define CELL_19   0x2088  // Decimal: 8328
#define CELL_20   0x208C  // Decimal: 8332
#define CELL_21   0x2090  // Decimal: 8336
#define CELL_22   0x2094  // Decimal: 8340
#define CELL_23   0x2098  // Decimal: 8344
#define CELL_24   0x209C  // Decimal: 8348

#define DISPLAY_MAX_FAULTS   4
#define MCU_FAULT_1   0x6000
#define MCU_FAULT_2   0x6020
#define MCU_FAULT_3   0x6040
#define MCU_FAULT_4	  0x6060

#define BMS_FAULT_1   0x6500
#define BMS_FAULT_2   0x6520
#define BMS_FAULT_3   0x6540
#define BMS_FAULT_4   0x6560

#define NEUTRAL  0
#define DRIVE    1
#define REVERSE  2

#endif /* INC_DWIN_DEFINES_H_ */
