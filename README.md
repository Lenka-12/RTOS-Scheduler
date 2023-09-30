# Real-Time Operating System (RTOS) Scheduler with Tasks



This project is an implementation of a simple Real-Time Operating System (RTOS) scheduler for an ARM Cortex-M microcontroller (specifically STM32F411xE). The scheduler allows for multitasking and context switching between multiple tasks. Each task has its own stack and is scheduled to run based on time quanta provided by a system tick timer.



## Introduction



The main.c file contains the setup and initialization code for the RTOS scheduler. It defines the user tasks, initializes the system tick timer, and switches the processor stack pointer to the Process Stack Pointer (PSP).



## Tasks



The project defines four user tasks:

1. Task1 (`task1_handler`): Toggles the GPIO pin PA8 at a delay of 1000 ms.

2. Task2 (`task2_handler`): Toggles the GPIO pin PC8 at a delay of 500 ms.

3. Task3 (`task3_handler`): Toggles the GPIO pin PC6 at a delay of 250 ms.

4. Task4 (`task4_handler`): Toggles the GPIO pin PC5 at a delay of 125 ms.



An idle task (`idle_task`) is also included, which runs when no other tasks are ready to execute.



## Scheduler and Context Switching
The scheduler uses a PendSV (Pendable request for system service) interrupt to perform context switching between tasks. The scheduler is implemented in the `PendSV_Handler` function. Context switching is based on the execution states of the tasks (ready or blocked).



## System Tick Timer
The system tick timer generates periodic interrupts, which are used to keep track of time and unblock tasks waiting for a specific delay. The SysTick interrupt is handled in the `SysTick_Handler` function.



## Usage



To use this RTOS scheduler, you need to include the necessary files in your project and configure the GPIO pins according to your hardware setup. Make sure to initialize the stack pointers for each task using the `init_task_stack` function and specify the number of tasks using the `MAX_TASKS` constant.

## License
This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).



## Contact
For any inquiries or questions, please contact [Malefetsane Lenka](mailto:malefetsanelenka@gmail.com).

