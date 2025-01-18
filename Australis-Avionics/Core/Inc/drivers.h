#ifndef _DRIVERS_H
#define _DRIVERS_H

#include "stm32f439xx.h"

void configure_RCC_APB1(void);
void configure_RCC_APB2(void);
void configure_RCC_AHB1(void);
void configure_MISC_GPIO(void);
void configure_UART3_GPS(void);
void configure_SPI3_LoRa();
void configure_UART6_Serial_2_mini_USB(void);
void configure_external_interrupts(void);
void EXTI1_IRQHandler(void);

void TIM6init(void);
void TIM7init(void);
void buzzer(int count);
#endif
