/***********************************************************************************
 * @file        AV2s.c                                                             *
 * @author      Matt Ricci                                                         *
 ***********************************************************************************/

#include "stm32f439xx.h"

#include "AustralisCore.h"
#include "devices.h"
#include "rcc.h"
#include "tasks.h"

void initRCC() {
  // Make sure all peripherals we will use are enabled
  RCC_START_PERIPHERAL(AHB1, GPIOA);
  RCC_START_PERIPHERAL(AHB1, GPIOB);
  RCC_START_PERIPHERAL(AHB1, GPIOC);
  RCC_START_PERIPHERAL(AHB1, GPIOD);
  RCC_START_PERIPHERAL(AHB1, GPIOE);
  RCC_START_PERIPHERAL(AHB1, GPIOF);
  RCC_START_PERIPHERAL(APB2, SPI1);
  RCC_START_PERIPHERAL(APB1, TIM6);
  RCC_START_PERIPHERAL(APB2, SYSCFG);
}

void delayPostInit() {
  // Delay to ensure time for device POR
  TIM2->ARR &= ~TIM_ARR_ARR; // Clear ARR
  TIM2->PSC &= ~TIM_PSC_PSC; // Clear PSC
  TIM2->PSC |= 335;          // TIM2 clock = f_ck/(PSC + 1) = 84MHz/335 = ~250kHz
  TIM2->ARR |= 49999;        // Auto reload set for 49999 x (1/250kHz) = 200ms
  TIM2->CR1 |= TIM_CR1_CEN;  // Enable timer
}

int main() {

  // Wait 100ms before device init
  delayPostInit();

  // Initialise RCC
  initRCC();

  // Initialise device drivers
  initDevices();

  // Bring up core
  Australis_init();

  // Initialise extra tasks
  initTasks();

  // Start the core
  //
  // This runs the FreeRTOS scheduler
  // and will never return.
  Australis_startCore();

  return 0;
}
