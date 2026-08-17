/*
 * GPIO.c
 *
 *  Created on: Aug 7, 2026
 *      Author: MadaYaswanth
 */


#include"main.h"
#include"GPIO.h"


static const Digital_Input_Pin Digital_Input_Pins[] =
{
    { HAND_BRAKE_PORT,      HAND_BRAKE_PIN,      GPIO_INPUT_PULL_UP },
    { BRAKE_FLUID_PORT,     BRAKE_FLUID_PIN,     GPIO_INPUT_PULL_UP },
    { HEAD_LIGHT_PORT,      HEAD_LIGHT_PIN,      GPIO_STATE_LOW     },
    { CHARGE_ACK_PORT,      CHARGE_ACK_PIN,      GPIO_STATE_LOW     },
    { LEFT_INDICATOR_PORT,  LEFT_INDICATOR_PIN,  GPIO_STATE_LOW     },
    { RIGHT_INDICATOR_PORT, RIGHT_INDICATOR_PIN, GPIO_STATE_LOW     },
    { MPPT_12V_IP_PORT,     MPPT_12V_IP_PIN,      GPIO_STATE_LOW },
};
static const Digital_Output_Pin Digital_Output_Pins[] =
{
    { MPPT_12V_OP_PORT, MPPT_12V_OP_PIN, GPIO_OUTPUT_PUSHPULL, GPIO_SPEED_50MHZ, GPIO_STATE_HIGH },
};

#define NUM_Digital_Input_Pins (sizeof(Digital_Input_Pins) / sizeof(Digital_Input_Pins[0]))
#define NUM_DIGITAL_OUTPUT_PINS (sizeof(Digital_Output_Pins) / sizeof(Digital_Output_Pins[0]))

/***********************************/
static Gpio_Status Gpio_Pin_Sts;
static Debounce_Data_t Debounce_Pins[MAX_DEBOUNCED_INPUTS];
static uint8_t Registered_Pin_Count = 0;
static bool Debounce_Initialized = false;
static int Hand_Brake_Index=-1;
static int Brake_Fluid_Index=-1;
static int Head_Light_Index=-1;
static int Charge_Index=-1;
static int Left_Indicatior_Index=-1;
static int Right_Indicator_Index=-1;
static int Mppt_Index=-1;
static int status_print_task_id=-1;
/*******************************************************************************
 * Function Name : Input_Pin_Init
 * Description   : Enables the corresponding GPIO port clock and configures a
 *                 specified GPIO pin as an input (Floating, Internal Pull-Up,
 *                 or Internal Pull-Down) on STM32F103 family microcontrollers.
 * Scope         : Static (Private to this file)
 * Parameters    : GPIOx        - Pointer to GPIO port register base (e.g., GPIOA, GPIOB)
 *                 Pin_Pos      - Pin position index (0 to 15)
 *                 Pull_Up_Down - Input configuration mode
 *                                (GPIO_INPUT_NO_PULL, GPIO_INPUT_PULL_UP,
 *                                 or GPIO_INPUT_PULL_DOWN)
 * Return Value  : bool (true = Pin successfully initialized, false = Invalid port/pin index)
 ******************************************************************************/
static bool Input_Pin_Init(GPIO_TypeDef *GPIOx, uint16_t Pin_Pos, uint8_t Pull_Up_Down)
{
	if (GPIOx == 0 || Pin_Pos > 15)
	{
		return false;
	}
    if(GPIOx == GPIOA)
    {
    	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    }
	else if (GPIOx == GPIOB)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	}
	else if (GPIOx == GPIOC)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	}
	else if (GPIOx == GPIOD)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	}
     volatile uint32_t *Config_Reg = (Pin_Pos < 8) ? &(GPIOx->CRL) : &(GPIOx->CRH);
	 uint8_t Bit_Shift = (Pin_Pos < 8) ? (Pin_Pos * 4U) : ((Pin_Pos - 8U) * 4U);
	 *Config_Reg &= ~(0x0FU << Bit_Shift);
	  if (Pull_Up_Down == GPIO_INPUT_NO_PULL)
	  {
	       *Config_Reg |= (0x04U << Bit_Shift);
	  }
	  else
	  {
	        *Config_Reg |= (0x08U << Bit_Shift);
	        if (Pull_Up_Down == GPIO_INPUT_PULL_UP)
	        {
	            GPIOx->ODR |= (1U << Pin_Pos);
	        }
	        else
	        {
	            GPIOx->ODR &= ~(1U << Pin_Pos);
	        }
	   }
	  return true;
}
/*******************************************************************************
 * Function Name : Output_Pin_Init
 * Description   : Enables the corresponding GPIO port clock, sets initial state,
 *                 and configures a specified pin as an output (Push-Pull/Open-Drain)
 *                 with specified slew rate speed on STM32F103 microcontrollers.
 * Scope         : Static (Private to this file)
 * Parameters    : GPIOx        - Pointer to GPIO port register base (e.g., GPIOA, GPIOB)
 *                 Pin_Pos      - Pin position index (0 to 15)
 *                 Output_Mode  - Output type (Push-Pull or Open-Drain)
 *                 Output_Speed - Max output speed / slew rate (10MHz, 2MHz, 50MHz)
 *                 Init_State   - Initial logical output state (GPIO_STATE_HIGH / LOW)
 * Return Value  : bool (true = Pin successfully initialized, false = Invalid port/pin index)
 ******************************************************************************/
static bool Output_Pin_Init(GPIO_TypeDef *GPIOx, uint8_t Pin_Pos, uint8_t Output_Mode, uint8_t Output_Speed, uint8_t Init_State)
{
	if (GPIOx == 0 || Pin_Pos > 15)
	{
	  return false;
	}
	    if(GPIOx == GPIOA)
	    {
	    	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	    }
	    else if (GPIOx == GPIOB)
	    {
	    	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	    }
	    else if (GPIOx == GPIOC)
	    {
	    	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	    }
	    else if (GPIOx == GPIOD)
	    {
	    	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	    }
	    if (Init_State == GPIO_STATE_HIGH)
	    {
	        GPIOx->BSRR = (1U << Pin_Pos);
	    }
	    else
	    {
	        GPIOx->BSRR = (1U << (Pin_Pos + 16U));
	    }
	    volatile uint32_t *Config_Reg = (Pin_Pos < 8) ? &(GPIOx->CRL) : &(GPIOx->CRH);
	    uint8_t Bit_Shift = (Pin_Pos < 8) ? (Pin_Pos * 4U) : ((Pin_Pos - 8U) * 4U);
	    uint8_t Config_Val = ((Output_Mode & 0x03U) << 2U) | (Output_Speed & 0x03U);
	    *Config_Reg &= ~(0x0FU << Bit_Shift);
	    *Config_Reg |= ((uint32_t)Config_Val << Bit_Shift);
	    return true;
}
/*******************************************************************************
 * Function Name : Debounce_Get_State
 * Description   : Retrieves the current debounced (stable) logical state of a
 *                 registered digital input pin.
 * Scope         : Static (Private to this file)
 * Parameters    : Pin_index - Array index of the registered input pin
 * Return Value  : bool (true = High/Pressed, false = Low/Unpressed or Invalid index)
 ******************************************************************************/
static bool Debounce_Get_State(int Pin_index)
{
    if (Pin_index < 0 || Pin_index >= Registered_Pin_Count || !Debounce_Pins[Pin_index].Initialized)
    {
        return false;
    }
    return (bool)Debounce_Pins[Pin_index].Stable_State;
}
/*******************************************************************************
 * Function Name : Gpio_Update_Input_States
 * Description   : Polls debounced digital input states for vehicle hardware signals
 *                 (handbrake, brake fluid, charger, lights, ignition, indicators)
 *                 and updates the global GPIO pin status structure with active-high
 *                 or inverted active-low values.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Gpio_Update_Input_States(void)
{
	Gpio_Pin_Sts.Hand_Brake_Sts = Debounce_Get_State(Hand_Brake_Index);
	Gpio_Pin_Sts.Brake_Fluid_Sts = !Debounce_Get_State(Brake_Fluid_Index);
	Gpio_Pin_Sts.Charge_Ack_Sts = !Debounce_Get_State(Charge_Index);
	Gpio_Pin_Sts.Head_Light_Sts= !Debounce_Get_State(Head_Light_Index);
	Gpio_Pin_Sts.Left_Indicator_Sts= !Debounce_Get_State(Left_Indicatior_Index);
	Gpio_Pin_Sts.Right_Indicator_sts = !Debounce_Get_State(Right_Indicator_Index);
	Gpio_Pin_Sts.Mppt_sts = !Debounce_Get_State(Mppt_Index);
}
/*******************************************************************************
 * Function Name : GPIO_Set_Reset_Verify
 * Description   : Checks the current physical state of a GPIO pin, sets or resets
 *                 the output via BSRR if it differs from the desired state, and
 *                 reads back IDR to verify if the output write succeeded.
 * Scope         : Global
 * Parameters    : GPIOx   - Pointer to GPIO port register base (e.g., GPIOA, GPIOB)
 *                 Pin_Pos - Pin position index (0 to 15)
 *                 State   - Target logical state (GPIO_STATE_HIGH or GPIO_STATE_LOW)
 * Return Value  : bool (true = Hardware readback matches target state, false = State mismatch)
 ******************************************************************************/
bool GPIO_Set_Reset_Verify(GPIO_TypeDef *GPIOx, uint16_t Pin_Pos, uint8_t State)
{
    uint16_t pinMask = (1U << Pin_Pos);
    uint8_t currentState = (GPIOx->IDR & pinMask) ? GPIO_STATE_HIGH : GPIO_STATE_LOW;
    if (currentState != State)
    {
        if (State == GPIO_STATE_HIGH)
        {
            GPIOx->BSRR = pinMask;
        }
        else
        {
            GPIOx->BSRR = (pinMask << 16);
        }
    }
    uint8_t actualState = (GPIOx->IDR & pinMask) ? GPIO_STATE_HIGH : GPIO_STATE_LOW;
    return (actualState == State);
}
/*******************************************************************************
 * Function Name : Gpio_Update_Output_Pins
 * Description   : Controls output pin states based on charging acknowledgment status,
 *                 toggling the pedal brake hardware signal and updating the global
 *                 pedal brake status flag upon hardware state verification.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Gpio_Update_Output_Pins()
{

	switch(Gpio_Pin_Sts.Mppt_Ip_Sts)
	{
	case 0:
	{
		if(GPIO_Set_Reset_Verify(MPPT_12V_OP_PORT,MPPT_12V_OP_PIN,GPIO_PIN_RESET))
		{
			Gpio_Pin_Sts.Mppt_Op_Bp_Sts=false;
		}
	}
	break;
	case 1:
	{
		if(GPIO_Set_Reset_Verify(MPPT_12V_OP_PORT,MPPT_12V_OP_PIN,GPIO_PIN_SET))
		{
			Gpio_Pin_Sts.Mppt_Op_Bp_Sts=true;
		}
	}
	break;
	}
}
/*******************************************************************************
 * Function Name : Gpio_Set_Pin
 * Description   : Drives a specific GPIO pin HIGH by writing to the Bit Set/Reset
 *                 Register (BSRR) for atomic output modification.
 * Scope         : Static Inline (Private to header/source unit)
 * Parameters    : GPIOx   - Pointer to GPIO port register base (e.g., GPIOA, GPIOB)
 *                 Pin_Pos - Target pin index (0 to 15)
 * Return Value  : None
 ******************************************************************************/
static inline void Gpio_Set_Pin(GPIO_TypeDef *GPIOx, uint8_t Pin_Pos)
{
    GPIOx->BSRR = (1U << Pin_Pos);
}
/*******************************************************************************
 * Function Name : Gpio_Reset_Pin
 * Description   : Drives a specific GPIO pin LOW by writing to the Bit Reset
 *                 Register (BRR) for atomic output modification.
 * Scope         : Static Inline (Private to header/source unit)
 * Parameters    : GPIOx   - Pointer to GPIO port register base (e.g., GPIOA, GPIOB)
 *                 Pin_Pos - Target pin index (0 to 15)
 * Return Value  : None
 ******************************************************************************/
static inline void Gpio_Reset_Pin(GPIO_TypeDef *GPIOx, uint8_t Pin_Pos)
{
    GPIOx->BRR = (1U << Pin_Pos);
}
/*******************************************************************************
 * Function Name : Gpio_Toggle_Pin
 * Description   : Inverts the current output state of a specified GPIO pin by
 *                 performing an XOR bitwise operation on the Output Data Register (ODR).
 * Scope         : Static Inline (Private to header/source unit)
 * Parameters    : GPIOx   - Pointer to GPIO port register base (e.g., GPIOA, GPIOB)
 *                 Pin_Pos - Target pin index (0 to 15)
 * Return Value  : None
 ******************************************************************************/
static inline void Gpio_Toggle_Pin(GPIO_TypeDef *GPIOx, uint8_t Pin_Pos)
{
    GPIOx->ODR ^= (1U << Pin_Pos);
}
/*******************************************************************************
 * Function Name : Gpio_Read_Pin
 * Description   : Reads and returns the digital input logic level of a specified
 *                 pin from the Input Data Register (IDR).
 * Scope         : Static Inline (Private to header/source unit)
 * Parameters    : GPIOx   - Pointer to GPIO port register base (e.g., GPIOA, GPIOB)
 *                 Pin_Pos - Target pin index (0 to 15)
 * Return Value  : uint8
 * _t (1U = Logic High, 0U = Logic Low)
 ******************************************************************************/
static inline uint8_t Gpio_Read_Pin(GPIO_TypeDef *GPIOx, uint8_t Pin_Pos)
{
    return ((GPIOx->IDR & (1U << Pin_Pos)) != 0U) ? 1U : 0U;
}
/*******************************************************************************
 * Function Name : Digital_Input_Pins_Init
 * Description   : Iterates through the predefined Digital_Input_Pins array and
 *                 initializes each digital input pin's GPIO port clock and
 *                 pull-up/pull-down configuration mode.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = All input pins initialized successfully, false = Initialization failed)
 ******************************************************************************/
static bool Digital_Input_Pins_Init()
{
    for (size_t i = 0; i < NUM_Digital_Input_Pins; i++)
    {
        if (!Input_Pin_Init(Digital_Input_Pins[i].Port,
                             Digital_Input_Pins[i].Pin,
                             Digital_Input_Pins[i].Mode))
        {
            return false;
        }
    }
    return true;
}
/*******************************************************************************
 * Function Name : Digital_Output_Pins_Init
 * Description   : Iterates through the predefined Digital_Output_Pins array and
 *                 initializes each digital output pin's GPIO port clock, output
 *                 mode, slew rate speed, and initial logic state.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = All output pins initialized successfully, false = Initialization failed)
 ******************************************************************************/
static bool Digital_Output_Pins_Init()
{
    for (size_t i = 0; i < NUM_DIGITAL_OUTPUT_PINS; i++)
    {
        if (!Output_Pin_Init(Digital_Output_Pins[i].Port,
        		Digital_Output_Pins[i].Pin,
				Digital_Output_Pins[i].Mode,
				Digital_Output_Pins[i].Speed,
				Digital_Output_Pins[i].Initial_State))
        {
            return false;
        }
    }
    return true;
}
/*******************************************************************************
 * Function Name : Debounce_Read_Pin
 * Description   : Reads the raw physical digital logic level of a GPIO pin via
 *                 the STM32 HAL interface and evaluates if it is set high.
 * Scope         : Static (Private to this file)
 * Parameters    : Pin_data - Pointer to debounce data structure containing GPIO
 *                            port and pin configuration (Debounce_Data_t*)
 * Return Value  : bool (true = Pin is HIGH/Set, false = Pin is LOW/Reset)
 ******************************************************************************/
static bool Debounce_Read_Pin(Debounce_Data_t* Pin_data)
{
        return (HAL_GPIO_ReadPin(Pin_data->Port, Pin_data->Pin) == GPIO_PIN_SET);
}
/*******************************************************************************
 * Function Name : Debounce_Update_Pin
 * Description   : Implements noise filtering and debouncing logic on a digital
 *                 input pin by validating consistent state readings across a
 *                 specified time duration and sample count before committing
 *                 to a new stable state.
 * Scope         : Static (Private to this file)
 * Parameters    : Pin_Data - Pointer to debounce status structure tracking pin
 *                            timers, sample counters, and state machine variables
 *                            (Debounce_Data_t*)
 * Return Value  : None
 ******************************************************************************/
static void Debounce_Update_Pin(Debounce_Data_t* Pin_Data)
{
    bool Reading = Debounce_Read_Pin(Pin_Data);
    uint32_t Current_Time = Get_Tick_Ms();

    Pin_Data->Previous_Stable_State = Pin_Data->Stable_State;
    if (Reading != Pin_Data->Current_State)
    {
        Pin_Data->Last_Debounce_Time = Current_Time;
        Pin_Data->Current_State = Reading;
        Pin_Data->Sample_Count = 0;
        Pin_Data->Debouncing = true;
    }
    if (Pin_Data->Debouncing)
    {
        if (Reading == Pin_Data->Current_State)
        {
            Pin_Data->Sample_Count++;
        }
        else
        {
            Pin_Data->Current_State = Reading;
            Pin_Data->Sample_Count = 1;
            Pin_Data->Last_Debounce_Time = Current_Time;
        }

        uint32_t Debounce_Duration = Pin_Data->Sample_Interval_Ms * Pin_Data->Min_Samples;
        if ((Current_Time - Pin_Data->Last_Debounce_Time) >= Debounce_Duration &&
            Pin_Data->Sample_Count >= Pin_Data->Min_Samples)
        {
            Pin_Data->Stable_State = Pin_Data->Current_State;
            Pin_Data->Debouncing = false;
        }
    }
}
/*******************************************************************************
 * Function Name : Debounce_Init
 * Description   : Clears the registered debounced input pin pool, resets the
 *                 pin counter, and initializes the debounce module module state.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = Debounce module initialized successfully)
 ******************************************************************************/
static bool Debounce_Init(void)
{
    if (Debounce_Initialized == true)
    {
        return true;
    }
    memset(Debounce_Pins, 0, sizeof(Debounce_Pins));
    Registered_Pin_Count = 0;
    Debounce_Initialized = true;
    return true;
}
/*******************************************************************************
 * Function Name : Debounce_Register_Pin
 * Description   : Registers a GPIO input pin into the debounce tracking pool with
 *                 configurable sampling intervals, required stable sample counts,
 *                 and an initial logical state.
 * Scope         : Static (Private to this file)
 * Parameters    : Port               - Pointer to GPIO port register base (GPIO_TypeDef*)
 *                 Pin                - GPIO pin mask or pin position
 *                 initial_state      - Starting logical state assumption (true/false)
 *                 Sample_Interval_Ms - Time interval between consecutive samples in ms
 *                 Min_Samples        - Required consecutive matching samples to validate state
 * Return Value  : uint8_t (Registered pin index on success, -1 / 255 on error)
 ******************************************************************************/
static uint8_t Debounce_Register_Pin(GPIO_TypeDef* Port, uint16_t Pin, bool initial_state,
                         uint16_t Sample_Interval_Ms, uint8_t Min_Samples)
{
    if (!Debounce_Initialized || Port == NULL || Registered_Pin_Count >= MAX_DEBOUNCED_INPUTS)
    {
        return -1;
    }
    uint8_t index = Registered_Pin_Count;
    Debounce_Pins[index].Port = Port;
    Debounce_Pins[index].Pin = Pin;
    Debounce_Pins[index].Last_Debounce_Time = Get_Tick_Ms();
    Debounce_Pins[index].Last_Sample_Time = Get_Tick_Ms();
    Debounce_Pins[index].Sample_Interval_Ms = Sample_Interval_Ms;
    Debounce_Pins[index].Min_Samples = Min_Samples;
    Debounce_Pins[index].Stable_State = initial_state;
    Debounce_Pins[index].Current_State = initial_state;
    Debounce_Pins[index].Previous_Stable_State = initial_state;
    Debounce_Pins[index].Sample_Count = Min_Samples;
    Debounce_Pins[index].Debouncing = false;
    Debounce_Pins[index].Initialized = true;
    Registered_Pin_Count++;
    return index;
}
/*******************************************************************************
 * Function Name : Debounce_Update_All
 * Description   : Iterates through all registered debounced pins and executes
 *                 debounce filtering routines for pins whose sampling interval
 *                 time has elapsed.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Debounce_Update_All(void)
{
    if (!Debounce_Initialized) return;
    uint32_t Current_Time = Get_Tick_Ms();
    for (uint8_t i = 0; i < Registered_Pin_Count; i++)
    {
        if (Debounce_Pins[i].Initialized)
        {
            if ((Current_Time - Debounce_Pins[i].Last_Sample_Time) >= Debounce_Pins[i].Sample_Interval_Ms)
            {
                Debounce_Pins[i].Last_Sample_Time = Current_Time;
                Debounce_Update_Pin(&Debounce_Pins[i]);
            }
        }
    }
}
/*******************************************************************************
 * Function Name : Debounce_Get_Raw_State
 * Description   : Bypasses state filtering to sample and return the immediate,
 *                 unfiltered physical logic level of a registered input pin.
 * Scope         : Static (Private to this file)
 * Parameters    : Pin_index - Array index of the registered input pin
 * Return Value  : bool (true = Pin is currently HIGH, false = Pin is LOW or invalid index)
 ******************************************************************************/
static bool Debounce_Get_Raw_State(int Pin_index)
{
    if (Pin_index < 0 || Pin_index >= Registered_Pin_Count || !Debounce_Pins[Pin_index].Initialized)
    {
        return false;
    }
    return Debounce_Read_Pin(&Debounce_Pins[Pin_index]);
}
/*******************************************************************************
 * Function Name : Debounce_Pins_Init
 * Description   : Initializes the debounce module and registers all core vehicle
 *                 digital input pins (handbrake, brake fluid, ignition, headlight,
 *                 charger ack, indicators) with default sample intervals and
 *                 filtering thresholds.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = All vehicle inputs registered successfully, false = Allocation/Init error)
 ******************************************************************************/
static bool Debounce_Pins_Init()
{
  if(!Debounce_Init())
  {
	  return false;
  }
  Hand_Brake_Index=Debounce_Register_Pin(HAND_BRAKE_PORT,HAND_BRAKE_PIN,GPIO_INPUT_PULL_UP,DEBOUNCE_SLOW_INTERVAL_MS,DEBOUNCE_SLOW_Min_Samples);
  Brake_Fluid_Index=Debounce_Register_Pin(BRAKE_FLUID_PORT,BRAKE_FLUID_PIN,GPIO_INPUT_PULL_UP,DEBOUNCE_SLOW_INTERVAL_MS,DEBOUNCE_SLOW_Min_Samples);
  Head_Light_Index=Debounce_Register_Pin(HEAD_LIGHT_PORT,HEAD_LIGHT_PIN,GPIO_STATE_LOW,DEBOUNCE_SLOW_INTERVAL_MS,DEBOUNCE_SLOW_Min_Samples);
  Charge_Index=Debounce_Register_Pin(CHARGE_ACK_PORT,CHARGE_ACK_PIN,GPIO_STATE_LOW,DEBOUNCE_SLOW_INTERVAL_MS,DEBOUNCE_SLOW_Min_Samples);
  Left_Indicatior_Index=Debounce_Register_Pin(LEFT_INDICATOR_PORT,LEFT_INDICATOR_PIN,GPIO_STATE_LOW,DEBOUNCE_FAST_INTERVAL_MS,DEBOUNCE_FAST_Min_Samples);
  Right_Indicator_Index=Debounce_Register_Pin(RIGHT_INDICATOR_PORT,RIGHT_INDICATOR_PIN,GPIO_STATE_LOW,DEBOUNCE_FAST_INTERVAL_MS,DEBOUNCE_FAST_Min_Samples);
  Mppt_Index=Debounce_Register_Pin(MPPT_12V_IP_PORT,MPPT_12V_IP_PIN,GPIO_STATE_LOW,DEBOUNCE_SLOW_INTERVAL_MS,DEBOUNCE_SLOW_Min_Samples);
  if(Hand_Brake_Index<0 || Brake_Fluid_Index<0 || Mppt_Index<0 || Head_Light_Index<0 ||Charge_Index<0 || Left_Indicatior_Index<0 ||Left_Indicatior_Index<0)
  {
	  return false;
  }
  return true;
}
/*******************************************************************************
 * Function Name : Update_Gpio_Pins
 * Description   : Periodic update task wrapper that processes input signal
 *                 debouncing across all registered pins and syncs the updated
 *                 debounced values into the global GPIO status structure.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Update_Gpio_Pins()
{
	Debounce_Update_All();
	Gpio_Update_Input_States();
	Gpio_Update_Output_Pins();
}
/*******************************************************************************
 * Function Name : Debounce_Print_Status
 * Description   : Formats and outputs diagnostic information over UART for all
 *                 registered debounced pins, including register addresses,
 *                 raw pin states, stable debounced states, and sample counters.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Debounce_Print_Status(void)
{
	Uart_Printf("\r\n=== Simple Debounce Status ===\r\n");
    Uart_Printf("Registered Pins: %d/%d\r\n", Registered_Pin_Count, MAX_DEBOUNCED_INPUTS);
    for (uint8_t i = 0; i < Registered_Pin_Count; i++)
    {
        Debounce_Data_t* Pin = &Debounce_Pins[i];

        if (Pin->Initialized)
        {
            bool raw_state = Debounce_Get_Raw_State(i);
            Uart_Printf("Pin %d: Port=0x%08lX, Pin=0x%04X, Raw=%d, Stable=%d, Debouncing=%d, Samples=%d\r\n",
                            i, (uint32_t)Pin->Port, Pin->Pin, raw_state, Pin->Stable_State,
                            Pin->Debouncing, Pin->Sample_Count);
        }
    }
    Uart_Printf("==============================\r\n");
}
/*******************************************************************************
 * Function Name : Print_Gpio_Data
 * Description   : Formats and prints human-readable status strings over UART
 *                 reflecting the system's vehicle inputs (brakes, ignition,
 *                 lights, charger, indicators).
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Print_Gpio_Data(void)
{
	Debounce_Print_Status();
	Uart_Printf("=============Input_Output_Data=====================\r\n");
	Uart_Printf("Hand Brake      : %s\r\n", !Gpio_Pin_Sts.Hand_Brake_Sts      ? "ENGAGED"  : "RELEASED");
	Uart_Printf("BP_RELAY      : %s\r\n", Gpio_Pin_Sts.Mppt_Op_Bp_Sts     ? "ON"  : "OFF");
	Uart_Printf("Brake Fluid     : %s\r\n", !Gpio_Pin_Sts.Brake_Fluid_Sts     ? "LOW"      : "NORMAL");
	Uart_Printf("MPPT_IP        : %s\r\n", Gpio_Pin_Sts.Mppt_Ip_Sts         ? "ON"       : "OFF");
	Uart_Printf("Head Light      : %s\r\n", Gpio_Pin_Sts.Head_Light_Sts       ? "ON"       : "OFF");
	Uart_Printf("Charge Ack      : %s\r\n", Gpio_Pin_Sts.Charge_Ack_Sts       ? "ACTIVE"   : "INACTIVE");
	Uart_Printf("Left Indicator  : %s\r\n", Gpio_Pin_Sts.Left_Indicator_Sts   ? "ON"       : "OFF");
	Uart_Printf("Right Indicator : %s\r\n", Gpio_Pin_Sts.Right_Indicator_sts  ? "ON"       : "OFF");
	Uart_Printf("====================================================\r\n");
}
/*******************************************************************************
 * Function Name : Task_Id_For_Gpio_Print
 * Description   : Registers a periodic software timer task to print vehicle
 *                 GPIO status over UART every 3000 ms if not already scheduled.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : bool (true = Task already active or successfully registered,
 *                       false = Timer allocation failed)
 ******************************************************************************/
static bool Task_Id_For_Gpio_Print(void)
{
    if (status_print_task_id < 0)
    {
        status_print_task_id = Task_Timer_Register(3000, Print_Gpio_Data);
        if (status_print_task_id < 0)
        {
        	return false;
        }
    }
    return true;
}

void Update_Gpio()
{
	Update_Gpio_Pins();
}
void Get_IO_Data(IO_Data_t* IO_Data)
{
	IO_Data->Brake_Fluid=Gpio_Pin_Sts.Brake_Fluid_Sts;
	IO_Data->Charge_Ack=Gpio_Pin_Sts.Charge_Ack_Sts;
	IO_Data->Hand_Brake=Gpio_Pin_Sts.Hand_Brake_Sts;
	IO_Data->Head_Light=Gpio_Pin_Sts.Head_Light_Sts;
	IO_Data->Left_Ind=Gpio_Pin_Sts.Left_Indicator_Sts;
	IO_Data->Right_Ind=Gpio_Pin_Sts.Right_Indicator_sts;
}
bool Enable_Gpio_Print_Task()
{
	return Task_Id_For_Gpio_Print();
}
bool General_Purpose_Input_Output_Init()
{
   if(!Digital_Input_Pins_Init())
   {
	   return false;
   }
   if(!Digital_Output_Pins_Init())
   {
	   return false;
   }
   if(!Debounce_Pins_Init())
   {
	   return false;
   }
   return true;
}
/* Not Using Now
bool Get_Hand_Brake_Sts(void)
{
	return Gpio_Pin_Sts.Hand_Brake_Sts;
}
bool Get_Peddal_Brake_Sts(void)
{
	return Gpio_Pin_Sts.Peddal_Brake_Sts;
}
bool Get_Charge_Ack_Sts(void)
{
	return Gpio_Pin_Sts.Charge_Ack_Sts;
}
bool Get_Left_Ind_Sts(void)
{
	return Gpio_Pin_Sts.Left_Indicator_Sts;
}
bool Get_Right_Ind_Sts(void)
{
	return Gpio_Pin_Sts.Right_Indicator_sts;
}
bool Get_Head_Light_Sts(void)
{
	return Gpio_Pin_Sts.Head_Light_Sts;
}
bool Get_Ignition_Sts(void)
{
	return Gpio_Pin_Sts.Ignition_Sts;
}
bool Get_Brake_Fluid_Sts(void)
{
	return Gpio_Pin_Sts.Brake_Fluid_Sts;
}
bool Get_Mppt_Sts(void)
{
	return Gpio_Pin_Sts.Mppt_sts;
}
*******************************************************************************
 * Function Name : Debounce_Reset_Pin
 * Description   : Forces a debounced pin's internal tracking states (stable,
 *                 current, and previous) to immediately align with its current
 *                 raw hardware level, clearing active debounce filtering.
 * Scope         : Static (Private to this file)
 * Parameters    : Pin_index - Array index of the registered input pin
 * Return Value  : bool (true = Pin reset successful, false = Invalid index)
 ******************************************************************************
static bool Debounce_Reset_Pin(int Pin_index)
{
    if (Pin_index < 0 || Pin_index >= Registered_Pin_Count || !Debounce_Pins[Pin_index].Initialized)
    {
        return false;
    }
    Debounce_Data_t* Pin = &Debounce_Pins[Pin_index];
    bool Current_Raw = Debounce_Read_Pin(Pin);
    Pin->Stable_State = Current_Raw;
    Pin->Current_State = Current_Raw;
    Pin->Previous_Stable_State = Current_Raw;
    Pin->Sample_Count = Pin->Min_Samples;
    Pin->Debouncing = false;
    Pin->Last_Debounce_Time = Get_Tick_Ms();
    Pin->Last_Sample_Time = Get_Tick_Ms();
    return true;
}*/
/*******************************************************************************
 * Function Name : Debounce_Get_Pin_Count
 * Description   : Returns the total number of input pins currently registered
 *                 and monitored by the debounce module.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : uint8_t (Number of registered debounced pins)
 ******************************************************************************
static uint8_t Debounce_Get_Pin_Count(void)
{
    return Registered_Pin_Count;
}*/
/*******************************************************************************
 * Function Name : Debounce_Rising_Edge
 * Description   : Checks if a debounced input pin has undergone a low-to-high
 *                 transition by evaluating its previous and current stable states.
 * Scope         : Static (Private to this file)
 * Parameters    : Pin_index - Array index of the registered input pin
 * Return Value  : bool (true = Rising edge detected, false = No edge or invalid index)
 ******************************************************************************
static bool Debounce_Rising_Edge(int Pin_index)
{
    if (Pin_index < 0 || Pin_index >= Registered_Pin_Count || !Debounce_Pins[Pin_index].Initialized)
    {
        return false;
    }

    Debounce_Data_t* Pin = &Debounce_Pins[Pin_index];
    return (!Pin->Previous_Stable_State && Pin->Stable_State);
}*/
/*******************************************************************************
 * Function Name : Debounce_Falling_Edge
 * Description   : Checks if a debounced input pin has undergone a high-to-low
 *                 transition by evaluating its previous and current stable states.
 * Scope         : Static (Private to this file)
 * Parameters    : Pin_index - Array index of the registered input pin
 * Return Value  : bool (true = Falling edge detected, false = No edge or invalid index)
 ******************************************************************************
static bool Debounce_Falling_Edge(int Pin_index)
{
    if (Pin_index < 0 || Pin_index >= Registered_Pin_Count || !Debounce_Pins[Pin_index].Initialized)
    {
        return false;
    }
    Debounce_Data_t* Pin = &Debounce_Pins[Pin_index];
    return (Pin->Previous_Stable_State && !Pin->Stable_State);
}
*/
