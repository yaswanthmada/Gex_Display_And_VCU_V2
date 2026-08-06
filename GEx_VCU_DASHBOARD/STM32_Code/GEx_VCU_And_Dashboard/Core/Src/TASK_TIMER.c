/**
  ******************************************************************************
  * @file           : task_timer.c
  * @brief          : Task timer scheduler implementation
  ******************************************************************************
  */

#include "TASK_TIMER.h"
#include "main.h"
static TaskTimer_t timers[MAX_TASKS];
/*******************************************************************************
 * Function Name : task_timer_do_nothing
 * Description   : Dummy callback routine used by non-blocking delay timers to
 *                 consume a scheduler slot without executing active logic.
 * Scope         : Static (Private to this file)
 * Parameters    : None
 * Return Value  : None
 ******************************************************************************/
static void Task_Timer_Do_Nothing(void);
static void Task_Timer_Do_Nothing(void)
{
}


void Task_Timer_Init(void)
{
    for (int i = 0; i < MAX_TASKS; i++)
    {
        timers[i].enabled = 0;
        timers[i].task_fn = 0;
        timers[i].period_ms = 0;
        timers[i].last_run_ms = 0;
        timers[i].is_one_shot = 0;
    }
}

int Task_Timer_Register(uint32_t period_ms, TaskFunction_t task_fn)
{
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (!timers[i].enabled && timers[i].task_fn == 0)
        {
            timers[i].period_ms = period_ms;
            timers[i].last_run_ms = Get_Tick_Ms();
            timers[i].task_fn = task_fn;
            timers[i].enabled = 1;
            timers[i].is_one_shot = 0;
            return i;
        }
    }
    return -1;
}

int Task_Timer_Schedule_Once(uint32_t delay_ms, TaskFunction_t task_fn)
{
    int task_id = Task_Timer_Register(delay_ms, task_fn);
    if (task_id >= 0)
    {
        timers[task_id].is_one_shot = 1;
    }
    return task_id;
}

int Task_Timer_Delay(uint32_t delay_ms)
{
    return Task_Timer_Schedule_Once(delay_ms, Task_Timer_Do_Nothing);
}

void Task_Timer_Run_All(void)
{
    uint32_t current_tick = Get_Tick_Ms();

    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (timers[i].enabled && timers[i].task_fn != 0)
        {
            if ((current_tick - timers[i].last_run_ms) >= timers[i].period_ms)
            {
                timers[i].last_run_ms = current_tick;
                timers[i].task_fn();

                if (timers[i].is_one_shot)
                {
                    Task_Timer_Delete(i);
                }
            }
        }
    }
}

void Task_Timer_Disable(int task_id)
{
    if (task_id >= 0 && task_id < MAX_TASKS)
    {
        timers[task_id].enabled = 0;
    }
}

void Task_Timer_Enable(int task_id)
{
    if (task_id >= 0 && task_id < MAX_TASKS)
    {
        timers[task_id].enabled = 1;
        timers[task_id].last_run_ms = Get_Tick_Ms();
    }
}

void Task_Timer_Delete(int task_id)
{
    if (task_id >= 0 && task_id < MAX_TASKS)
    {
        timers[task_id].enabled = 0;
        timers[task_id].task_fn = 0;
        timers[task_id].period_ms = 0;
        timers[task_id].last_run_ms = 0;
        timers[task_id].is_one_shot = 0;
    }
}

uint32_t Task_Timer_Get_Tick(void)
{
    return Get_Tick_Ms();
}
