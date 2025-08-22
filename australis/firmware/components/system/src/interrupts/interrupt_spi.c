/***********************************************************************************
 * @file                                                                           *
 * @brief                                                                          *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "system.h"

#include "interrupts/interrupts.h"
#include "interrupts/interrupt_spi.h"
#include "interrupts/_interrupt_spi.h"

void SPI_RegisterIRQ(InterruptIdentifier interrupt, InterruptContext context) {
  ASSERT(interrupt >= INTERRUPT_SPI1 && interrupt <= INTERRUPT_SPI3);

  switch (interrupt) {
  case INTERRUPT_SPI1:
    SPI1_IRQContext = context;
    break;
  case INTERRUPT_SPI2:
    SPI2_IRQContext = context;
    break;
  case INTERRUPT_SPI3:
    SPI3_IRQContext = context;
    break;
  default:
    break;
  }
}

void SPI1_IRQHandler(void) {
  SPI1_IRQContext.callback(&SPI1_IRQContext.context);
}

void SPI2_IRQHandler(void) {
  SPI2_IRQContext.callback(&SPI2_IRQContext.context);
}

void SPI3_IRQHandler(void) {
  SPI3_IRQContext.callback(&SPI3_IRQContext.context);
}

/** @} */
