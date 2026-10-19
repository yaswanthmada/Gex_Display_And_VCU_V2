/*
 * MOTOR_DATA.h
 *
 *  Created on: Sep 9, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_MOTOR_DATA_H_
#define INC_MOTOR_DATA_H_

#include <stdint.h>
#include <stdbool.h>

void   Odometer_Init(void);
void   Update_Odometer(float Speed_Kmh);
void   Reset_Trip_Odometer(void);
void Check_Trip_Reset_Hold(uint8_t Switch_Level);
double Get_Odometer_Km(void);
double Get_Trip_Km(void);

#define TRIP_HOLD_RESET_MS      5000U
#define TRIP_SWITCH_PRESSED_LEVEL  0U   /* value that means pressed. 0 = pin LOW (your wiring).
                                           Change to 1 if yours is the other way round. */



#ifdef ODO_SELFTEST_BUILD
bool Run_Odometer_Tests(void);
#endif

#endif /* INC_MOTOR_DATA_H_ */
