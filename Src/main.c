#include "stm32f411xe.h"
#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "led.h"
/*
 * Each Task will have its own Stack
 * MSP is used by scheduler; Scheduler runs in Handler Mode
 * PSP is used by user tasks; All tasks run in Thread Mode
 * @Note: Handler mode always uses MSP
 * @Note: Cortex M Stack is full Descending stack
 * @Note: Stack starts at the end of SRAM
 *
 */

/*
 * User Tasks functions
 */
void task1_handler(void);  //This is task1
void task2_handler(void);  //This is task2
void task3_handler(void);  //This is task3
void task4_handler(void);  //This is task4
void idle_task(void);      //Idle task



void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) void init_sched_stack(uint32_t sched_top_of_stack);
void init_task_stack();
void  enable_processor_faults();
__attribute__((naked)) void switch_sp_to_psp(void);
uint32_t get_psp_value(void);
void update_next_task(void);
void save_psp_value(uint32_t current_psp_value);
void task_delay(uint32_t tick_count);
void schedule(void);
void update_global_tick_count(void);
uint8_t current_task = 1; //Notes current task running
uint32_t global_tick_count=0;

typedef struct{
    uint32_t psp_value;
    uint32_t block_count;
    uint8_t current_state;
    void (*task_handler)(void);

}TCB_t;

TCB_t user_tasks[MAX_TASKS];
int main(void){
    enable_processor_faults();
    init_sched_stack(SCHED_STACK_START );
    init_task_stack();
    led_init();
    init_systick_timer(TICK_HZ);
    switch_sp_to_psp();
    task1_handler();

    for (;;){

    }

}
void idle_task(void){
    while (1);

}

void task1_handler(void){
    for (;;){
        GPIOA->ODR ^= GPIO_ODR_ODR_8;
        task_delay(1000);
    }

}
void task2_handler(void){
    for (;;){
            GPIOC->ODR ^= GPIO_ODR_ODR_8;
            task_delay(500);
    }

}

void task3_handler(void){
    for (;;){
            GPIOC->ODR ^= GPIO_ODR_ODR_6;
            task_delay(250);
        }
}

void task4_handler(void){
    for (;;){
            GPIOC->ODR ^= GPIO_ODR_ODR_5;
            task_delay(125);
    }

}

/*
 * @Brief: Initializes systick timer for generating time quanta
 * @Note: Formula for desired SysTick Frequency: PSC_FREQ = HSICLK/CNT_VALUE
 * PSC_FREQ; desired frequency
 * HSI-CLK: Internal RC oscillator frequency
 * CNT_VALUE: Reload value
 *
 */
void init_systick_timer(uint32_t tick_hz){
    uint32_t count_value = HSI_CLOCK/tick_hz -1;
    SysTick->LOAD = count_value&0xFFFFFF;   // LOAD value length<=24bits
    SysTick->VAL &= ~0x00FFFFFF;

    /*Enable Systick Exception and Select clock source*/
    SysTick -> CTRL |= SysTick_CTRL_CLKSOURCE | SysTick_CTRL_TICKINIT;

    //Enable Counter
    SysTick ->CTRL |= SysTick_CTRL_ENABLE;




}
uint32_t get_psp_value(void){
    return user_tasks[current_task].psp_value;
}

void save_psp_value(uint32_t current_psp_value){
    user_tasks[current_task].psp_value= current_psp_value;
}
/*
 *
 *
 */
void update_next_task(void){
    uint32_t state=TASK_BLOCKING_STATE;
    for (int i=0; i<MAX_TASKS; i++){
            current_task++;
            current_task%=MAX_TASKS;
            state = user_tasks[current_task].current_state;
            if ((state==TASK_READY_STATE)&&(current_task!=0)){
                break;
            }

    }
    if (state!=TASK_READY_STATE){
        current_task=0;
    }



}
__attribute__((naked)) void switch_sp_to_psp(){
    //1. Initialize PSP with TASK1 stack address
    //get the value of psp of current task
    __asm volatile("PUSH {LR}");
    __asm volatile("BL get_psp_value");
    //Return value is stored in R0
    __asm volatile("MSR PSP,R0");   //Inialize PSP
    __asm volatile("POP {LR}");
    //2. Change SP  to PSP using CONTROL register
    __asm volatile("MOV R0,#0x02");
    __asm volatile("MSR CONTROL,R0");

    __asm volatile("BX LR");


  }
  void schedule(void){
     SCB->ICSR |= 1U<<28;


  }

  void task_delay(uint32_t tick_count){
    __asm volatile("CPSID I");
    if (current_task){
            user_tasks[current_task].block_count = global_tick_count+tick_count;
            user_tasks[current_task].current_state = TASK_BLOCKING_STATE;
            schedule();

    }
     __asm volatile("CPSIE I");

  }



__attribute__((naked))void PendSV_Handler(void){
     /*Save Context of current task*/
    //1. Get current running task PSP value
   __asm volatile("MRS R0,PSP");
    //2. Use PSP value to store Stack Frame 2 (R4 to R11)
    __asm volatile("STMDB R0!,{R4-R11}");  //Since the Stack is full descending stack, final address is stored to R0

    //3. Save current PSP value
    __asm volatile("PUSH {LR}");
    __asm volatile("BL save_psp_value");  //R0  will be passed as argument according to ARM Procedure for function calls


    /*Retrieve the value of next task*/
    //1. Decide the next task
    __asm volatile("BL update_next_task");
    //2. Get its past psp value
    __asm volatile("BL get_psp_value");

    __asm volatile("POP {LR}");

    //3. Retrieve SF2 of this task
    __asm volatile("LDMIA R0!, {R4-R11}");

    //4. update PSP
    __asm volatile("MSR PSP, R0");
    __asm volatile("BX LR");

}

 void update_global_tick_count(void){
    global_tick_count++;

 }
 void unblock_tasks(void){
    for (uint32_t i=1; i<MAX_TASKS; i++){
        if (user_tasks[i].current_state !=TASK_READY_STATE){
            if(user_tasks[i].block_count==global_tick_count){
                user_tasks[i].current_state =TASK_READY_STATE;
            }
        }
    }

 }
void SysTick_Handler(void){
    update_global_tick_count();
    unblock_tasks();
    //pend PendSV exception
    SCB->ICSR |= 1U<<28;

}

 __attribute__((naked)) void init_sched_stack(uint32_t sched_top_of_stack){
        __asm volatile("MSR MSP,R0");  //RO takes value of first parameter according to Arm Standard
        __asm volatile("BX LR");       // Return back to main

 }

 void init_task_stack(void){
    for (uint32_t i=0; i<MAX_TASKS; i++){
        user_tasks[i].current_state = TASK_READY_STATE;

    }
    user_tasks[0].psp_value = IDLE_STACK_START;
    user_tasks[1].psp_value = T1_STACK_START;
    user_tasks[2].psp_value = T2_STACK_START;
    user_tasks[3].psp_value = T3_STACK_START;
    user_tasks[4].psp_value = T4_STACK_START;


    user_tasks[0].task_handler = idle_task;
    user_tasks[1].task_handler = task1_handler;
    user_tasks[2].task_handler = task2_handler;
    user_tasks[3].task_handler = task3_handler;
    user_tasks[4].task_handler = task4_handler;

    uint32_t *pPSP;

    for (int i=0; i< MAX_TASKS; i++){
        pPSP = (uint32_t*) user_tasks[i].psp_value;

        pPSP--;
        *pPSP = PSR_VALUE;  //Initialize Dummy XPSR value,T bit is 1

        pPSP--;
        *pPSP = (uint32_t)user_tasks[i].task_handler;   //Set PC to task handler address

        pPSP--;
        *pPSP = EXC_RETURN;   //LR value


        for (int j=0; j<13; j++){

          pPSP--;
         *pPSP = 0;   //R0-R12 dummy data


        }
        //store the value of PSP
        user_tasks[i].psp_value = (uint32_t)pPSP;


    }

  }

void  enable_processor_faults(){
        SCB ->SHCSR |= 7<<16;  //Enable Faults
  }

void HardFault_Handler(void){
    printf("This HardFault Handler\n");
  }
void BusFault_Handler(void){
    printf("This BusFault Handler\n");
  }

void MemManage_Handler(void){
    printf("This MemManage Handler\n");
  }
