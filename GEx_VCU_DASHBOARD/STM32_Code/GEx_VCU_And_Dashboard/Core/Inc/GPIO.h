/*
 * GPIO.h
 *
 *  Created on: Aug 7, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_
#include"SYSTICK.h"
#include"TASK_TIMER.h"
#include"UART.h"
#include<string.h>
#include"DWIN_DISPLAY.h"

#define GPIO_INPUT_NO_PULL    0U
#define GPIO_INPUT_PULL_UP    1U
#define GPIO_INPUT_PULL_DOWN  2U


#define GPIO_OUTPUT_PUSHPULL   0U
#define GPIO_OUTPUT_OPENDRAIN  1U

#define GPIO_SPEED_10MHZ       1U
#define GPIO_SPEED_2MHZ        2U
#define GPIO_SPEED_50MHZ       3U

#define GPIO_STATE_LOW         0U
#define GPIO_STATE_HIGH        1U

//Input pin Ports
#define HAND_BRAKE_PORT     GPIOB
#define BRAKE_FLUID_PORT	GPIOA
#define HEAD_LIGHT_PORT		GPIOA
#define CHARGE_ACK_PORT		GPIOA
#define LEFT_INDICATOR_PORT	 GPIOA
#define RIGHT_INDICATOR_PORT GPIOA
#define MPPT_12V_IP_PORT       GPIOA

#define HAND_BRAKE_PIN    4
#define BRAKE_FLUID_PIN  7
#define HEAD_LIGHT_PIN    0
#define CHARGE_ACK_PIN    13
#define LEFT_INDICATOR_PIN   1
#define RIGHT_INDICATOR_PIN  2
#define MPPT_12V_IP_PIN       3


//Output pin Ports
#define MPPT_12V_OP_PORT  GPIOA
#define MPPT_12V_OP_PIN  6


/* Maximum number of debounced inputs */
#define MAX_DEBOUNCED_INPUTS     8

/* Fast debounce (9ms total): 3ms sample interval, 3 samples */
#define DEBOUNCE_FAST_INTERVAL_MS    3
#define DEBOUNCE_FAST_Min_Samples    3

/* Standard debounce (30ms total): 10ms sample interval, 3 samples */
#define DEBOUNCE_STD_INTERVAL_MS     10
#define DEBOUNCE_STD_Min_Samples     3

/* Slow debounce (150ms total): 30ms sample interval, 5 samples */
#define DEBOUNCE_SLOW_INTERVAL_MS    30
#define DEBOUNCE_SLOW_Min_Samples    5

#define DEBOUNCE_DELAY_MS         10    /* 10ms total debounce delay */
#define DEBOUNCE_Min_Samples      3     /* Minimum 3 consistent samples */
#define DEBOUNCE_SAMPLE_INTERVAL  3     /* Sample every 3ms */

typedef struct
{
    GPIO_TypeDef* Port;           /**< GPIO Port (NULL for virtual Pins) */
    uint16_t Pin;                 /**< GPIO Pin (unused for virtual Pins) */
    uint32_t Last_Debounce_Time;  /**< Time when state change started */
    uint32_t Last_Sample_Time;    /**< Time of last sample */
    uint16_t Sample_Interval_Ms;  /**< Sample interval in milliseconds */
    uint8_t Min_Samples;          /**< Minimum number of consistent samples required */
    bool Stable_State;            /**< Current stable/debounced state */
    bool Current_State;           /**< Current reading state */
    bool Previous_Stable_State;   /**< Previous stable state for edge detection */
    uint8_t Sample_Count;         /**< Number of consistent samples */
    bool Debouncing;              /**< True if currently Debouncing */
    bool Initialized;             /**< True if Initialized */
} Debounce_Data_t;

typedef struct
{
	GPIO_TypeDef *Port;
	uint8_t  Pin;
    uint8_t     Mode;
    uint8_t     Speed;
    uint8_t     Initial_State;
}Digital_Output_Pin;
typedef struct
{
	GPIO_TypeDef *Port;
	uint16_t      Pin;
    uint8_t      Mode;
}Digital_Input_Pin;
typedef struct
{
	bool Hand_Brake_Sts;
	bool Mppt_Op_Bp_Sts;
	bool Brake_Fluid_Sts;
	bool Mppt_Ip_Sts;
	bool Head_Light_Sts;
	bool Charge_Ack_Sts;
	bool Left_Indicator_Sts;
	bool Right_Indicator_sts;
	bool Mppt_sts;

}Gpio_Status;
/*******************************************************************************
 * Function Name : General_Purpose_Input_Output_Init
 * Description   : Top-level GPIO peripheral initialization function. Configures
 *                 all digital input and output pins and initializes debouncing
 *                 structures for system inputs.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : bool (true = All input, output, and debounce configurations initialized successfully,
 *                       false = Initialization failure in sub-modules)
 ******************************************************************************/
bool General_Purpose_Input_Output_Init();
/*******************************************************************************
 * Function Name : Update_Gpio
 * Description   : Top-level execution call to process input pin debouncing
 *                 routines and update system input state variables.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Update_Gpio();
/*******************************************************************************
 * Function Name : Enable_Gpio_Print_Task
 * Description   : Top-level wrapper function to schedule and enable the
 *                 periodic GPIO status UART reporting task.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : bool (true = Print task enabled/active, false = Scheduling failed)
 ******************************************************************************/
bool Enable_Gpio_Print_Task();

#endif /* INC_GPIO_H_ */
