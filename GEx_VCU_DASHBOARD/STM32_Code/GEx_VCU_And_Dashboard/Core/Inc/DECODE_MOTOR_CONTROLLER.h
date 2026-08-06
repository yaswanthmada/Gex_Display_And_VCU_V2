/*
 * DECODE_MOTOR_CONTROLLER.h
 *
 *  Created on: Aug 4, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_DECODE_MOTOR_CONTROLLER_H_
#define INC_DECODE_MOTOR_CONTROLLER_H_

#include"CONFIGURATION_FILES.h"


#ifdef MCU_NANJING_QUARK

#define MCU_MSG_0x011_ID  0x011
#define MCU_MSG_0x012_ID  0x012
#define MCU_MSG_0x013_ID  0x013
#define MCU_MSG_0x014_ID  0x014

typedef struct {
    uint8_t  Gear_Position;
    uint8_t  High_Or_Low;
    uint8_t  Ready;
    uint8_t  Mcu_Alarm_Level;
    uint16_t Motor_Speed;
    uint8_t  Trouble_Code;
    uint8_t  Low_Power_Mode;
    float    Ac_Voltage;
    float    Ac_Current;
    uint32_t last_rx_time;
} MCU_Msg_0x011_t;

typedef struct {
    float    Dc_Voltage;
    float    Dc_Current;
    uint8_t  Drive_Motor_State;
    uint8_t  Brake_Pedal_State;
    uint8_t  P_Gear_State;
    uint8_t  Reserved_1;
    float    Acc_Pedal_Signal_Voltage;
    uint8_t  Hardwire_Gear_State;
    uint8_t  Reserved_2;
    uint8_t  Acc_Pedal_Opening;
    uint32_t last_rx_time;
} MCU_Msg_0x012_t;

typedef struct {
    int16_t  Motor_Temp;
    int16_t  Controller_Temp;
    float    Torque_Request;
    float    Motor_Torque;
    uint16_t Motor_Output_Power;
    uint32_t last_rx_time;
} MCU_Msg_0x013_t;

typedef struct {
    uint8_t  Mcu_Code;
    uint8_t  Mcu_Hardware_Version;
    uint8_t  Mcu_Software_Version;
    uint8_t  Mcu_Small_Version;
    uint32_t last_rx_time;
} MCU_Msg_0x014_t;


void Decode_MCU_0x011(const uint8_t* Data, MCU_Msg_0x011_t* Message);
void Decode_MCU_0x012(const uint8_t* Data, MCU_Msg_0x012_t* Message);
void Decode_MCU_0x013(const uint8_t* Data, MCU_Msg_0x013_t* Message);
void Decode_MCU_0x014(const uint8_t* Data, MCU_Msg_0x014_t* Message);
#endif

#endif /* INC_DECODE_MOTOR_CONTROLLER_H_ */
