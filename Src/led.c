/*
 * led.c
 *
 *  Created on: Jul 31, 2023
 *      Author: malef
 */

#include "led.h"
#include "stm32f411xe.h"
void led_init(void){
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;   //Eanble clocks for GPIOA and C

    //Configure outputs
    GPIOA->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->MODER |= (GPIO_MODER_MODER8_0| GPIO_MODER_MODER6_0|  GPIO_MODER_MODER5_0);
}



