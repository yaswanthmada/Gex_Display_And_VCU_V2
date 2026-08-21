/*
 * TASK_TIMER.h
 *
 *  Created on: May 4, 2026
 *      Author: MadaYaswanth
 */

#ifndef INC_TASK_TIMER_H_
#define INC_TASK_TIMER_H_



#include <stdint.h>
#include"SYSTICK.h"

typedef void (*TaskFunction_t)(void);

typedef struct {
    uint32_t period_ms;
    uint32_t last_run_ms;
    TaskFunction_t task_fn;
    uint8_t enabled;
    uint8_t is_one_shot;
} TaskTimer_t;

/* Max number of tasks that can be registered */
#define MAX_TASKS 20



/*******************************************************************************
 * Function Name : task_timer_init
 * Description   : Clears all task timer slots in the internal array, resetting
 *                 flags, periods, and function pointers to default state.
 * Scope         : Global / Public
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Task_Timer_Init(void);

/*******************************************************************************
 * Function Name : task_timer_register
 * Description   : Finds the first available timer slot and registers a recurring
 *                 task callback with a specified period in milliseconds.
 * Scope         : Global / Public
 * Parameters    : period_ms - Interval between task executions in milliseconds
 *                 task_fn   - Pointer to function callback (TaskFunction_t)
 * Return Value  : int (Registered task ID index on success, -1 if pool is full)
 ******************************************************************************/
int Task_Timer_Register(uint32_t period_ms, TaskFunction_t task_fn);

/*******************************************************************************
 * Function Name : task_timer_schedule_once
 * Description   : Registers a one-shot task callback scheduled to execute once
 *                 after a specified delay in milliseconds.
 * Scope         : Global / Public
 * Parameters    : delay_ms - Delay duration before execution in milliseconds
 *                 task_fn  - Pointer to function callback (TaskFunction_t)
 * Return Value  : int (Scheduled task ID index on success, -1 if pool is full)
 ******************************************************************************/
int Task_Timer_Schedule_Once(uint32_t delay_ms, TaskFunction_t task_fn);

/*******************************************************************************
 * Function Name : task_timer_delay
 * Description   : Schedules a one-shot non-blocking delay using a dummy callback
 *                 to mark time progression within the scheduler.
 * Scope         : Global / Public
 * Parameters    : delay_ms - Delay duration in milliseconds
 * Return Value  : int (Scheduled task ID index on success, -1 if pool is full)
 ******************************************************************************/
int Task_Timer_Delay(uint32_t delay_ms);

/*******************************************************************************
 * Function Name : task_timer_run_all
 * Description   : Iterates through registered task timers, executes ready
 *                 callbacks based on current tick count, and auto-deletes
 *                 one-shot tasks upon completion.
 * Scope         : Global / Public
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
void Task_Timer_Run_All(void);
/*******************************************************************************
 * Function Name : task_timer_disable
 * Description   : Pauses execution of a registered task timer without freeing
 *                 its allocated slot.
 * Scope         : Global / Public
 * Parameters    : task_id - Target task slot index to disable (int)
 * Return Value  : None
 ******************************************************************************/
void Task_Timer_Disable(int task_id);

/*******************************************************************************
 * Function Name : task_timer_enable
 * Description   : Resumes execution of a paused task timer and resets its
 *                 last-run timestamp to the current tick.
 * Scope         : Global / Public
 * Parameters    : task_id - Target task slot index to enable (int)
 * Return Value  : None
 ******************************************************************************/
void Task_Timer_Enable(int task_id);

/*******************************************************************************
 * Function Name : task_timer_delete
 * Description   : Completely unregisters a task timer, resetting all slot data
 *                 and freeing it for future registration.
 * Scope         : Global / Public
 * Parameters    : task_id - Target task slot index to delete (int)
 * Return Value  : None
 ******************************************************************************/
void Task_Timer_Delete(int task_id);

/*******************************************************************************
 * Function Name : task_timer_get_tick
 * Description   : Wrapper function retrieving current system tick time in
 *                 milliseconds from underlying HAL layer.
 * Scope         : Global / Public
 * Parameters    : None
 * Return Value  : uint32_t (Current system tick value in milliseconds)
 ******************************************************************************/
uint32_t Task_Timer_Get_Tick(void);

bool Enable_Task_Timer_Init();

#endif /* INC_TASK_TIMER_H_ */
