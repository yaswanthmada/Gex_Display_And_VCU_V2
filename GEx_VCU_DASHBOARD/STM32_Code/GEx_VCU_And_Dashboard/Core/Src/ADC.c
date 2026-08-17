/*
 * ADC.c
 *
 *  Created on: Aug 11, 2026
 *      Author: MadaYaswanth
 */

#include "ADC.h"

#define BATTERY_ADC_AVG_SAMPLES       16U
#define BATTERY_FAULT_THRESHOLD_V     10.0f

static const ADC_Channel_t ADC_Channels[] =
{
    { ADC1, BATTERY_12V_PORT, BATTERY_12V_PIN, BATTERY_12V_CHANNEL }
};

#define NUM_ADC_CHANNELS \
    (sizeof(ADC_Channels) / sizeof(ADC_Channels[0]))

static Battery_Status_t Battery_Status = {0};
static uint16_t ADC_Results[NUM_ADC_CHANNELS];
static int status_print_task_id = -1;


/*******************************************************************************
 * Function Name : ADC_Pin_Init
 * Description   : Configures a specified GPIO pin as an analog input.
 * Scope         : Static (Private to this file)
 * Parameters    : GPIOx   - Pointer to the GPIO port
 *                 Pin_Pos - GPIO pin position from 0 to 15
 * Return Value  : None
 ******************************************************************************/
static void ADC_Pin_Init(GPIO_TypeDef *GPIOx, uint8_t Pin_Pos)
{
    if (Pin_Pos < 8U)
    {
        GPIOx->CRL &= ~(0xFU << (Pin_Pos * 4U));
    }
    else
    {
        GPIOx->CRH &= ~(0xFU << ((Pin_Pos - 8U) * 4U));
    }
}


/*******************************************************************************
 * Function Name : ADC_Set_Prescaler
 * Description   : Configures the ADC clock prescaler based on APB2 frequency.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = successful, false = invalid frequency)
 ******************************************************************************/
static bool ADC_Set_Prescaler(void)
{
    uint32_t Apb2_Freq = Get_APB2_freq();
    uint32_t Adc_Pre;

    if ((Apb2_Freq / 2U) <= ADC_MAX_CLOCK_HZ)
    {
        Adc_Pre = 0b00U;
    }
    else if ((Apb2_Freq / 4U) <= ADC_MAX_CLOCK_HZ)
    {
        Adc_Pre = 0b01U;
    }
    else if ((Apb2_Freq / 6U) <= ADC_MAX_CLOCK_HZ)
    {
        Adc_Pre = 0b10U;
    }
    else if ((Apb2_Freq / 8U) <= ADC_MAX_CLOCK_HZ)
    {
        Adc_Pre = 0b11U;
    }
    else
    {
        return false;
    }

    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= (Adc_Pre << RCC_CFGR_ADCPRE_Pos);

    return true;
}


/*******************************************************************************
 * Function Name : ADC_Init
 * Description   : Initializes ADC peripheral, configures GPIO pins,
 *                 sets ADC clock and performs ADC calibration.
 * Scope         : Static (Private to this file)
 * Parameters    : ADCx - Pointer to ADC peripheral
 * Return Value  : bool (true = successful, false = failed)
 ******************************************************************************/
static bool ADC_Init(ADC_TypeDef *ADCx)
{
    for (uint8_t i = 0U; i < NUM_ADC_CHANNELS; i++)
    {
        if (ADC_Channels[i].Port == GPIOA)
        {
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        }
        else if (ADC_Channels[i].Port == GPIOB)
        {
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        }
        else if (ADC_Channels[i].Port == GPIOC)
        {
            RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
        }
        else
        {
            return false;
        }
    }

    if (!ADC_Set_Prescaler())
    {
        return false;
    }

    for (uint8_t i = 0U; i < NUM_ADC_CHANNELS; i++)
    {
        ADC_Pin_Init(
            ADC_Channels[i].Port,
            ADC_Channels[i].Pin
        );
    }

    if (ADCx == ADC1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    }
    else if (ADCx == ADC2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
    }
    else
    {
        return false;
    }

    ADCx->CR2 |= ADC_CR2_ADON;

    for (volatile uint32_t i = 0U; i < 1000U; i++)
    {
        __NOP();
    }

    ADCx->CR2 |= ADC_CR2_RSTCAL;

    while (ADCx->CR2 & ADC_CR2_RSTCAL)
    {
    }

    ADCx->CR2 |= ADC_CR2_CAL;

    while (ADCx->CR2 & ADC_CR2_CAL)
    {
    }

    ADCx->CR1 &= ~ADC_CR1_SCAN;
    ADCx->CR2 &= ~ADC_CR2_CONT;

    ADCx->CR2 &= ~ADC_CR2_EXTSEL;
    ADCx->CR2 |= (0b111U << 17U);

    ADCx->CR2 |= ADC_CR2_EXTTRIG;

    return true;
}


/*******************************************************************************
 * Function Name : ADC_Read_Channel
 * Description   : Selects the ADC channel, starts a software conversion,
 *                 waits for conversion completion and returns raw ADC value.
 * Scope         : Public / Driver API
 * Parameters    : ADCx    - Pointer to ADC peripheral
 *                 channel - ADC channel number
 * Return Value  : uint16_t (12-bit ADC conversion result)
 ******************************************************************************/
uint16_t ADC_Read_Channel(ADC_TypeDef *ADCx, uint8_t channel)
{
    ADCx->SQR1 &= ~ADC_SQR1_L;

    ADCx->SQR3 = (channel & 0x1FU);

    if (channel < 10U)
    {
        ADCx->SMPR2 &= ~(0x7U << (channel * 3U));
        ADCx->SMPR2 |= (0x7U << (channel * 3U));
    }
    else
    {
        ADCx->SMPR1 &= ~(0x7U << ((channel - 10U) * 3U));
        ADCx->SMPR1 |= (0x7U << ((channel - 10U) * 3U));
    }

    ADCx->SR &= ~ADC_SR_EOC;

    ADCx->CR2 |= ADC_CR2_SWSTART;

    while (!(ADCx->SR & ADC_SR_EOC))
    {
    }

    return (uint16_t)(ADCx->DR & 0x0FFFU);
}


/*******************************************************************************
 * Function Name : ADC_Get_Result
 * Description   : Retrieves the latest cached ADC conversion result.
 * Scope         : Public / Driver API
 * Parameters    : index - ADC channel array index
 * Return Value  : uint16_t (ADC result, or 0 if index is invalid)
 ******************************************************************************/
uint16_t ADC_Get_Result(uint8_t index)
{
    if (index >= NUM_ADC_CHANNELS)
    {
        return 0U;
    }

    return ADC_Results[index];
}


/*******************************************************************************
 * Function Name : Battery_ADC_Get_Average
 * Description   : Takes multiple ADC samples and returns their average value.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint16_t (Average ADC value)
 ******************************************************************************/
static uint16_t Battery_ADC_Get_Average(void)
{
    uint32_t Adc_Sum = 0U;

    for (uint8_t i = 0U; i < BATTERY_ADC_AVG_SAMPLES; i++)
    {
        Adc_Sum += ADC_Read_Channel(
            ADC_Channels[BATTERY_ADC_IDX].ADCx,
            ADC_Channels[BATTERY_ADC_IDX].Channel
        );
    }

    return (uint16_t)
           (Adc_Sum / BATTERY_ADC_AVG_SAMPLES);
}


/*******************************************************************************
 * Function Name : Battery_Voltage_Update
 * Description   : Reads the 12 V battery voltage, averages ADC samples,
 *                 converts the ADC value to battery voltage and detects
 *                 low-voltage condition below the configured threshold.
 * Scope         : Public / Driver API
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Battery_Voltage_Update(void)
{
    uint16_t Raw_Adc;
    float Pin_Voltage;
    float Battery_Voltage;
    uint32_t Current_Time;
    Raw_Adc = Battery_ADC_Get_Average();
    Pin_Voltage =
        ((float)Raw_Adc / (float)ADC_MAX_VALUE) *
        (ADC_VREF_MV / 1000.0f);
    Battery_Voltage =Pin_Voltage * BATTERY_DIVIDER_RATIO;
    Current_Time = Get_Tick_Ms();
    Battery_Status.Raw_Adc = Raw_Adc;
    Battery_Status.Pin_Voltage = Pin_Voltage;
    Battery_Status.voltage = Battery_Voltage;
    Battery_Status.Input_Voltage = Battery_Voltage;
    Battery_Status.Last_Read_Time = Current_Time;
    if (Battery_Voltage <= BATTERY_FAULT_THRESHOLD_V)
    {
        if (Battery_Status.Low_Voltage_Since == 0U)
        {
            Battery_Status.Low_Voltage_Since = Current_Time;
        }

        if ((Current_Time -
             Battery_Status.Low_Voltage_Since) >=
            BATTERY_FAULT_HOLD_TIME_MS)
        {
            Battery_Status.fault = true;
        }
    }
    else
    {
        Battery_Status.Low_Voltage_Since = 0U;
        Battery_Status.fault = false;
    }
}


/*******************************************************************************
 * Function Name : ADC_Update_All_Channels
 * Description   : Reads all configured ADC channels and updates the
 *                 cached ADC result array.
 * Scope         : Public / Driver API
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void ADC_Update_All_Channels(void)
{
    for (uint8_t i = 0U; i < NUM_ADC_CHANNELS; i++)
    {
        ADC_Results[i] =
            ADC_Read_Channel(
                ADC_Channels[i].ADCx,
                ADC_Channels[i].Channel
            );
    }
}


/*******************************************************************************
 * Function Name : Print_Adc_Data
 * Description   : Prints battery ADC value, pin voltage, battery voltage,
 *                 timestamp and low-voltage fault status over UART.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Print_Adc_Data(void)
{
    uint32_t elapsed;

    elapsed =
        Get_Tick_Ms() -
        Battery_Status.Last_Read_Time;

    Uart_Printf(
        "Raw=%u  PinV=%.2fV  BatteryV=%.2fV  "
        "LastRead=%lu ms  Elapsed=%lu ms  Fault=%s\r\n",

        Battery_Status.Raw_Adc,

        Battery_Status.Pin_Voltage,

        Battery_Status.Input_Voltage,

        (unsigned long)Battery_Status.Last_Read_Time,

        (unsigned long)elapsed,

        Battery_Status.fault ? "YES" : "NO"
    );
}


/*******************************************************************************
 * Function Name : Task_Id_For_Adc_Print
 * Description   : Registers the periodic ADC print task if it has not
 *                 already been registered.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = registered or active,
 *                 false = registration failed)
 ******************************************************************************/
static bool Task_Id_For_Adc_Print(void)
{
    if (status_print_task_id < 0)
    {
        status_print_task_id =
            Task_Timer_Register(
                3000U,
                Print_Adc_Data
            );

        if (status_print_task_id < 0)
        {
            return false;
        }
    }

    return true;
}


/*******************************************************************************
 * Function Name : Enable_Adc_Print_Task
 * Description   : Enables the periodic ADC status print task.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : bool (true = enabled, false = failed)
 ******************************************************************************/
bool Enable_Adc_Print_Task(void)
{
    return Task_Id_For_Adc_Print();
}


/*******************************************************************************
 * Function Name : Update_Adc
 * Description   : Updates the battery ADC measurement and low-voltage status.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Update_Adc(void)
{
    Battery_Voltage_Update();
}


/*******************************************************************************
 * Function Name : Get_Adc_Data
 * Description   : Provides the latest 12 V battery voltage and low-voltage
 *                 status to the application.
 * Scope         : Global
 * Parameters    : Adc_Data - Pointer to ADC data structure
 * Return Value  : None
 ******************************************************************************/
void Get_Adc_Data(ADC_Data_t *Adc_Data)
{
    if (Adc_Data == NULL)
    {
        return;
    }

    Adc_Data->Voltage_12v =
        Battery_Status.Input_Voltage;

    Adc_Data->Battery_Low_12V =
        Battery_Status.fault;
}
/*******************************************************************************
 * Function Name : Analog_To_Digital_Converter_Init
 * Description   : Initializes ADC1 and clears the ADC result and battery
 *                 voltage fault status.
 * Scope         : Global
 * Parameters    : None
 * Return Value  : bool (true = initialization successful,
 *                 false = initialization failed)
 ******************************************************************************/
bool Analog_To_Digital_Converter_Init(void)
{
    for (uint8_t i = 0U; i < NUM_ADC_CHANNELS; i++)
    {
        ADC_Results[i] = 0U;
    }

    Battery_Status.Raw_Adc = 0U;
    Battery_Status.Pin_Voltage = 0.0f;
    Battery_Status.voltage = 0.0f;
    Battery_Status.Input_Voltage = 0.0f;
    Battery_Status.Last_Read_Time = 0U;
    Battery_Status.Low_Voltage_Since = 0U;
    Battery_Status.fault = false;

    return ADC_Init(ADC1);
}
