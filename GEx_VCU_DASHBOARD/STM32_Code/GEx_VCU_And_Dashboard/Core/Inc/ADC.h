/*
 * ADC.h
 *
 *  Created on: Aug 11, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include"RCC.h"
#include"main.h"
#include"SYSTICK.h"
#include"TASK_TIMER.h"
#include"UART.h"
#include"DWIN_DISPLAY.h"


#define ADC_MAX_CLOCK_HZ     14000000UL
#define ADC_VREF_MV           3300UL
#define ADC_MAX_VALUE          4095UL
#define BATTERY_DIVIDER_R1           30000.0f
#define BATTERY_DIVIDER_R2           10000.0f
#define BATTERY_DIVIDER_RATIO        (BATTERY_DIVIDER_R2 / (BATTERY_DIVIDER_R1 + BATTERY_DIVIDER_R2))

#define BATTERY_FAULT_THRESHOLD_V    10.0f
#define BATTERY_FAULT_DEBOUNCE_CNT   5
#define BATTERY_ADC_IDX       0


#define BATTERY_12V_PORT  GPIOB
#define BATTERY_12V_PIN   0
#define BATTERY_12V_CHANNEL  8
#define BATTERY_FAULT_HOLD_TIME_MS   1000
typedef struct
{
    ADC_TypeDef  *ADCx;
    GPIO_TypeDef *Port;
    uint8_t       Pin;
    uint8_t       Channel;
} ADC_Channel_t;
typedef struct
{
    float   voltage;
    float    Pin_Voltage;
    float    Input_Voltage;
    uint32_t Last_Read_Time;
    uint32_t Low_Voltage_Since;
    uint16_t Raw_Adc;
    bool    fault;
} Battery_Status_t;


/*******************************************************************************
 * Function Name : Analog_To_Digital_Converter_Init
 * Description   : High-level wrapper function to initialize ADC1 for the application.
 * Scope         : Public / Driver API
 * Parameters    : None
 * Return Value  : bool (true = Initialization successful, false = Failed)
 ******************************************************************************/
bool Analog_To_Digital_Converter_Init(void);

/*******************************************************************************
 * Function Name : Enable_Adc_Print_Task
 * Description   : Enables the periodic background printing task for ADC debug data.
 * Scope         : Public / Driver API
 * Parameters    : None
 * Return Value  : bool (true = Enabled successfully, false = Failed)
 ******************************************************************************/
bool Enable_Adc_Print_Task();
/*******************************************************************************
 * Function Name : Update_Adc
 * Description   : Performs a full refresh cycle by updating all ADC channels
 *                 and recalculating battery voltage metrics.
 * Scope         : Public / Driver API
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Update_Adc();

#endif /* INC_ADC_H_ */
