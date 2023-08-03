
#ifndef  MAIN_H_
#define  MAIN_H_

#include "core_cm4.h"
/* Stack memory calculations */

#define TASK_STACK_SIZE               1024U
#define SCHED_STACK_SIZE              1024U

#define SRAM_START                    0x20000000U
#define SRAM_SIZE                     ((128)*(1024))
#define SRAM_END                      ((SRAM_START)+(SRAM_SIZE))
/*
 *Task1,2,3,4 and Scheduler stacks start
 */

#define T1_STACK_START                 SRAM_END
#define T2_STACK_START                ((SRAM_END)-(1*TASK_STACK_SIZE))
#define T3_STACK_START                ((SRAM_END)-(2*TASK_STACK_SIZE))
#define T4_STACK_START                ((SRAM_END)-(3*TASK_STACK_SIZE))
#define  IDLE_STACK_START             ((SRAM_END)-(4*TASK_STACK_SIZE))

#define SCHED_STACK_START             ((SRAM_END)-(5*TASK_STACK_SIZE))

#define MAX_TASKS                      5U

#define TICK_HZ                       1000U
#define HSI_CLOCK                     16000000U
#define SysTick_CTRL_CLKSOURCE        SysTick_CTRL_CLKSOURCE_Msk
#define SysTick_CTRL_ENABLE           SysTick_CTRL_ENABLE_Msk
#define SysTick_CTRL_TICKINIT         SysTick_CTRL_TICKINT_Msk

#define PSR_VALUE                     (1UL<<24)
#define EXC_RETURN                    0xFFFFFFFDU
#define TASK_READY_STATE              0x00
#define TASK_BLOCKING_STATE           0xFF


#endif
